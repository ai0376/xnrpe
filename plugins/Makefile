CC=gcc
CXX=g++
SRC_DIR=.
INC_DIR=.

all: check_disk_io check_cpu check_mem check_version check_swap check_proc check_ifport check_disk_sys \
	check_informix_basic  check_informix_chunk_io check_informix_chunk check_informix_dbspace check_informix_log check_informix_logbuff \
	check_informix_runstat check_informix_sharemem

check_disk_io: $(SRC_DIR)/check_disk_io.c 
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_disk_io.c

check_cpu: $(SRC_DIR)/check_cpu.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_cpu.c

check_mem: $(SRC_DIR)/check_mem.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_mem.c

check_version: $(SRC_DIR)/check_version.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_version.c

check_swap: $(SRC_DIR)/check_swap.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_swap.c

check_proc: $(SRC_DIR)/check_proc.c
	$(CXX) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_proc.c

check_ifport: $(SRC_DIR)/check_ifport.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_ifport.c

check_disk_sys: $(SRC_DIR)/check_disk_sys.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_disk_sys.c

check_informix_basic: $(SRC_DIR)/check_informix_basic.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_informix_basic.c

check_informix_chunk_io: $(SRC_DIR)/check_informix_chunk_io.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_informix_chunk_io.c

check_informix_chunk: $(SRC_DIR)/check_informix_chunk.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_informix_chunk.c

check_informix_dbspace: $(SRC_DIR)/check_informix_dbspace.c
	$(CXX) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_informix_dbspace.c

check_informix_log: $(SRC_DIR)/check_informix_log.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_informix_log.c

check_informix_logbuff: $(SRC_DIR)/check_informix_logbuff.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_informix_logbuff.c

check_informix_runstat: $(SRC_DIR)/check_informix_runstat.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_informix_runstat.c

check_informix_sharemem: $(SRC_DIR)/check_informix_sharemem.c
	$(CC) -Wall -I $(INC_DIR) -o $@ $(SRC_DIR)/check_informix_sharemem.c

clean:
	rm -rf *.o check_disk_io check_cpu check_mem check_version check_swap check_proc check_ifport check_disk_sys \
	check_informix_basic  check_informix_chunk_io check_informix_chunk check_informix_dbspace check_informix_log check_informix_logbuff \
	check_informix_runstat check_informix_sharemem
