DISK = disk
FILE = io
TESTS = apps

diskTest: $(TESTS)/createDiskTest.o $(FILE)/File.o $(DISK)/disk.o
	gcc -Wall -Werror -o $(TESTS)/createDiskTest $(TESTS)/createDiskTest.o $(FILE)/File.o $(DISK)/disk.o
writeTest: $(TESTS)/writeSomeFilesTest.o $(FILE)/File.o $(DISK)/disk.o
	gcc -Wall -Werror -o $(TESTS)/writeSomeFilesTest $(TESTS)/writeSomeFilesTest.o $(FILE)/File.o $(DISK)/disk.o
disk.o: $(DISK)/disk.c
	gcc -Wall -Werror -c $(DISK)/disk.c
File.o: $(FILE)/File.c
	gcc -Wall -Werror -c $(FILE)/File.c
createDisk.o: $(TESTS)/createDiskTest.c
	gcc -Wall -Werror -c $(TESTS)/createDiskTest.c
writeSomeFilesTest.o: $(TESTS)/writeSomeFilesTest.c
	gcc -Wall -Werror -c $(TESTS)/writeSomeFilesTest.c
