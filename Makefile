CC = gcc
CFLAGS = -Wall -g

LIB = 

SRC = driver.c data.c quadtree.c array.c linkedlist.c

OBJ = $(SRC:.c=.o)
 
EXE = dict

$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)

driver.o: driver.c data.h quadtree.h array.h linkedlist.h

data.o: data.c data.h

quadtree.o: quadtree.c quadtree.h data.h array.h linkedlist.h

array.o: array.c array.h data.h

linkedlist.o: linkedlist.c linkedlist.h

clean:
	rm -f $(OBJ) $(EXE)
