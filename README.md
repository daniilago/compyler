# Compilador em C (Em Desenvolvimento)

Este projeto consiste na construção de um compilador criado do zero utilizando a linguagem C. O objetivo do projeto é ler um código-fonte, passar por todas as etapas clássicas de compilação e, eventualmente, gerar código executável ou intermediário.

Atualmente, o projeto está em suas fases iniciais e a **Análise Léxica (Scanner)** já está totalmente implementada.

## Analisador Léxico

Ele varre o arquivo fonte e categoriza os caracteres nos seguintes `Tokens`:

* **Palavras Reservadas:** `if`, `else`, `while`, `for`, `return`, `int`, `float`, `char`, `void`, `do`.
* **Identificadores:** Nomes de variáveis e funções.
* **Literais e Números:** Inteiros, floats, strings (`"..."`) e caracteres (`'...'`).
* **Operadores:** Lógicos (`==`, `!=`, `&&`, etc.) e Aritméticos (`+`, `-`, `*`, `/`, etc.).
* **Delimitadores e separadores:** `{`, `}`, `(`, `)`, `;`, etc.
* **Comentários:** Suporte para ignorar comentários de linha (`//`) e de bloco (`/* ... */`).

### Tratamento de Erros Léxicos
O Lexer é resiliente e não trava diante de código malformado. Ele gera um `TOKEN_ERROR` com mensagens descritivas para:
- Comentários de bloco que chegam ao fim do arquivo sem fechamento.
- Strings ou caracteres literais com quebra de linha indevida ou sem fechamento.
- Formatações numéricas inválidas (ex: números colados em letras).

## Estrutura do Projeto

* `main.c`: Ponto de entrada. 
* `lexer.c` / `lexer.h`: Motor principal da análise léxica.
* `reader.h` / `reader.c`: Utilitários para leitura e carregamento do arquivo de código-fonte para a memória.

## Como Compilar e Testar

Para compilar o estado atual do projeto (testando o Lexer), você precisará de um compilador C (como o `gcc`):

1. **Compile os arquivos:**
   ```bash
   gcc main.c lexer.c reader.c -o compilador