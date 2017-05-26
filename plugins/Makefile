CC=gcc
SRC_DIR=.
INC_DIR=.

all: check_disk_io

xnrpe: $(SRC_DIR)/check_disk_io
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_disk_io.c

clean:
	rm -rf *.o check_disk_io
