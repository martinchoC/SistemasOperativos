clear
gcc principal.c -o principal -pthread
gcc consumidor.c -o consumidor -pthread
gcc productor.c -o productor -pthread
./principal