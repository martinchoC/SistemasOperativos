clear
gcc principal.c -o principal -pthread
gcc lectores.c -o lectores -pthread
gcc escritores.c -o escritores -pthread
./principal