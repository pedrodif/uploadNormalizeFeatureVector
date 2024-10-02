#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_VALUE 100.0 //valor máximo que x pode armazenar para a tabela
#define LOOKUP_TABLE_SIZE 100 
#define MIN_VALUE 0.1 //valor mínimo que x pode armazenar para a tabela

// Lookup table para armazenar valores de 1/sqrt(x)
float lookup_table[LOOKUP_TABLE_SIZE];

// Função para inicializar a lookup table
void initialize_lookup_table() {
    float step = (MAX_VALUE - MIN_VALUE) / (LOOKUP_TABLE_SIZE - 1);
    for (int i = 0; i < LOOKUP_TABLE_SIZE; i++) {
        float x = MIN_VALUE + i * step;
        lookup_table[i] = 1.0f / sqrt(x);
    }
}

// Função para normalizar um vetor de características usando a lookup table
void normalize_feature_vector(float* features, int length) {
    float sum = 0.0f;
    for (int i = 0; i < length; i++) {
        sum += features[i] * features[i];
    }

    // Mapear o valor de `sum` para o índice apropriado na lookup table
    float step = (MAX_VALUE - MIN_VALUE) / (LOOKUP_TABLE_SIZE - 1);
    int index = (int)((sum - MIN_VALUE) / step);
    if (index < 0) index = 0;
    if (index >= LOOKUP_TABLE_SIZE) index = LOOKUP_TABLE_SIZE - 1;

    float inv_sqrt = lookup_table[index];

    for (int i = 0; i < length; i++) {
        features[i] *= inv_sqrt;
    }
}

// Função para ler um arquivo CSV e retornar um array de floats
float* read_csv_file(const char* filename, int* length) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        return NULL;
    }

    int capacity = 100; // capacidade inicial
    float* data = (float*)malloc(capacity * sizeof(float));
    *length = 0;

    while (fscanf(file, "%f,", &data[*length]) != EOF) {
        (*length)++;
        if (*length >= capacity) {
            capacity *= 2;
            data = (float*)realloc(data, capacity * sizeof(float));
        }
    }

    fclose(file);
    return data;
}

int main() {
    // Inicialize a lookup table
    initialize_lookup_table();

    // Lista de arquivos
    const char* files[] = {"arquivoa.csv", "arquivob.csv", "arquivoc.csv", "arquivod.csv", "arquivoe.csv"};
    int num_files = sizeof(files) / sizeof(files[0]);

    for (int i = 0; i < num_files; i++) {
        // Ler o arquivo
        int length;
        float* features = read_csv_file(files[i], &length);
        
        if (!features) {
            printf("Erro ao ler o arquivo %s\n", files[i]);
            continue;
        }

        // Normalizar o vetor de características
        normalize_feature_vector(features, length);

        // Imprimir o vetor normalizado
        printf("Normalized features for file %s:\n", files[i]);
        for (int j = 0; j < length; j++) {
            printf("%f ", features[j]);
        }
        printf("\n");

        free(features);
    }

    return 0;
}
