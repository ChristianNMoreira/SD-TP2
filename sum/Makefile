CC = gcc

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall -Werror -D_POSIX_THREAD_SEMANTICS
LDFLAGS = -pthread -lm

# the build target executable:
TARGET = sum

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	$(RM) $(TARGET)