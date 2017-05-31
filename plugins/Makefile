CC=gcc
SRC_DIR=.
INC_DIR=.

all: check_disk_io check_cpu

check_disk_io: $(SRC_DIR)/check_disk_io.c 
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_disk_io.c

check_cpu: $(SRC_DIR)/check_cpu.c
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_cpu.c

clean:
	rm -rf *.o check_disk_io check_cpu
