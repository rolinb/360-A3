General Readme stuff

Most of the Inode stuff is in a separate file because it has the math working it out.

Currently done:
create disk
pre-set disk:
  make super block
  make used/free block
  make inodeMap
reserve block 10 for root directory
Create file, write data to file, read file
3 tests 1 creates disk 1 makes some field another reads those files


Changes:
Directory structure will use 4 for inode 28 for filename/directory name
otherwise there are only 255 inodes and the rest was done prior to this

To Do - no time for this finals and interview took up too much time:
sub directories?
read write in other directories?
deletion. - freeing block exists



cache/robustness:
because of time constraints related to other classes, finals, interviews there is no caching
everything is written to disk as it goes.
This increases robustness at the cost of speed.
