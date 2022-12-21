CC=gcc
CFLAGS=
SRC=$(wildcard *.c)
OBJS=$(SRC:%.c=%.o)
TARGET=basic

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

$(OBJS): $(SRC)
	$(CC) -c $(SRC)

clean:
	rm -f $(TARGET) $(OBJS)
