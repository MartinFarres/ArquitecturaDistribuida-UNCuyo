#!/bin/bash

# Ruta de la carpeta a copiar
SOURCE_DIR="/home/mpiuser/martin_farres"
# Archivo que contiene las IPs
IP_FILE="ips.txt"

# Comprobamos si la carpeta existe
if [ ! -d "$SOURCE_DIR" ]; then
    echo "La carpeta $SOURCE_DIR no existe."
    exit 1
fi

# Comprobamos si el archivo de IPs existe
if [ ! -f "$IP_FILE" ]; then
    echo "El archivo $IP_FILE no existe."
    exit 1
fi

# Obtener la IP local
LOCAL_IP=$(hostname -I | awk '{print $1}')

# Loop a través de cada IP en el archivo
while IFS= read -r ip; do
    # Verificar que la línea no esté vacía y que no sea la IP local
    if [ -n "$ip" ] && [ "$ip" != "$LOCAL_IP" ]; then
        echo "Copiando la carpeta a $ip..."
        scp -r "$SOURCE_DIR" mpiuser@"$ip":/home/mpiuser/
        if [ $? -ne 0 ]; then
            echo "Error al copiar a $ip"
        fi
    else
        echo "Omitiendo la IP local: $ip"
    fi
done < "$IP_FILE"
