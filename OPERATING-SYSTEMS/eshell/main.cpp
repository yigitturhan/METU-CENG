extern "C" {
#include "parser.h"
}
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

void handle_parsed_input(parsed_input pinput);

void execute_command(char* args[]) {
    execvp(args[0], args);
    exit(EXIT_SUCCESS);
}

void handle_single_input(single_input input){
    if(input.type == INPUT_TYPE_COMMAND){
        pid_t pid = fork();
        if(pid == 0){
            execute_command(input.data.cmd.args);
            exit(EXIT_SUCCESS);
        }
        else waitpid(pid,NULL,0);
    }
    else if(input.type == INPUT_TYPE_PIPELINE){
        pipeline pl = input.data.pline;
        int i, num_pipes = pl.num_commands - 1, j;
        int pipefds[2 * num_pipes];
        for (i = 0; i < num_pipes; i++) {
            if (pipe(pipefds + i * 2) < 0) {
                perror("Couldn't Pipe");
                exit(EXIT_FAILURE);
            }
        }
        for (j = 0; j < pl.num_commands; j++) {
            pid_t pid = fork();
            if (pid == 0) {
                if (j != 0) {
                    if (dup2(pipefds[(j - 1) * 2], STDIN_FILENO) < 0) {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                }
                if (j != num_pipes) {
                    if (dup2(pipefds[j * 2 + 1], STDOUT_FILENO) < 0) {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                }
                for (i = 0; i < 2 * num_pipes; i++) close(pipefds[i]);
                execute_command(pl.commands[j].args);
                exit(EXIT_SUCCESS);
            }
            else if (pid < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
        }
        for (i = 0; i < 2 * num_pipes; i++) close(pipefds[i]);
        for (i = 0; i < pl.num_commands; i++) wait(NULL);
    }
    else if(input.type == INPUT_TYPE_SUBSHELL){
        pid_t pid = fork();
        if (pid == 0) {
            parsed_input subshell_input;
            if (parse_line(input.data.subshell, &subshell_input)) {
                if(subshell_input.separator != SEPARATOR_PARA){
                    handle_parsed_input(subshell_input);
                    free_parsed_input(&subshell_input);
                }
                else{
                    int pipefds[2 * subshell_input.num_inputs];
                    for(int i = 0; i < subshell_input.num_inputs; i++) pipe(pipefds + i * 2);
                    for(int i = 0; i < subshell_input.num_inputs; i++){
                        if(fork() == 0){
                            dup2(pipefds[2*i], STDIN_FILENO);
                            for(int j = 0; j < subshell_input.num_inputs*2; j++) close(pipefds[j]);
                            handle_single_input(subshell_input.inputs[i]);
                            exit(EXIT_SUCCESS);
                        }
                    }
                    if(fork() == 0){
                        char buffer[INPUT_BUFFER_SIZE];
                        ssize_t bytes = read(STDIN_FILENO, buffer, INPUT_BUFFER_SIZE);
                        while (bytes > 0){
                            bool cond = true;
                            for(int i = 0; i < subshell_input.num_inputs; i++){
                                if(write(pipefds[2*i + 1],buffer,bytes)) cond = false;
                            }
                            if(cond) break;
                            bytes = read(STDIN_FILENO, buffer, INPUT_BUFFER_SIZE);
                        }
                        for(int i = 0; i < subshell_input.num_inputs*2; i++) close(pipefds[i]);
                        exit(EXIT_SUCCESS);
                    }
                    else{
                        for(int i = 0; i < subshell_input.num_inputs*2; i++) close(pipefds[i]);
                        for(int i = 0; i < subshell_input.num_inputs+1;i++) wait(NULL);
                    }
                }
            }
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0) waitpid(pid, NULL, 0);
    }
    else exit(EXIT_SUCCESS);
}

void handle_parsed_input(parsed_input pinput){
    if(pinput.separator == SEPARATOR_PIPE){
        int i, num_pipes = pinput.num_inputs - 1, j;
        int pipefds[2 * num_pipes];
        for (i = 0; i < num_pipes; i++) {
            if (pipe(pipefds + i * 2) < 0) {
                perror("Couldn't Pipe");
                exit(EXIT_FAILURE);
            }
        }
        for (j = 0; j < pinput.num_inputs; j++) {
            if (fork() == 0) {
                if (j != 0) {
                    if (dup2(pipefds[(j - 1) * 2], STDIN_FILENO) < 0) {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                }
                if (j != num_pipes) {
                    if (dup2(pipefds[j * 2 + 1], STDOUT_FILENO) < 0) {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                }
                for (i = 0; i < 2 * num_pipes; i++) close(pipefds[i]);
                handle_single_input(pinput.inputs[j]);
                exit(EXIT_SUCCESS);
            }
        }
        for (i = 0; i < 2 * num_pipes; i++) close(pipefds[i]);
        for (i = 0; i < pinput.num_inputs; i++) wait(NULL);
    }
    else if(pinput.separator == SEPARATOR_NONE){
        pid_t pid = fork();
        if(pid == 0){
            for(int i = 0; i < pinput.num_inputs; i++) handle_single_input(pinput.inputs[i]);
            exit(EXIT_SUCCESS);
        }
        else waitpid(pid,NULL,0);
    }
    else if(pinput.separator == SEPARATOR_SEQ){
        for(int i = 0; i < pinput.num_inputs; i++){
            pid_t pid = fork();
            if(pid == 0){
                handle_single_input(pinput.inputs[i]);
                exit(EXIT_SUCCESS);
            }
            else waitpid(pid,NULL,0);
        }
    }
    else if(pinput.separator == SEPARATOR_PARA){
        for (int i = 0; i < pinput.num_inputs; i++) {
            if (fork() == 0){
                handle_single_input(pinput.inputs[i]);
                exit(EXIT_SUCCESS);
            }
        }
        for (int i = 0; i < pinput.num_inputs; i++) wait(NULL);
    }
    else exit(EXIT_SUCCESS);
}

int main(){
    while(true){
        parsed_input pinput;
        string line;
        cout << "/> ";
        getline(cin, line);
        if(line == "quit") break;
        char c_line[INPUT_BUFFER_SIZE];
        strncpy(c_line, line.c_str(), INPUT_BUFFER_SIZE);
        c_line[INPUT_BUFFER_SIZE - 1] = '\0';
        if (!parse_line(c_line, &pinput)) cerr << "Error parsing input.\n";
        handle_parsed_input(pinput);
        free_parsed_input(&pinput);
    }
}