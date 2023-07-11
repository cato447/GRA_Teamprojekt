TARGET = program

CC = gcc
CFLAGS = -O3 -g -std=c17 -Wall -Wpedantic -Wextra -msse4.2

OUTDIR = ./build
SUBDIR = src src/Implementierung src/IOSystem src/Testsystem
DIR_OBJ = ./obj
LIBS = -lm -lrt -pthread

INCS = $(wildcard *.h $(foreach path, $(SUBDIR), $(path)/*.h))
SRCS = $(wildcard *.c $(foreach path, $(SUBDIR), $(path)/*.c))
OBJS = $(addprefix $(DIR_OBJ)/, $(SRCS:c=o))
INC_DIRS = -I./ $(addprefix -I, $(SUBDIR))

PHONY := $(TARGET)
$(TARGET): $(OBJS)
	mkdir -p $(OUTDIR)
	$(CC) -o $(OUTDIR)/$@ $(OBJS) $(LIBS)

$(DIR_OBJ)/%.o: %.c $(INCS)
	mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) -c $< $(INC_DIRS)

PHONY += clean
clean:
	rm -rf $(OUTDIR)/* $(DIR_OBJ)/*
.PHONY = $(PHONY)
