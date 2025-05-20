// Program: scanner.c
// Author: Elliott Cepin
// Purpose: Creates a .tk file from a given .bm file (.bm being B-minor)
//          the .tk holds tokens in order seperated by a newline
//          this scanner/lexer does not classify syntax errors

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum tk{
	IDENTIFIER,
	NUMBER,
	COMMENT,
	STRING,
	NONE
} TokenType;


FILE *arg_read(int, char **);
FILE *file_write(char *);
char *type_string(TokenType);
int valid_identifier(char, int);
int valid_number(char);
int valid_comment(char, int, FILE *);
int valid_string(char, int, FILE *, FILE *);

int error_code = 0;

int main(int argc, char **argv){
	int comment_type = 0; // C-style comments are 2 and C++ style commnets are 1. i.e. single line // == 2 multiline /* == 1; no comment is 0

	char next;
	TokenType token_type = NONE;
	FILE *in = arg_read(argc, argv); 
	
	// arg_read does error checking for 
	char *out_file = malloc(strlen(argv[1]) + 4);
	strcat(out_file, argv[1]);
	strcat(out_file, ".tk");
	FILE *out  = file_write(out_file);
	
	int last_was_escape = 0;
	while ((next = fgetc(in)) != EOF){
		last_was_escape = (next == '\\') || last_was_escape;
		
		if (token_type == NONE){
			if (valid_identifier(next, 1)){
				token_type = IDENTIFIER;
				fprintf(out, "%s:%c", type_string(token_type), next); // no newline; this is only part of the identifier
				continue;
			}

			if (valid_number(next)){
				token_type = NUMBER;
				fprintf(out, "%s:%c", type_string(token_type), next);
				continue;
			}

			if (valid_comment(next, comment_type, in)){
				// it looks like we'll have to have a running "comment_type"
				// valid_comment will peak and ungetc no matter what
				token_type = COMMENT;
				fprintf(out, "%s:%c", type_string(token_type), next);
				char comment_chr2 = fgetc(in);
				comment_type = (comment_chr2 == '/') + 1; // this will be used later
				fprintf(out, "%c", comment_chr2);
				continue;
			}
			
			if (valid_string(next, 1, in, out) && !last_was_escape){
				token_type = STRING;
				fprintf(out, "%s:%c", type_string(token_type), next);
				continue;
			}
			// TODO: expand out all cases
			continue;
		}

		last_was_escape = ('\\' == next);
		switch (token_type){
			// TODO: replace token_types with enums
			case IDENTIFIER:
				
				if (valid_identifier(next, 0)){
					fprintf(out, "%c", next);
					break;
				}

			case NUMBER:
				if (valid_number(next)){
					fprintf(out, "%c", next);
					break;
				}
			case COMMENT:
				if (valid_comment(next, comment_type, in)){
					if (next == '\n' && comment_type == 1) {
						break; // this prevents unwanted newlines in our file
					}
					fprintf(out, "%c", next);
					break;
				}
			case STRING:
				if (valid_string(next, 0, in, out) && token_type == STRING){
					if (next == '\\'){
						break; // we dealt with this already
					}
					fprintf(out, "%c", next);
					break;
				}	
				// I almost just decided that newline meant end-of-string
				if ((next == '\"') && token_type == STRING){
					fprintf(out, "\"");
					next = fgetc(in);
				}


			// TODO: expand out all cases
			default:
				token_type = NONE;
				fprintf(out, "\n");
				ungetc(next, in);
				comment_type = 0;

		}
	}
	fclose(out);
	fclose(in);
	return error_code;
}

// turns enum into a string for easy printing... 
// TODO: add all enums
char *type_string(TokenType tk){
	switch (tk){
		case IDENTIFIER:
			return "IDENTIFIER";
		case NUMBER:
			return "NUMBER";

		case COMMENT:
			return "COMMENT";
		case STRING:
			return "STRING";
		default:
			return ""; // again, this shouldn't really show up
	}
}

// doesn't do anything about errors
int valid_string(char c, int first_char, FILE *in, FILE *out){
	if (first_char){
		return c == '\"';
	}

	if (c == '\\'){
		fprintf(out, "\\");
		char next = fgetc(in);
		if (next == '\"'){
			fprintf(out, "\"");
			return 1;
		}
		ungetc(next, in);
		return 1;
	}
	
	return (c != '\"' && c != '\n');
}
/*
 * this is just here so that I can actually test that this works on comments
 * this type of comment gets treated differently, because the switch case ungets on false return value
 * so it has to do a get as a countermeasure, 
 * since we want to be at the end of the comment so we don't reprocess the / in comment end
 * also, these get flattened into one line in the .tk file
 */
int valid_comment(char c, int comment_type, FILE *in){
	char next;

	if (!comment_type){
		next = fgetc(in);
		ungetc(next, in);
		return c == '/' && (next == '/' || next == '*');
	}
	
	if (comment_type == 1){
		next = fgetc(in);
		if (c == '*' && next == '/'){
			fgetc(in); // this is to circumvent my lazy switch case code
			return 0;
		}
		ungetc(next, in);
		return 1;
	}
	
	if (comment_type == 2){
		return c != '\n';
	}


	return 0;
}

int valid_number(char c){
	return isdigit(c);
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
