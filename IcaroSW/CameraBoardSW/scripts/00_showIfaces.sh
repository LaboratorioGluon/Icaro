#!/bin/bash

# List Wifi interfaces available 
interfaces=$(iw dev | grep Interface | awk '{print $2}')

if [ -z "$interfaces" ]; then
    echo "No WiFi interaces found"
    exit 1
fi

echo "WiFi interfaces:"
echo "$interfaces"

