CC=gcc
SRC_DIR=.
INC_DIR=.

all: xnrpe

xnrpe: $(SRC_DIR)/xnrpe.c $(SRC_DIR)/xcommon.c $(INC_DIR)/xcommon.h
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/xnrpe.c $(SRC_DIR)/xcommon.c -lpthread

clean:
	rm -rf *.o xnrpe


