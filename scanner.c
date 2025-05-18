#include <stdio.h>
#include <stdlib.h>

FILE *arg_read(int, char **);

int error_code = 0;


int main(int argc, char **argv){
	int counter = 0; // this will be used to count the characters in the file
	char next;

	FILE *file = arg_read(argc, argv);

	while ((next = fgetc(file)) != EOF){
		counter++;	
	}

	printf("There are %d characters\n", counter);

	return error_code;
}

FILE *arg_read(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr, "There should be exactly 1 arguement");
		exit(1);
	}

	FILE *file = fopen(argv[1], "r");

	if (file == NULL){
		fprintf(stderr, "File: '%s' could not be opened", argv[1]);
		exit(1);
	}

	return file;
}
