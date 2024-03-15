PROG ?= rockrobo_control

# all: $(PROG)
# 	$(DEBUGGER) ./$(PROG) $(ARGS)

CROSS = 
CC     = $(CROSS)arm-linux-gnueabihf-gcc

$(PROG): main.c
	$(CC) ./mongoose.c -I../.. -W -Wall -DMG_ENABLE_LINES=1 -DMG_ENABLE_DIRECTORY_LISTING=1 $(CFLAGS) -o $(PROG) main.c

clean:
	rm -rf $(PROG) *.o *.dSYM *.gcov *.gcno *.gcda *.obj *.exe *.ilk *.pdb
