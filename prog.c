#include <stdio.h>
#include <unistd.h>  // isatty(), fileno()
#include <stdbool.h>

int main(int argc, char *argv[]) {
    char buffer[256];       // Buffer para leitura de linhas de stdin
    bool leu_algo = false;  // Flag que indica se já iniciou a leitura

    // 1) Impressão de argumentos de linha de comando
    if (argc > 1) {
        printf("Voce chamou prog com %d argumento(s):\n", argc - 1);
        for (int i = 1; i < argc; i++) {
            printf("  arg%d: %s\n", i, argv[i]);
        }
    }

    // 2) Processamento de stdin apenas se redirecionado
    //    isatty(fileno(stdin)) retorna false quando stdin NÃO é terminal
    if (!isatty(fileno(stdin))) {
        // Loop de leitura de cada linha até EOF
        while (fgets(buffer, sizeof(buffer), stdin)) {
            if (!leu_algo) {
                printf("\n--- Lendo da entrada padrao ---\n");
                leu_algo = true;
            }
            // Imprime a linha recebida (já contém '\n' ao final)
            printf("Recebido: %s", buffer);
        }
        // Se leu algo, sinaliza fim da entrada
        if (leu_algo) {
            printf("\n--- Fim da entrada ---\n");
        }
    }

    return 0;
}