CC=gcc
CFLAGS=-Wall -Wextra -g
M32=-m32
INCLUDES=-I../src/
LDFLAGS=
RM=rm -rf
SOURCES=$(wildcard *.c) $(wildcard ../src/*.c)
TARGET=test
CP=cp
RM=rm -rf

default:
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LDFLAGS) 

m32:
	$(CC) $(CFLAGS) $(M32) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LDFLAGS) 

clean:
	$(RM) $(TARGET)

install:
	$(CP) $(TARGET) /usr/bin/

uninstall:
	$(RM) /usr/bin/$(TARGET)

