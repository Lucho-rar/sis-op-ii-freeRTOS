
#!/bin/bash
# debug.sh - lanzar QEMU y GDB para depuración

# Salir si hay algún error
set -e

# Ruta del kernel
KERNEL="gcc/RTOSDemo.axf"

echo "Iniciando QEMU en modo espera de GDB..."
# QEMU corre en background
qemu-system-arm -M lm3s811evb -kernel "$KERNEL" -S -s -serial stdio &

# Guardamos el PID de QEMU por si queremos matarlo después
QEMU_PID=$!

echo "Abriendo GDB en otra terminal..."
# Abrir GDB en otra terminal y conectar al puerto 1234
gnome-terminal -- bash -c "gdb-multiarch '$KERNEL' -ex 'target remote :1234'; exec bash"

# Esperar a que QEMU termine si se cierra solo
wait $QEMU_PID
