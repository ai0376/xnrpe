CC=gcc
SRC_DIR=.
INC_DIR=.

all: sample_server

xnrpe: $(SRC_DIR)/sample_server.c 
	$(CC) -I $(INC_DIR) -o $@ $(SRC_DIR)/sample_server.c

clean:
	rm -rf *.o sample_server


