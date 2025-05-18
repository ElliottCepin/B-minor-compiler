#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum tk{
	IDENTIFIER,
	NONE
} TokenType;


FILE *arg_read(int, char **);
FILE *file_write(char *);
char *type_string(TokenType);
int valid_identifier(char, int);

int error_code = 0;

int main(int argc, char **argv){
	char next;
	TokenType token_type = NONE;
	char *out_file = malloc(strlen(argv[1]) + 4);
	strcat(out_file, *argv);
	strcat(out_file, ".tk");

	FILE *file = arg_read(argc, argv);
	FILE *out  = file_write(out_file);

	while ((next = fgetc(file)) != EOF){
		if (token_type == NONE){
			if (valid_identifier(next, 1)){
				token_type = IDENTIFIER;
				fprintf(out, "%s:%c", type_string(token_type), next); // no newline; this is only part of the identifier
				continue;
			}
			// TODO: expand out all cases
			continue;
		}
		switch (token_type){
			// TODO: replace token_types with enums
			case IDENTIFIER:
				
				if (valid_identifier(next, 0)){
					fprintf(out, "%c", next);
				} else {
					token_type = NONE;
					fprintf(out, "\n");
					ungetc(next, file);
				}
				break;
			// TODO: expand out all cases
			default:
				token_type = NONE;
				ungetc(next, file);
				// this probably shouldn't happen once the code is fully done

		}
	}
	fclose(out);
	fclose(file);
	return error_code;
}


// turns enum into a string for easy printing... 
// TODO: add all enums
char *type_string(TokenType tk){
	switch (tk){
		case IDENTIFIER:
			return "IDENTIFIER";
		default:
			return ""; // again, this shouldn't really show up
	}
}

// returns true if the given character could be part of an identifier
// the first_char of an identifier cannot be a number 
int valid_identifier(char c, int first_char){
	if (isalpha(c) || c == '_'){
		return 1;
	}

	if (!first_char && isdigit(c)){
		return 1;
	}

	return 0;
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

FILE *file_write(char *name){
	FILE *file = fopen(name, "w");
	if (file == NULL){
		fprintf(stderr, "could not open file '%s'", name);
		exit(1);
	}

	return file;
}
