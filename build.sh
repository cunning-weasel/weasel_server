#!/bin/bash
echo "Compile option master weasel:"
echo "[1] Debug"
echo "[2] Debug 64-bit"
echo "[3] Regular compile"
echo "[4] Optimized compile"
read -p "Enter your cunning choice: " choice
case $choice in
    1)
        -Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined
        ;;
    2)
        -Wall -Wextra -Wpedantic -Og -g3 -fsanitize=address,undefined -m64
        ;;
    3)
        gcc -o output_weasel_server weasel_server.c
        ;;
    4)
        gcc -o output_hm_weasel hm_weasel.c -O3
        ;;
    *)
        echo "Invalid option"
        ;;
esac
