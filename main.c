#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <xmmintrin.h> // Para SSE
#define _GNU_SOURCE

#define TABLE_SIZE 1024
#define MAX_VALUE 1000.0f

float lookup_table[TABLE_SIZE];

// Inicializa a tabela de consulta
void init_lookup_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        float x = (float)i / (float)(TABLE_SIZE - 1) * MAX_VALUE;
        lookup_table[i] = (x == 0) ? 0 : 1.0f / sqrt(x); // Evita divisão por zero
    }
}

// Função de normalização usando Lookup Table
void normalize_feature_vector_lookup(float* features, int length) {
    for (int i = 0; i < length; i++) {
        int index = (int)(features[i] * (TABLE_SIZE - 1) / MAX_VALUE);
        if (index >= 0 && index < TABLE_SIZE) {
            features[i] *= lookup_table[index];
        }
    }
}

// Função de normalização usando Newton-Raphson (Quake III)
float fast_inverse_sqrt(float x) {
    if (x <= 0) return 0; // Evita raiz quadrada de número não positivo

    long i;
    float x2, y;
    x2 = x * 0.5f;
    y = x; // Estimativa inicial

    i = *(long *)&y; // Manipulação de bits
    i = 0x5f3759df - (i >> 1); // Aproximação
    y = *(float *)&i;

    // Uma iteração de Newton-Raphson
    y = y * (1.5f - (x2 * y * y));

    return y;
}

void normalize_feature_vector_quake(float* features, int length) {
    for (int i = 0; i < length; i++) {
        float inv_sqrt = fast_inverse_sqrt(features[i]);
        features[i] *= inv_sqrt;
    }
}

// Função de normalização usando SSE
void normalize_feature_vector_sse(float* features, int length) {
    for (int i = 0; i < length; i += 4) {
        __m128 data = _mm_loadu_ps(&features[i]);
        __m128 inv_sqrt = _mm_rsqrt_ps(data);
        _mm_storeu_ps(&features[i], _mm_mul_ps(data, inv_sqrt));
    }
}

float** read_csv(const char* filename, int* num_elements, int* num_dimensions) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Falha ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    *num_elements = 0;
    *num_dimensions = 0;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (*num_elements == 0) {
            char* token = strtok(line, ",");
            while (token) {
                (*num_dimensions)++;
                token = strtok(NULL, ",");
            }
        }
        (*num_elements)++;
    }
    rewind(file);

    // Alocação correta de memória para features
    float** features = (float**)malloc(*num_elements * sizeof(float*));
    if (!features) {
        perror("Falha na alocação de memória para features");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < *num_elements; i++) {
        features[i] = (float*)malloc(*num_dimensions * sizeof(float));
        if (!features[i]) {
            perror("Falha na alocação de memória para features[i]");
            exit(EXIT_FAILURE);
        }
    }

    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        int j = 0;
        char* token = strtok(line, ",");
        while (token) {
            features[i][j++] = atof(token);
            token = strtok(NULL, ",");
        }
        i++;
    }

    fclose(file);
    return features;
}

void get_resource_usage(struct rusage* usage) {
    getrusage(RUSAGE_SELF, usage);
}

void print_resource_usage(const char* label, struct rusage* usage) {
    printf("%s\n", label);
    printf("Tempo de usuário: %ld.%06ld segundos\n", usage->ru_utime.tv_sec, usage->ru_utime.tv_usec);
    printf("Tempo de sistema: %ld.%06ld segundos\n", usage->ru_stime.tv_sec, usage->ru_stime.tv_usec);
    printf("Tamanho máximo do conjunto residente: %ld kilobytes\n", usage->ru_maxrss);
}

int main() {
    // Nomes dos arquivos CSV
    const char* files[] = {
        "arquivoa.csv",
        //"arquivob.csv",
        //"arquivoc.csv",
        //"arquivod.csv",
        //"arquivoe.csv"
    };
    const int num_files = sizeof(files) / sizeof(files[0]);

    // Inicializa a tabela de consulta
    init_lookup_table();

    for (int f = 0; f < num_files; f++) {
        printf("Lendo arquivo: %s\n", files[f]); // Mensagem de depuração
        int num_elements, num_dimensions;
        float** features = read_csv(files[f], &num_elements, &num_dimensions);

        struct rusage start_usage, end_usage;

        // Normalização com Lookup Table
        get_resource_usage(&start_usage);
        for (int i = 0; i < num_elements; i++) {
            normalize_feature_vector_lookup(features[i], num_dimensions);
        }
        get_resource_usage(&end_usage);
        printf("Características normalizadas (Tabela de Consulta) - %s:\n", files[f]);
        print_resource_usage("Uso de Recursos na Tabela de Consulta (Início)", &start_usage);
        print_resource_usage("Uso de Recursos na Tabela de Consulta (Fim)", &end_usage);

        // Normalização com Quake III
        get_resource_usage(&start_usage);
        for (int i = 0; i < num_elements; i++) {
            normalize_feature_vector_quake(features[i], num_dimensions);
        }
        get_resource_usage(&end_usage);
        printf("Características normalizadas (Quake III) - %s:\n", files[f]);
        print_resource_usage("Uso de Recursos no Quake III (Início)", &start_usage);
        print_resource_usage("Uso de Recursos no Quake III (Fim)", &end_usage);

        // Normalização com SSE
        get_resource_usage(&start_usage);
        for (int i = 0; i < num_elements; i++) {
            normalize_feature_vector_sse(features[i], num_dimensions);
        }
        get_resource_usage(&end_usage);
        printf("Características normalizadas (SSE) - %s:\n", files[f]);
        print_resource_usage("Uso de Recursos no SSE (Início)", &start_usage);
        print_resource_usage("Uso de Recursos no SSE (Fim)", &end_usage);

        // Liberar memória alocada
        for (int i = 0; i < num_elements; i++) {
            free(features[i]);
        }
        free(features);
    }

    return 0;
}