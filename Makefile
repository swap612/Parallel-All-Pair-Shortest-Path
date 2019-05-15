CC      = mpicc
COPT    = -g -O3

LD      = $(CC)
LDFLAGS = $(COPT) 

all: Blocked_Parallel.x naiveAPSP.x naiveCheckerboard.x

%.x: %.o Blocked_Parallel.o naiveAPSP.o naiveCheckerboard.o
	$(LD) $(LDFLAGS) $< -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o *.x
