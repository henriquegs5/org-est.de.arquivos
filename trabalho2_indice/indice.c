// Trabalho 2 - Criação e Uso de Índice
// Integrante: Henrique Guimarães Silva

// Descrição:
//  1. Leê o arquivo de dados original (cep.dat) e constrói um vetor de índices
//  contendo o CEP e a posição (offset em bytes) de cada registro.
//  2. Ordena o vetor de índices por CEP com qsort.
//  3. Grava o índice ordenado em 'indice.dat'.
//  4. Realiza busca binária no arquivo de índice.
//  5. A partir da posição armazenada no índice, recupera o registro
//  correspondente no arquivo original e o exibe.
// Uso: ./indice <CEP>
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
#define ARQUIVO_DADOS  "cep.dat"
#define ARQUIVO_INDICE "indice.dat"
#define TAM_CEP        8
 
// - Estruturas
 
typedef struct {
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
} Endereco;
 
typedef struct {
    char cep[8];
    long posicao;   // offset em bytes no arquivo de dados
} Indice;
 
// - Auxiliares
 
static int comparar_indices(const void *a, const void *b)
{
    return strncmp(((const Indice *)a)->cep,
                   ((const Indice *)b)->cep, TAM_CEP);
}
 
static void imprimir_endereco(const Endereco *e)
{
    printf("Logradouro : %.72s\n", e->logradouro);
    printf("Bairro     : %.72s\n", e->bairro);
    printf("Cidade     : %.72s\n", e->cidade);
    printf("UF         : %.72s\n", e->uf);
    printf("Sigla      : %.2s\n",  e->sigla);
    printf("CEP        : %.8s\n",  e->cep);
}
 
// - Etapa 1: construir e salvar o indice
 
static Indice *construir_indice(FILE *dados, long total, const char *caminho_idx)
{
    Indice *idx = malloc(total * sizeof(Indice));
    if (!idx) {
        fprintf(stderr, "Erro: sem memoria para o indice.\n");
        return NULL;
    }
 
    rewind(dados);
    Endereco e;
    long pos = 0;
 
    for (long j = 0; fread(&e, sizeof(Endereco), 1, dados) == 1; j++) {
        strncpy(idx[j].cep, e.cep, TAM_CEP);
        idx[j].posicao = pos;
        pos += (long)sizeof(Endereco);
    }
 
    qsort(idx, total, sizeof(Indice), comparar_indices);
 
    FILE *arq_idx = fopen(caminho_idx, "wb");
    if (!arq_idx) {
        fprintf(stderr, "Erro: nao foi possivel criar '%s'.\n", caminho_idx);
        free(idx);
        return NULL;
    }
    fwrite(idx, sizeof(Indice), total, arq_idx);
    fclose(arq_idx);
 
    printf("Indice criado: '%s' (%ld entradas).\n\n", caminho_idx, total);
    return idx;
}
 
// - Etapa 2: busca binaria no indice
// Busca 'cep_busca' no arquivo de indice.
// Devolve o offset do registro no arquivo de dados, ou -1 se nao achar.

static long buscar_no_indice(FILE *arq_idx, long total, const char *cep_busca, int *comparacoes)
{
    long inicio = 0, fim = total - 1;
    *comparacoes = 0;
    Indice entrada;
 
    while (inicio <= fim) {
        long meio = inicio + (fim - inicio) / 2;
        (*comparacoes)++;
 
        fseek(arq_idx, meio * (long)sizeof(Indice), SEEK_SET);
        if (fread(&entrada, sizeof(Indice), 1, arq_idx) != 1)
            break;
 
        int cmp = strncmp(cep_busca, entrada.cep, TAM_CEP);
 
        if (cmp == 0)
            return entrada.posicao;
        else if (cmp < 0)
            fim = meio - 1;
        else
            inicio = meio + 1;
    }
    return -1L;
}
 
/* ---- main ---- */
 
int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <CEP>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 01310100\n", argv[0]);
        return 1;
    }
 
    /* Abre arquivo de dados */
    FILE *dados = fopen(ARQUIVO_DADOS, "rb");
    if (!dados) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", ARQUIVO_DADOS);
        return 1;
    }
 
    fseek(dados, 0, SEEK_END);
    long total = ftell(dados) / (long)sizeof(Endereco);
    rewind(dados);
 
    printf("Tamanho do registro : %zu bytes\n", sizeof(Endereco));
    printf("Total de registros  : %ld\n\n", total);
 
    /* Constroi e salva o indice */
    Indice *idx = construir_indice(dados, total, ARQUIVO_INDICE);
    if (!idx) { fclose(dados); return 1; }
 
    /* Reabre o arquivo de indice para a busca binaria */
    FILE *arq_idx = fopen(ARQUIVO_INDICE, "rb");
    if (!arq_idx) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", ARQUIVO_INDICE);
        free(idx); fclose(dados);
        return 1;
    }
 
    int comparacoes = 0;
    long offset = buscar_no_indice(arq_idx, total, argv[1], &comparacoes);
 
    if (offset < 0) {
        printf("CEP '%s' nao encontrado no indice.\n", argv[1]);
    } else {
        /* Recupera o registro original pelo offset */
        fseek(dados, offset, SEEK_SET);
        Endereco e;
        if (fread(&e, sizeof(Endereco), 1, dados) == 1) {
            printf("Registro encontrado:\n");
            printf("--------------------\n");
            imprimir_endereco(&e);
        } else {
            printf("Erro ao ler o registro no arquivo de dados.\n");
        }
    }
 
    printf("\nComparacoes realizadas: %d\n", comparacoes);
 
    fclose(arq_idx);
    fclose(dados);
    free(idx);
    return 0;
}
 
