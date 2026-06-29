#include <stdio.h>
#include <string.h>
#include "ArvoreB.h"

#define TAM_REGISTRO 300

void gerarResultado()
{
    FILE *arquivo = fopen("cep2.dat", "r");
    FILE *resultado = fopen("resultado.dat", "w");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir cep2.dat\n");
        return;
    }

    ArvoreB *arvore = ArvoreB_Abre("indice.dat");

    char registro[TAM_REGISTRO + 1];
    char cep[TAM_CHAVE];

    while (fgets(registro, sizeof(registro), arquivo))
    {
        memcpy(cep, registro + 291, TAM_CHAVE);

        if (ArvoreB_Busca(arvore, cep) != -1)
        {
            fputs(registro, resultado);
        }
    }

    ArvoreB_Fecha(arvore);

    fclose(arquivo);
    fclose(resultado);
}
