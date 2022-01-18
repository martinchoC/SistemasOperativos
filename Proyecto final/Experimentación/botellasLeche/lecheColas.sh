clear
gcc lecheColas.c -o lecheColas -lrt -pthread
ipcrm --all=msg
./lecheColas