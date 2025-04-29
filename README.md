# Documentação do Projeto Mini-shell

### Alunos: Jontan de Lima Santos, Rui Andrade de Carvalho Nunes
### Turma: COMP26

## 1. Visão Geral

O objetivo deste projeto é construir uma **mini-shell** em C que permita:

- **Prompt interativo** para comandos do usuário.
- **Execução de programas externos** usando chamadas de sistema (`fork`, `execv`).
- **Redirecionamento de entrada** (`<`) e **saída** (`>`), manipulando `stdin` e `stdout` via `dup2`.
- **Comando interno** simples: `exit` para encerrar o shell.

Além disso, inclui um programa auxiliar (`prog.c`) para fins de teste, demonstrando leitura de argumentos e detecção de redirecionamento de entrada.

---

## 2. Makefile

O **Makefile** simplifica a compilação do código.

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: minishell

minishell: minishell.c
	$(CC) $(CFLAGS) minishell.c -o minishell

clean:
	rm -f minishell
```

- **CC**: compilador (`gcc`).
- **CFLAGS**:
  - `-Wall`: mostra todos os avisos.
  - `-Wextra`: habilita avisos adicionais.
  - `-std=c11`: usa padrão C11.

### Alvos (targets)

- `make` ou `make all`: compila `minishell.c` e gera o executável `minishell`.
- `make clean`: remove o binário `minishell`.


---

## 3. Arquivo `prog.c`

Programa simples para testar comportamento de redirecionamento e passagem de argumentos.

### Principais funcionalidades

1. **Impressão de argumentos**
   - Se `argc > 1`, exibe cada argumento recebido.
2. **Leitura de stdin redirecionado**
   - Usa `isatty(fileno(stdin))` para verificar se a entrada padrão não é o terminal.
   - Em caso de pipe ou `< arquivo`, lê linhas com `fgets` e imprime cada uma.

```c
// Fluxo resumido em prog.c
if (argc > 1) print_args();
if (!isatty(stdin)) read_and_print_stdin();
```

### Como compilar e rodar

```bash
gcc prog.c -o prog             # Compila da forma padrão
chmod +x prog                  # Torna executável oferecendo permissões adequadas

# Testes:
./prog a b c                   # Testa argumentos
echo -e "linha1\nlinha2" | ./prog   # Testa pipe
./prog < arquivo.txt           # Testa redirecionamento de arquivo
```

---

## 4. Arquivo `minishell.c`

Implementação principal do mini-shell, dividida em etapas:

1. **Leitura do comando**
   - Usa `fgets` para capturar até 1023 caracteres.
   - Remove o caractere de nova linha.
2. **Comando interno**
   - Se o usuário digitar `exit`, o shell encerra.
3. **Tokenização**
   - Separa a entrada em tokens por espaços com `strtok`.
   - Detecta `<` e `>` para redirecionar arquivos.
   - Armazena apenas os argumentos reais em `args[]`.
4. **Execução**
   - `fork` para criar processo filho.
   - No **filho**:
     - Redireciona `stdin`/`stdout` se necessário (`open` + `dup2`).
     - Chama `execv` com o vetor de argumentos.
   - No **pai**:
     - Usa `waitpid` para aguardar o término do filho.

### Estrutura resumida

```c
while (fgets(line)) {
    if (strcmp(line, "exit") == 0) break;

    // Parsing: args, input_file, output_file

    pid = fork();
    if (pid == 0) {
        redirect_if_needed();
        execv(args[0], args);
    } else {
        waitpid(pid);
    }
}
```

### Pontos de atenção

- **Caminhos absolutos**: o usuário deve fornecer o caminho completo do comando executável (e.g., `/bin/ls`).
- **Tamanho limitado**: aceita até 1023 caracteres por comando.
- **Erros de exec**: em caso de falha, `perror` exibe a mensagem adequada.

---

## 5. Exemplos de Uso

| Comando                         | Descrição                                      |
|---------------------------------|------------------------------------------------|
| `/bin/ls`                       | Lista o diretório atual                        |
| `/bin/echo Ola > hello.txt`     | Redireciona saída para `hello.txt`             |
| `/bin/cat < hello.txt`          | Lê `hello.txt` via stdin                       |
| `./prog arg1 arg2 < in.txt`     | Testa `prog.c` com args e redirecionamento     |
| `exit`                          | Sai do mini-shell                              |

**Fluxo completo**:
```bash
$ make
$ ./minishell
Welcome to the miniature-shell.
cmd> /bin/ls
...
cmd> /bin/echo Teste > saida.txt
cmd> /bin/cat < saida.txt
Teste
cmd> exit
$
```

---

