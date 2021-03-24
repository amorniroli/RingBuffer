SRC_DIR = ./example/
OBJ_DIR = ./obj/
INC_DIR = -I./inc/
INC_DIR += -I./example

TARGET  = main
DEVELOP =

CC   = gcc
SIZE = size
NM   = nm

C_SOURCES  = $(wildcard $(SRC_DIR)*.c)

OBJECTS = $(patsubst $(SRC_DIR)%.c, $(OBJ_DIR)%.o, $(C_SOURCES))

OPT         =
INCS        = $(INC_DIR)
DEBUG       = -g $(DEVELOP)
CSTANDARD   = -std=c99
MISC        = -fprofile-arcs -ftest-coverage
WARN        = -Wall

GLOBAL_FLAGS = -c $(DEBUG) $(INCS) -O$(OPT) $(WARN) $(MISC)

C_FLAGS = $(CSTANDARD) $(GLOBAL_FLAGS)

ALL_CFLAGS   = $(filter-out $(NOT_CFLAGS), $(C_FLAGS))

# RULES

all: 
	mkdir $(OBJ_DIR)
	make build

build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -coverage --output $@ $(LDFLAGS) $(OBJECTS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) -o $@ $(ALL_CFLAGS) $<

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -R -f $(OBJ_DIR)
	
usage:
	@echo --------------------------------------------------------------------
	@echo USAGE:
	@echo  make clean .............. cleans all temporary files
	@echo  make all ................ builds the project and the related libs
	@echo --------------------------------------------------------------------

.PHONY:	all build bin clean