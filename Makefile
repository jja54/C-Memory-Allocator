
mydriver: mydriver.c mymalloc.c mymalloc.h
	gcc -Wall -Werror -m32 -g -o mydriver mydriver.c mymalloc.c

bigdriver: bigdriver.c mymalloc.c mymalloc.h
	gcc -Wall -Werror -m32 -g -o bigdriver bigdriver.c mymalloc.c

clean:
	rm -f mydriver bigdriver