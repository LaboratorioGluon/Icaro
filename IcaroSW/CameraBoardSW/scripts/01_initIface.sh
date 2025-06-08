#!/bin/bash

# Read arguments
if [ -z "$1" ]; then
    echo "Usage: $0 <wifi_interface>"
    exit 1
fi
INTERFACE=$1
MON_INTERFACE=${INTERFACE}mon
CHANNEL=13

# Configure monitor mode for interface
sudo ip link set $INTERFACE down
sudo iw dev $INTERFACE set type monitor
sudo ip link set $INTERFACE up

# Reconfigure the interface in monitor mode
#   This helps to allow wireshark to change the channel of the interface
sudo airmon-ng check kill
sudo airmon-ng start $INTERFACE

# Configure channel to ESP32 channel
sudo iwconfig ${MON_INTERFACE} channel ${CHANNEL}

# Print current mode of interface
echo "The ${MON_INTERFACE} should be in monitor mode."
echo "Please verify before continuing:"
iw dev ${MON_INTERFACE} info

