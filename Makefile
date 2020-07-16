# Makefile of #1 exercise - Semester A (winter) 2019


CC = gcc
CFLAGS = -Wall  -lpthread
LINKFLAGS = -pthread -lz

OBJECT1 = CoinUtils.o
OBJECT2 = MtaCoin.o
OBJECT3 = DataStructures.o


SOURCE1 = MtaCoin.c
SOURCE2 = CoinUtils.c
SOURCE3 = DataStructures.c





EXEFILE = MtaCoin

test : all
	./MtaCoin

all : MtaCoin

$(OBJECT3): $(SOURCE3)
	$(CC) $(CFLAGS) -c $(SOURCE3)

$(OBJECT1) : $(OBJECT3) $(SOURCE2)
	$(CC) -c $(CFLAGS)  $(SOURCE2)

$(EXEFILE) :  $(OBJECT1) $(SOURCE1)
	 $(CC) $(CFLAGS) -o $(EXEFILE) $(SOURCE1) $(OBJECT1) $(OBJECT3)  $(LINKFLAGS)

clean:
	rm  -vf *.o
