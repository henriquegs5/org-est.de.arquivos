#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_REGISTRO 300

void gerarAmostras()
{
    FILE *entrada = fopen("cep.dat", "r");
    FILE *cep1 = fopen("cep1.dat", "w");
    FILE *cep2 = fopen("cep2.dat", "w");

    if (entrada == NULL)
    {
        printf("Erro ao abrir cep.dat\n");
        return;
    }

    srand(time(NULL));

    char registro[TAM_REGISTRO + 1];

    while (fgets(registro, sizeof(registro), entrada))
    {
        if ((double)rand() / RAND_MAX <= 0.8)
            fputs(registro, cep1);

        if ((double)rand() / RAND_MAX <= 0.8)
            fputs(registro, cep2);
    }

    fclose(entrada);
    fclose(cep1);
    fclose(cep2);
}
