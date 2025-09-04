
#!/bin/bash
# format.sh - aplica clang-format solo a src/ e include/ del directorio raíz

set -e

ROOT=$(pwd)   # directorio raíz actual
DIRS=("src" "include")

for DIR in "${DIRS[@]}"; do
    FULLPATH="$ROOT/$DIR"
    if [ -d "$FULLPATH" ]; then
        echo "Formateando archivos en $FULLPATH..."
        # Solo recorre recursivamente este directorio específico
        find "$FULLPATH" -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format -i -style=file {} +
    fi
done

echo "¡Formateo finalizado!"
