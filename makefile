all: capos.c
	gcc -Wall -o capos capos.c

clean:
	$(RM) capos
