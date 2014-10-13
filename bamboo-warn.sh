#!/bin/bash


show_help() {
cat << EOF
Usage: ${0##*/} [FLAGG] [URL]
Poll Bamboo for status updates.

    -h          display help message
    -i          seconds between each update
    -b          blink count on build failure
    -u          bamboo username
    -p          bamboo password
EOF
}

# Default values
pollInterval=60 # Seconds
httpErrorFlashCount=1
buildErrorFlashCount=3


while getopts "hu:i:b:u:p:" opt; do
    case $opt in
        h)
            show_help
            exit 0
            ;;
        i)
            pollInterval=$OPTARG
            ;;
        b)
            buildErrorFlashCount=$OPTARG
            ;;
        u)
            username=$OPTARG
            ;;
        p)
            password=$OPTARG
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit 1
            ;;
        *)
            echo "Unknown option -$OPTARG." >&2
            exit 1
            ;;
    esac
done

uri=${@:$OPTIND:1}



if [ -z "$password" ]
then
    read -s -p "Password: " password
fi



function poll {
    HTML=$(curl --insecure -s --fail -X GET -u "$username:$password" -H "Content-Type: application/json" $uri)

    # Verify curl exit code
    if [ $? -eq 0 ]; then
        FAILED=$(echo $HTML | xmllint --format --xpath 'contains((//item/title)[1]/text(), "FAILED")' -)

        if [ "$FAILED" == "true" ]; then
            ./flashing.sh $buildErrorFlashCount
        fi
    else
        ./flashing.sh $httpErrorFlashCount
    fi
}

while :
do
    poll
    sleep $pollInterval
done
