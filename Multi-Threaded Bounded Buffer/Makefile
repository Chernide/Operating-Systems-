HEADERS = multi_lookup.h util.h
CFLAGS = -Wall -Wextra

default: multi_lookup

multi_lookup.o: multi_lookup.c $(HEADERS) util.c
	gcc $(CFLAGS) -c multi_lookup.c -o multi_lookup.o
multi_lookup: multi_lookup.o
	gcc -pthread multi_lookup.o -o multi_lookup

clean:
	-rm -f multi-lookup
	-rm -f multi_lookup.o
	-rm -f results.txt
	-rm -f serviced.txt
