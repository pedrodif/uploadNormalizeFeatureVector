#include <stdio.h>
#include <stdint.h> // Para manipulação de bits com tipos inteiros de tamanho fixo
#include <sys/resource.h> // Para medir o uso de recursos
#include <stdlib.h> // Para uso de atof
#include <math.h> // Para uso de sqrt
#include <string.h> // Para uso de strtok

// Função para calcular a raiz quadrada inversa utilizando a otimização do Quake III
float quake3_rsqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;                      // Trata a representação de bits de um float como um inteiro
    i = 0x5f3759df - (i >> 1);           // Faz o "magic number" para uma estimativa inicial
    y = *(float*)&i;                     // Converte de volta para float

    // Uma iteração de Newton-Raphson para refinar o resultado
    y = y * (threehalfs - (x2 * y * y));

    return y;
}

// Função para normalizar um vetor de características utilizando a otimização do Quake III
void normalize_feature_vector_quake3(float* features, int length) {
    float sum = 0.0f;

    // Calcula a soma dos quadrados dos elementos do vetor
    for (int i = 0; i < length; i++) {
        sum += features[i] * features[i];
    }

    // Usa a raiz quadrada inversa otimizada do Quake III
    float inv_sqrt = quake3_rsqrt(sum);

    // Multiplica cada elemento do vetor pela raiz quadrada inversa para normalizá-lo
    for (int i = 0; i < length; i++) {
        features[i] *= inv_sqrt;
    }
}

// Função para ler dados de um arquivo CSV
float** read_csv(const char* filename, int* num_elements, int* num_dimensions) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Determinar o número de elementos e dimensões
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

    // Alocar memória para as características
    float** features = (float**)malloc(*num_elements * sizeof(float*));
    for (int i = 0; i < *num_elements; i++) {
        features[i] = (float*)malloc(*num_dimensions * sizeof(float));
    }

    // Ler os dados
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

// Função para medir o uso de recursos
void get_resource_usage(struct rusage* usage) {
    getrusage(RUSAGE_SELF, usage);
}

void print_resource_usage(const char* label, struct rusage* usage) {
    printf("%s\n", label);
    printf("User time: %ld.%06ld seconds\n", usage->ru_utime.tv_sec, usage->ru_utime.tv_usec);
    printf("System time: %ld.%06ld seconds\n", usage->ru_stime.tv_sec, usage->ru_stime.tv_usec);
    printf("Maximum resident set size: %ld kilobytes\n", usage->ru_maxrss);
}

int main() {
    int num_elements, num_dimensions;
    
    // Carregar os dados do arquivo CSV
    float** features = read_csv("data.csv", &num_elements, &num_dimensions);

    struct rusage start_usage, end_usage;

    printf("Normalização utilizando a otimização do Quake III:\n");

    // Medição de tempo e recursos antes da normalização
    get_resource_usage(&start_usage);

    // Normalizar cada vetor
    for (int i = 0; i < num_elements; i++) {
        normalize_feature_vector_quake3(features[i], num_dimensions);
    }

    // Medição de tempo e recursos após a normalização
    get_resource_usage(&end_usage);

    // Mostrar o vetor normalizado
    printf("Vetor normalizado:\n");
    for (int i = 0; i < num_elements; i++) {
        for (int j = 0; j < num_dimensions; j++) {
            printf("%f ", features[i][j]);
        }
        printf("\n");
    }

    // Mostrar o uso de recursos
    printf("\nMedição de tempo e uso de recursos:\n");
    print_resource_usage("Start Usage", &start_usage);
    print_resource_usage("End Usage", &end_usage);

    // Liberar a memória alocada
    for (int i = 0; i < num_elements; i++) {
        free(features[i]);
    }
    free(features);

    return 0;
}
