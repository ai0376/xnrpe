CC=gcc
SRC_DIR=.
INC_DIR=.
CJSON_DIR=../libs/cjson
LIB_SRC= $(CJSON_DIR)/cJSON.c  $(CJSON_DIR)/cJSON_Utils.c

all: xnrpe

xnrpe: $(SRC_DIR)/xnrpe.c $(SRC_DIR)/xcommon.c $(INC_DIR)/xcommon.h $(LIB_SRC)
	$(CC) -I $(INC_DIR) -I $(CJSON_DIR) -o $@ $(SRC_DIR)/xnrpe.c $(SRC_DIR)/xcommon.c  $(LIB_SRC) -lpthread

clean:
	rm -rf *.o xnrpe


