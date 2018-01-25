CC ?= gcc 
AR ?= ar
CFLAGS ?= -Wall -O2

INC := -Iinclude
LIB := 
PRG := socket
OBJ := main.o

all : $(PRG)

$(PRG) : $(OBJ)
	rm -rf $(PRG)
	$(CC) $(CFLAGS) $? -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LIB)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LIB)

.PRONY: clean 

clean:
	@echo "Removing linked and compiled files..."
	rm -rf $(OBJ) $(PRG)
