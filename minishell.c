/*
 * minishell.c
 *
 * Um shell simplificado para a disciplina CSC-33:
 *   - Prompt interativo
 *   - Execução de programas externos via fork + execv
 *   - Redirecionamento de entrada (<) e saída (>) usando dup2
 *   - Comando interno: exit
 *
 * Exemplo de uso:
 *   cmd> /bin/ls -l > lista.txt
 *   cmd> /bin/cat < lista.txt
 *   cmd> exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // fork, execv, dup2
#include <sys/wait.h>   // waitpid
#include <fcntl.h>      // open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <sys/types.h>  // pid_t

#define MAX_INPUT 1024
#define MAX_ARGS 100

int main() {
    char input[MAX_INPUT];         // Armazena a linha de comando digitada (buffer)
    char *args[MAX_ARGS];          // Vetor de ponteiros para os argumentos
    char *input_file = NULL;       // Ponteiro para o nome do arquivo para redirecionamento de entrada
    char *output_file = NULL;      // Ponteiro para o nome do arquivo para redirecionamento de saída

    printf("Welcome to the miniature-shell.\n");

    while (1) {
        /************ 1. Exibe o prompt e lê a linha de comando do usuário *******************************************************/
        printf("cmd> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\nExiting mini-shell.\n");
            break;  // Encerra ao receber EOF (Ctrl+D)
        }


        /************ 2. Remove o caractere de nova linha, se houver *************************************************************/
        input[strcspn(input, "\n")] = 0;


        /************ 3. Se o usuário digitar "exit", sai da shell ***************************************************************/
        if (strcmp(input, "exit") == 0) {
            break;
        }


        /************ 4. Variáveis auxiliares para inicializar parsing ***********************************************************/
        int arg_index = 0;
        input_file = NULL;
        output_file = NULL;


        /************ 5. Tokeniza a linha de comando, args e redirecionamentos utilizando os espaços como delimitadores **********/
        char *token = strtok(input, " ");
        while (token != NULL && arg_index < MAX_ARGS - 1) {
            if (strcmp(token, "<") == 0) {
                // 5.1 Nesse caso, o próximo token é o nome do arquivo para redirecionamento de entrada
                token = strtok(NULL, " ");
                if (token) input_file = token;
            } else if (strcmp(token, ">") == 0) {
                // 5.2 Nesse caso, o próximo token é o nome do arquivo para redirecionamento de saída
                token = strtok(NULL, " ");
                if (token) output_file = token;
            } else {
                // 5.3 Argumento normal (sem redirecionamento)
                args[arg_index++] = token;
            }
            token = strtok(NULL, " ");
        }

        args[arg_index] = NULL;  // Termina o vetor de argumentos com NULL

        if (args[0] == NULL) {
            // Se nenhum argumento foi digitado, volta ao prompt
            continue;
        }


        /************ 6. Criação do processo filho com fork *************************************************************************/
        pid_t pid = fork();  
        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            // Código para o processo filho

            /************ 7. Redireciona entrada padrão (stdin) do processo filho, se necessário ************************************/
            if (input_file) {
                int fd_in = open(input_file, O_RDONLY);  // Usando open para abrir o arquivo de entrada

                // Se o arquivo de entrada não puder ser aberto, exibe erro e encerra
                if (fd_in < 0) {
                    perror("failed to open input file");
                    exit(1);
                }

                // Usando dup2 para redirecionar a entrada padrão
                // Se dup2 falhar, exibe erro e encerra
                if (dup2(fd_in, STDIN_FILENO) < 0) {   
                    perror("dup2 input_file");
                    close(fd_in);
                    exit(EXIT_FAILURE);
                }
                close(fd_in);
            }

            /************ 8. Redireciona saída padrão(stdout) do processo filho, se necessário **************************************/
            if (output_file) {
                int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Usando open para abrir o arquivo de saída

                // Se o arquivo de saída não puder ser aberto, exibe erro e encerra
                if (fd_out < 0) {
                    perror("failed to open output file");
                    exit(1);
                }

                // Usando dup2 para redirecionar a saída padrão
                // Se dup2 falhar, exibe erro e encerra
                if (dup2(fd_out, STDOUT_FILENO) < 0) {
                    perror("dup2 output_file");
                    close(fd_out);
                    exit(EXIT_FAILURE);
                }
                close(fd_out);
            }

            /************ 9. Executa o comando com execv -> chama execve internamente ************************************************/
            execv(args[0], args);  // execv

            // Se execv falhar, exibe erro e encerra
            perror("execv failed");
            exit(EXIT_FAILURE);
            
        } else {
            /************ 10. Processo pai: Espera o filho terminar ******************************************************************/
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid failed");
            }

            // Se o filho terminou com erro, exibe erro
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                perror("child process failed");
            }
        }
    }

    return 0;
}
