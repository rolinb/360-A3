DISK = ../disk

File: File.o $(DISK)/disk.o
	gcc -Wall -Werror -o File File.o $(DISK)/disk.o
disk.o: $(DISK)/disk.c
	gcc -Wall -Werror -c $(DISK)/disk.c
File.o: File.c
	gcc -Wall -Werror -c File.c
