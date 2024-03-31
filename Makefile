all: sixth

sixth: sixth.c
	gcc -g -Wall -Werror -fsanitize=address,undefined -g sixth.c -o sixth

clean: 
	rm -rf sixth