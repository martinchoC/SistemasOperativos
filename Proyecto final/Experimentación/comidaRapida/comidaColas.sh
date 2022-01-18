clear
gcc comidaColas.c -o comida -lrt -pthread
ipcrm --all=msg
./comida