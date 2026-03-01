CC = gcc
CFLAGS = -Wall -O2
TARGET = datagen
SRCS = datagen.c Lab3IO.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)