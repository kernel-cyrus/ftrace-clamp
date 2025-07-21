CC := gcc
CFLAGS := -Wall -O2 -DBUILD_AS_EXECUTABLE

TARGET := ftrace_clamp

SRC := ftrace_clamp.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)