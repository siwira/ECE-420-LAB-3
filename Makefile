CC = gcc
CFLAGS = -Wall -O2
TARGET = datagen
SRCS = datagen.c Lab3IO.c
MAIN = main
MAIN_SRCS = main.c Lab3IO.c
NUM_THREADS = 4

all: $(TARGET) $(MAIN)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

$(MAIN): $(MAIN_SRCS)
	$(CC) $(CFLAGS) $(MAIN_SRCS) -o $(MAIN)

run: all
	./$(TARGET) -s 3 -p
	./$(MAIN) $(NUM_THREADS)

clean:
	rm -f $(TARGET) $(MAIN)