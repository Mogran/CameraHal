CC=gcc
CPP=g++
RM= rm -rf

SRC_PATH := $(shell pwd)
OUTPUT := $(shell ls -l | grep ^d | awk '{if($$9 == output) print $$9}')
TARGET := test

SRCS += $(wildcard $(SRC_PATH)/lib/*.c)
SRCS += $(wildcard $(SRC_PATH)/*.c)

OBJS := $(patsubst %.c, %.o, $(SRCS))

CFLAGS += -Wall -g -Iinclude

all:clean build

build:
	$(CC) -c $(CFLAGS) $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	$(RM) $(OBJS)

clean:
	$(RM) $(OBJS) 
	$(RM) $(TARGET)
