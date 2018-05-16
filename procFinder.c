#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "argvTable.h"

void parse_argv(int argc, char *argv[], struct ArgvTable *argvTable);
void ps(struct ArgvTable *argvTable);

int main(int argc, char *argv[]) {

	struct ArgvTable argvTable = {0,0,0,0,0,0};
	parse_argv(argc, argv, &argvTable);
	ps(&argvTable);

	return 0;
}


void parse_argv(int argc, char *argv[], struct ArgvTable *argvTable) {

	int ch;
	while((ch = getopt(argc, argv, "axpqrs")) != -1) {
		switch(ch) {
			case 'a':
				argvTable->a = 1;
				break;
			case 'x':
				argvTable->x = 1;
				break;
			case 'p':
				if(argvTable->q == 0 && argvTable->r == 0 && argvTable->s == 0)
					argvTable->p = 1;
				break;
			case 'q':
				if(argvTable->p == 0 && argvTable->r == 0 && argvTable->s == 0)
					argvTable->q = 1;
				break;
			case 'r':
				if(argvTable->p == 0 && argvTable->q == 0 && argvTable->s == 0)
					argvTable->r = 1;
				break;
			case 's':
				if(argvTable->p == 0 && argvTable->q == 0 && argvTable->r == 0)
					argvTable->s = 1;
				break;
			default:
				break;
		}
	}

	return;
}


