all: clean ans 

ans: ans.c mems.h
	gcc -o ans ans.c -lm

clean:
	rm -rf ans