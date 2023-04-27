#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int** lerArquivo(int *numeroLinhasPonteiro){
    FILE *fp;
    char nomeArquivo[] = "attitude_exam.log";
    char linha[100];
    int **matrizArquivo;
    int numeroLinhas = 0;

    printf("Tentando abrir o arquivo\n");
    fp = fopen(nomeArquivo, "r");
    if (fp == NULL) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }
    printf("Arquivo aberto com sucesso\n");

    printf("Contando as linhas\n");
    while (fgets(linha, 100, fp) != NULL) {
        numeroLinhas++;
    }

    *numeroLinhasPonteiro = numeroLinhas;

    printf("%d linhas foram encontradas, alocando a matriz\n", numeroLinhas);
    matrizArquivo = new int*[numeroLinhas];
    for (int i = 0; i < numeroLinhas; i++) {
        matrizArquivo[i] = new int[4];
    }
    printf("Matriz alocada com sucesso!\n");

    printf("Retornando ao inicio do arquivo\n");
    fseek(fp, 0, SEEK_SET);

    printf("Lendo linha por linha e salvando na matriz\n");
    for (int i = 0; i < numeroLinhas; i++) {
        fgets(linha, 100, fp);
        sscanf(linha, "%d; %d; %d; %d;", &matrizArquivo[i][0], &matrizArquivo[i][1], &matrizArquivo[i][2], &matrizArquivo[i][3]);
    }

    printf("Fechando arquivo\n");
    fclose(fp);

    return matrizArquivo;
}

float** estimmarRollPitch(int **matrizArquivo, int numeroLinhas){
    double acel_x, acel_y, acel_z;
    float **matrizEstimada;

    printf("Alocando matriz para as estimacoes\n");
    matrizEstimada = new float*[numeroLinhas];
    for (int i = 0; i < numeroLinhas; i++) {
        matrizEstimada[i] = new float[4];
    }
    printf("Matriz alocada com sucesso! Iniciando calculos do Pitch e Roll\n");

    for (int i = 0; i < numeroLinhas; i++) {
        acel_x = matrizArquivo[i][1];
        acel_y = matrizArquivo[i][2];
        acel_z = matrizArquivo[i][3];

        matrizEstimada[i][0] = matrizArquivo[i][0];

        matrizEstimada[i][1] = atan2(-acel_y, sqrt(acel_x * acel_x + acel_z * acel_z)) * (180.0 / M_PI);

        matrizEstimada[i][2] = atan2(acel_y, acel_z) * (180.0 / M_PI);
    }
    printf("Calculos completos\n");

    return matrizEstimada;
}

void salvarValoresEmArquivo(float** estimativas, int numeroLinhas){
    FILE *arquivo;
    arquivo = fopen("output_estimations.log", "w");

    printf("Abrindo ou criando arquivo, se necessario\n");
    if (arquivo == NULL) {
        printf("Erro criando arquivo!");
        return;
    }
    printf("Arquivo aberto com sucesso!\n");

    printf("Salvando matriz no arquivo no formato:\n<marcacaoDeTempo>; <anguloPitch>; <anguloRoll>\n");
    for (int i = 0; i < numeroLinhas; i++) {
        fprintf(arquivo, "%.f; %.4f; %.4f\n", estimativas[i][0], estimativas[i][1], estimativas[i][2]);
    }

    printf("Dados salvos em %s com sucesso!\n", "output_estimations.log");
    fclose(arquivo);
}

int main() {
    int numeroLinhas;

    int **matrizArquivo = lerArquivo(&numeroLinhas);
    float **matrizEstimada = estimmarRollPitch(matrizArquivo, numeroLinhas);
    salvarValoresEmArquivo(matrizEstimada, numeroLinhas);

    return 0;
}
