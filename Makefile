FILE=shell.c

defualt: run

check: myshell
	./myshell 

gdb: myshell 
	gdb --args myshell

myshell: ${FILE}
	gcc -g -O0 -o myshell ${FILE}

vi: ${FILE}
	vi ${FILE}

clean:
	rm -f myshell 
