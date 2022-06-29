BIN:=mqtt_sub
SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ -lubox -luci -lmosquitto -lsqlite3 -lcurl

clean:
	$(RM) $(BIN) $(OBJS)
