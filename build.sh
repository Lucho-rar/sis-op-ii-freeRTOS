#!/bin/bash
# run.sh - script para limpiar y compilar el proyecto

set -e

echo "Limpiando build anterior..."
make clean

echo "Compilando proyecto..."
make

echo "¡Compilación finalizada!"
