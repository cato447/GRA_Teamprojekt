TARGET = program

CC = gcc
CFLAGS = -g -std=c17 -Wall -Wpedantic

OUTDIR = ./build
SUBDIR = src src/Implementierung src/IOSystem src/Testsystem
DIR_OBJ = ./obj
LIBS =

INCS = $(wildcard *.h $(foreach path, $(SUBDIR), $(path)/*.h))
SRCS = $(wildcard *.c $(foreach path, $(SUBDIR), $(path)/*.c))
OBJS = $(addprefix $(DIR_OBJ)/, $(SRCS:c=o))
INC_DIRS = -I./ $(addprefix -I, $(SUBDIR))

ifeq ($(OS),Windows_NT)
	LIBS += -lm -lbsd
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LIBS += -lm -lbsd
	endif
	ifeq ($(UNAME_S),Darwin)
		LIBS += -lm
	endif
endif

PHONY := $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $(OUTDIR)/$@ $(OBJS) $(LIBS)

$(DIR_OBJ)/%.o: %.c $(INCS)
	mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) -c $< $(INC_DIRS)

PHONY += clean
clean:
	rm -rf $(OUTDIR)/* $(DIR_OBJ)/*

.PHONY = $(PHONY)
