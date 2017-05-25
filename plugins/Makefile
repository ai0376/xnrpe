CC=gcc
SRC_DIR=.
INC_DIR=.

all: check_disk

xnrpe: $(SRC_DIR)/check_disk
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/check_disk.c

clean:
	rm -rf *.o check_disk
