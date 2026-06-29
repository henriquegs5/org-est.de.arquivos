#include <stdio.h>
#include <string.h>
#include "ArvoreB.h"

#define TAM_REGISTRO 300

void criarIndice()
{
    FILE *arquivo = fopen("cep1.dat", "r");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir cep1.dat\n");
        return;
    }

    ArvoreB *arvore = ArvoreB_Abre("indice.dat");

    char registro[TAM_REGISTRO + 1];
    char cep[TAM_CHAVE];

    long posicao = 0;

    while (fgets(registro, sizeof(registro), arquivo))
    {
        memcpy(cep, registro + 291, TAM_CHAVE);

        ArvoreB_Insere(arvore, cep, posicao);

        posicao++;
    }

    ArvoreB_Fecha(arvore);
    fclose(arquivo);
}
