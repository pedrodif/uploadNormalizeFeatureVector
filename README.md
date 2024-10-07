## Compilação e Execução

Este programa foi desenvolvido em linguagem C e implementa três métodos de normalização de vetores de características: Tabela de Consulta, Newton-Raphson (Quake III) e SSE (Streaming SIMD Extensions). O programa lê dados de um arquivo CSV, normaliza os vetores utilizando cada um dos três métodos e mede o uso de recursos (tempo de usuário, tempo de sistema e memória) para cada método.

### Pré-requisitos

Para compilar e executar este código, é necessário:

- Um compilador C, como o GCC.
- A biblioteca `sys/resource.h` para monitoramento do uso de recursos e a `xmmintrin.h` para suporte a SSE.

### Compilação

Para compilar o código, utilize o seguinte comando no terminal:

```bash
gcc -o main_executable main.c
```
Onde:

- `main.c` é o nome do arquivo de código-fonte.

### Execução

Após a compilação, você pode executar o programa com o seguinte comando:

```bash
./main_executable
```

Os arquivos CSV de entrada já estão incluídos no projeto e foram nomeados de `arquivoa.csv` a `arquivoe.csv`. Sugerimos que leia um arquivo por vez em cada execução. Para isso, comente os demais arquivos no código, deixando apenas o arquivo que deseja processar.

### Padrão dos testes

O programa irá processar os dados do arquivo CSV selecionado, aplicando os três métodos de normalização em sequência, e apresentará o uso de recursos para cada método, incluindo:

- **Tempo de Usuário:** Tempo que o CPU dedicou exclusivamente ao processo.
- **Tempo de Sistema:** Tempo gasto em chamadas ao sistema.
- **Memória:** Tamanho máximo do conjunto residente em kilobytes.

### Personalização

**__Para processar outros arquivos CSV, edite a lista de arquivos na função `main`, comentando os arquivos que não deseja processar.__**

### Notas

- O código evita operações inválidas como a divisão por zero e o cálculo da raiz quadrada de valores não positivos.
- O método de normalização com SSE processa os dados em blocos de 4 floats, garantindo maior eficiência em vetores grandes.
- Certifique-se de que o sistema de execução suporte o conjunto de instruções SSE antes de utilizar esse método.
