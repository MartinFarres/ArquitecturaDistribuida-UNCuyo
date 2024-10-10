#!/bin/bash

# Define the file containing the IP addresses
IP_FILE="ips.txt"

# Check if the file exists
if [ ! -f "$IP_FILE" ]; then
    echo "File $IP_FILE does not exist."
    exit 1
fi

# Loop through each IP in the file and share the public key
while IFS= read -r ip; do
    if [[ ! -z "$ip" ]]; then
        echo "Sharing public key with $ip..."
        ssh-copy-id mpiuser@"$ip"
    fi
done < "$IP_FILE"
