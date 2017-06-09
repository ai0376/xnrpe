CC=gcc
CXX=g++
SRC_DIR=.
INC_DIR=.

all: check_disk_io check_cpu check_mem check_version check_swap check_proc check_ifport check_disk_sys

check_disk_io: $(SRC_DIR)/check_disk_io.c 
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_disk_io.c

check_cpu: $(SRC_DIR)/check_cpu.c
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_cpu.c

check_mem: $(SRC_DIR)/check_mem.c
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_mem.c

check_version: $(SRC_DIR)/check_version.c
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_version.c

check_swap: $(SRC_DIR)/check_swap.c
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_swap.c

check_proc: $(SRC_DIR)/check_proc.c
	$(CXX) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_proc.c

check_ifport: $(SRC_DIR)/check_ifport.c
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_ifport.c

check_disk_sys: $(SRC_DIR)/check_disk_sys.c
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_disk_sys.c
clean:
	rm -rf *.o check_disk_io check_cpu check_mem check_version check_swap check_proc check_ifport check_disk_sys
