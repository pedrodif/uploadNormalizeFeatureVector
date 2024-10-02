#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <xmmintrin.h>  // Para usar as instruções SSE
#define _GNU_SOURCE

// Função para calcular a raiz quadrada inversa de forma otimizada com SSE
float fast_inverse_sqrt(float number) {
    __m128 input_value = _mm_set_ss(number);  // Armazena o valor no registrador SIMD
    input_value = _mm_rsqrt_ss(input_value);  // Usa SSE para calcular a raiz inversa
    return _mm_cvtss_f32(input_value);        // Converte o valor SIMD para float
}

// Normaliza o vetor de características usando a raiz quadrada inversa otimizada
void normalize_vector(float* vector, int size) {
    float magnitude_squared = 0.0f;

    // Calcula a soma dos quadrados dos elementos
    for (int idx = 0; idx < size; idx++) {
        magnitude_squared += vector[idx] * vector[idx];
    }

    float inv_sqrt_magnitude = fast_inverse_sqrt(magnitude_squared);

    // Aplica a normalização aos elementos do vetor
    for (int idx = 0; idx < size; idx++) {
        vector[idx] *= inv_sqrt_magnitude;
    }
}

// Lê dados de um arquivo CSV e os armazena em uma matriz de floats
float** load_csv_data(const char* filepath, int* rows, int* cols) {
    FILE* csv_file = fopen(filepath, "r");
    if (!csv_file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    *rows = 0;
    *cols = 0;
    char buffer[1024];

    // Identifica o número de colunas e linhas
    while (fgets(buffer, sizeof(buffer), csv_file)) {
        if (*rows == 0) {
            char* token = strtok(buffer, ",");
            while (token) {
                (*cols)++;
                token = strtok(NULL, ",");
            }
        }
        (*rows)++;
    }
    rewind(csv_file);

    // Aloca a matriz de características
    float** data_matrix = (float**)malloc(*rows * sizeof(float*));
    for (int i = 0; i < *rows; i++) {
        data_matrix[i] = (float*)malloc(*cols * sizeof(float));
    }

    // Preenche a matriz com os dados do CSV
    int row_idx = 0;
    while (fgets(buffer, sizeof(buffer), csv_file)) {
        int col_idx = 0;
        char* token = strtok(buffer, ",");
        while (token) {
            data_matrix[row_idx][col_idx++] = atof(token);
            token = strtok(NULL, ",");
        }
        row_idx++;
    }

    fclose(csv_file);
    return data_matrix;
}

// Coleta informações sobre o uso de recursos do sistema
void fetch_resource_usage(struct rusage* resource_data) {
    getrusage(RUSAGE_SELF, resource_data);
}

// Exibe informações sobre o uso de recursos
void display_resource_usage(const char* stage, struct rusage* usage_info) {
    printf("%s\n", stage);
    printf("Tempo de usuário: %ld.%06ld segundos\n", usage_info->ru_utime.tv_sec, usage_info->ru_utime.tv_usec);
    printf("Tempo de sistema: %ld.%06ld segundos\n", usage_info->ru_stime.tv_sec, usage_info->ru_stime.tv_usec);
    printf("Tamanho máximo da memória residente: %ld kilobytes\n", usage_info->ru_maxrss);
}

int main() {
    int element_count, dimension_count;
    float** feature_matrix = load_csv_data("data.csv", &element_count, &dimension_count);

    struct rusage usage_before, usage_after;

    // Coleta o uso de recursos antes da normalização
    fetch_resource_usage(&usage_before);

    // Normaliza cada vetor de características
    for (int i = 0; i < element_count; i++) {
        normalize_vector(feature_matrix[i], dimension_count);
    }

    // Coleta o uso de recursos após a normalização
    fetch_resource_usage(&usage_after);

    // Exibe os vetores normalizados
    printf("Vetor normalizado:\n");
    for (int i = 0; i < element_count; i++) {
        for (int j = 0; j < dimension_count; j++) {
            printf("%f ", feature_matrix[i][j]);
        }
        printf("\n");
    }

    // Mostra o uso de recursos no início e no fim
    printf("Uso de recursos durante a execução:\n");
    display_resource_usage("Uso inicial", &usage_before);
    display_resource_usage("Uso final", &usage_after);

    // Liberação da memória alocada
    for (int i = 0; i < element_count; i++) {
        free(feature_matrix[i]);
    }
    free(feature_matrix);

    return 0;
}
