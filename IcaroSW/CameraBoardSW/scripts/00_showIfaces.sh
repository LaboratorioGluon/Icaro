#!/bin/bash

# Listar interfaces WiFi disponibles
interfaces=$(iw dev | grep Interface | awk '{print $2}')

if [ -z "$interfaces" ]; then
    echo "No se encontraron interfaces WiFi."
    exit 1
fi

echo "Interfaces WiFi disponibles:"
echo "$interfaces"

