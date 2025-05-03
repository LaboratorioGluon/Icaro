#!/bin/bash

# Verifica si el usuario proporciona una interfaz como argumento
if [ -z "$1" ]; then
    echo "Uso: $0 <interfaz_wifi>"
    exit 1
fi

INTERFACE=$1

# Apagar la interfaz
sudo ip link set $INTERFACE down

# Configurar en modo monitor
sudo iw dev $INTERFACE set type monitor

# Encender la interfaz
sudo ip link set $INTERFACE up

echo "La interfaz $INTERFACE ahora está en modo monitor."

# Verificar el estado
iw dev $INTERFACE info

