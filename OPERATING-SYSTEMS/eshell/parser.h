#ifndef PARSER_H
#define PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define INPUT_BUFFER_SIZE 256
#define MAX_ARGS 20
#define MAX_INPUTS 10

typedef enum {
    INPUT_TYPE_NON, INPUT_TYPE_SUBSHELL, INPUT_TYPE_COMMAND, INPUT_TYPE_PIPELINE
} SINGLE_INPUT_TYPE;
typedef enum {
    SEPARATOR_NONE, SEPARATOR_PIPE, SEPARATOR_SEQ, SEPARATOR_PARA
} SEPARATOR;

typedef struct {
    char *args[MAX_ARGS];
} command;

typedef struct {
    command commands[MAX_INPUTS];
    int num_commands;
} pipeline;

typedef union {
    char subshell[INPUT_BUFFER_SIZE];
    command cmd;
    pipeline pline;
} single_input_union;

typedef struct {
    SINGLE_INPUT_TYPE type;
    single_input_union data;
} single_input;

typedef struct {
    single_input inputs[MAX_INPUTS];
    SEPARATOR separator;
    int num_inputs;
} parsed_input;
int parse_line(char *line, parsed_input *input);
void free_parsed_input(parsed_input *input);
void pretty_print(parsed_input *input);
#ifdef __cplusplus
}
#endif
#endif //PARSER_H


