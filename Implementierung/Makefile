TARGET = program

CC = gcc
CFLAGS = -g -std=c17 -Wall

OUTDIR = ./build
SUBDIR = src src/Implementierung src/IOSystem src/Testsystem
DIR_OBJ = ./obj

INCS = $(wildcard *.h $(foreach path, $(SUBDIR), $(path)/*.h))
SRCS = $(wildcard *.c $(foreach path, $(SUBDIR), $(path)/*.c))
OBJS = $(addprefix $(DIR_OBJ)/, $(SRCS:c=o))
INC_DIRS = -I./ $(addprefix -I, $(SUBDIR))

PHONY := $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $(OUTDIR)/$@ $(OBJS)

$(DIR_OBJ)/%.o: %.c $(INCS)
	mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) -c $< $(INC_DIRS)

PHONY += clean
clean:
	rm -rf $(OUTDIR)/* $(DATADIR)/* $(DIR_OBJ)/*

.PHONY = $(PHONY)
