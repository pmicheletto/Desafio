# Estimativa dos valores de Pitch e Roll

## Lendo o Arquivo

Inicialmente, o primeiro passo é ler o arquivo e salvar os valores em uma variável. <br>
Como o arquivo pode conter um número N de linhas e 4 colunas, sendo elas a timestamp, aceleração em X, Y e Z, optou-se por uma matriz Nx4. <br>
Como o valor de N pode variar, a matriz será alocada dinamicamente.<br>

Assim, o primeiro passo é contar quantas linhas o arquivo tem, para isso, o arquivo é aberto e percorrido até o final em um loop, salvando a quantidade de linhas em uma variável.<br>
Como a quantidade de linhas é importante, o valor será salvo por referencia em uma variável na função main.<br>

Assim, o protótipo da função "lerArquivo" é:
```
int** lerArquivo(int *numeroLinhasPonteiro)
```
A abertura do arquivo é dada por:
```
    FILE *fp;
    char nomeArquivo[] = "attitude_exam.log";
    char buffer[100];

    printf("Tentando abrir o arquivo\n");
    fp = fopen(nomeArquivo, "r");
    if (fp == NULL) {
        printf("Erro ao abrir arquivo.\n");
        exit(1);
    }
    printf("Arquivo aberto com sucesso\n");
```
Em seguida, o arquivo é percorrido contando a quantidade de linhas e salvando em uma variável local, que é salva no ponteiro:
```
    printf("Contando as linhas\n");
    while (fgets(linha, 100, fp) != NULL) {
        numeroLinhas++;
    }

    *numeroLinhasPonteiro = numeroLinhas;
```
Com isso, a matriz pode ser criada:
```
    printf("%d linhas foram encontradas, alocando a matriz\n", numeroLinhas);
    matrizArquivo = new int*[numeroLinhas];
    for (int i = 0; i < numeroLinhas; i++) {
        matrizArquivo[i] = new int[4];
    }
    printf("Matriz alocada com sucesso!\n");
```
Para por fim, voltar ao inicio do arquivo e salvar os valores em tal matriz:
```
    printf("Retornando ao inicio do arquivo\n");
    fseek(fp, 0, SEEK_SET);

    printf("Lendo linha por linha e salvando na matriz\n");
    for (int i = 0; i < numeroLinhas; i++) {
        fgets(linha, 100, fp);
        sscanf(linha, "%d; %d; %d; %d;", &matrizArquivo[i][0], &matrizArquivo[i][1], &matrizArquivo[i][2], &matrizArquivo[i][3]);
    }

    printf("Fechando arquivo\n");
    fclose(fp);
```
## Estimando Roll e Pitch

Para realizar a estimativa de roll e de pitch, foram utilizadas as equações 28 e 29 do artigo [Tilt Sensing using a Three-Axix Accelerometer](https://www.nxp.com/docs/en/application-note/AN3461.pdf).

$$
    pitch = tan(\theta) = \tan(G_y/\sqrt(G_x^2+G_z^2))
$$
$$
    roll = tan(\theta) = \tan(-G_x/G_z)
$$

Para encontrar o angulo $\theta$ é necessário realizar o arcotangente, que derá o resultado em radianos, em seguida multiplicar o resultado por $(180/\pi)$

No código, a primeira coisa a ser feita é alocar uma matriz de float para salvar o resultado, onde a primeira coluna será o tempo (timestamp em ms), a segunda será o angulo Pitch e a terceira o Roll, em graus.

Assim, o protótipo da função será:
```
float** estimmarRollPitch(int **matrizArquivo, int numeroLinhas)
```

Primeiramente, a matriz será alocada:
```
    printf("Alocando matriz para as estimacoes\n");
    matrizEstimada = new float*[numeroLinhas];
    for (int i = 0; i < numeroLinhas; i++) {
        matrizEstimada[i] = new float[4];
    }
    printf("Matriz alocada com sucesso! Iniciando calculos do Pitch e Roll\n");
```

E por fim, para cada linha, serão realizados os calculos e o valor do timeStamp será apenas copiado:
```
    for (int i = 0; i < numeroLinhas; i++) {
        acel_x = matrizArquivo[i][1];
        acel_y = matrizArquivo[i][2];
        acel_z = matrizArquivo[i][3];

        matrizEstimada[i][0] = matrizArquivo[i][0];

        matrizEstimada[i][1] = atan2(-acel_y, sqrt(acel_x * acel_x + acel_z * acel_z)) * (180.0 / M_PI);

        matrizEstimada[i][2] = atan2(acel_y, acel_z) * (180.0 / M_PI);
    }
    printf("Calculos completos\n");
```

## Escrevendo arquivo de saída

Para salvar o arquivo, será mantido o padrão do arquivo original, salvando da seguinte forma:
```
<MarcacaoDeTempo>; <anguloPitch>; <anguloRoll>
```
O protótipo da função para salvar será:
```
void salvarValoresEmArquivo(float** estimativas, int numeroLinhas)
```
Pra isso, primeiramente, o arquivo `output_estimations.log` é aberto, caso não exista, é criado:
```
    FILE *arquivo;
    arquivo = fopen("output_estimations.log", "w");

    printf("Abrindo ou criando arquivo, se necessario\n");
    if (arquivo == NULL) {
        printf("Erro criando arquivo!");
        return;
    }
    printf("Arquivo aberto com sucesso!\n");
```
Em seguida, os dados são escritos no arquivo:
```
    printf("Salvando matriz no arquivo no formato:\n<marcacaoDeTempo>; <anguloPitch>; <anguloRoll>\n");
    for (int i = 0; i < numeroLinhas; i++) {
        fprintf(arquivo, "%.f; %.4f; %.4f\n", estimativas[i][0], estimativas[i][1], estimativas[i][2]);
    }

    printf("Dados salvos em %s com sucesso!\n", "output_estimations.log");
    fclose(arquivo);
```
## Chamada das funções

Para utilizar o código, tudo que é necessário, é ter o arquivo na mesma pasta que o código e executa-lo.
A função main com a chamada das funções é a seguinte:
```
int main() {
    int numeroLinhas;

    int **matrizArquivo = lerArquivo(&numeroLinhas);
    float **matrizEstimada = estimmarRollPitch(matrizArquivo, numeroLinhas);
    salvarValoresEmArquivo(matrizEstimada, numeroLinhas);

    return 0;
}
```
Após a execução, os resultados serão salvos no arquivo `output_estimations.log` com cada linha sendo:
```
<MarcacaoDeTempo>; <anguloPitch>; <anguloRoll>
```
