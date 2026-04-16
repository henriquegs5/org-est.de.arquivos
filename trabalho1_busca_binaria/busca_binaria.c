// Trabalho 1 - Busca Binária em Arquivo de CEP Ordenado
// Integrante: Henrique Guimarães Silva
// Descrição:
// Lê um arquivo binário de endereços previamente ordenado por CEP
// e realiza busca binaria para localizar um registro a partir do CEP
// passado como argumento na linha de comando.
 *
 * Uso: ./busca_binaria <CEP>
 */
 
#include <stdio.h>
#include <string.h>
 
#define ARQUIVO_CEP "cep_ordenado.dat"
#define TAM_CEP 8
 
struct Endereco {
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
} typedef struct Endereco;
 
// Imprime os dados de um endereco formatado
static void imprimir_endereco(const Endereco *e)
{
    printf("Logradouro : %.72s\n", e->logradouro);
    printf("Bairro     : %.72s\n", e->bairro);
    printf("Cidade     : %.72s\n", e->cidade);
    printf("UF         : %.72s\n", e->uf);
    printf("Sigla      : %.2s\n",  e->sigla);
    printf("CEP        : %.8s\n",  e->cep);
}
 
// Busca binária no arquivo ordenado.
// Retorna 1 se encontrado (preenche *resultado), 0 caso contrário.
// Armazena em *comparacoes o número de interações realizadas.
static int busca_binaria(FILE *f, long total_registros,
                         const char *cep_busca,
                         Endereco *resultado, int *comparacoes)
{
    long inicio = 0;
    long fim    = total_registros - 1;
    *comparacoes = 0;
 
    while (inicio <= fim) {
        long meio = inicio + (fim - inicio) / 2;   // evita overflow
        (*comparacoes)++;
 
        fseek(f, meio * (long)sizeof(Endereco), SEEK_SET);
        if (fread(resultado, sizeof(Endereco), 1, f) != 1)
            break;
 
        int cmp = strncmp(cep_busca, resultado->cep, TAM_CEP);
 
        if (cmp == 0)
            return 1;          // encontrado
        else if (cmp < 0)
            fim = meio - 1;    // buscar na metade esquerda
        else
            inicio = meio + 1; // buscar na metade direita
    }
    return 0;
}
 
int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <CEP>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 01310100\n", argv[0]);
        return 1;
    }
 
    FILE *f = fopen(ARQUIVO_CEP, "rb");
    if (!f) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", ARQUIVO_CEP);
        return 1;
    }
 
    // Calcula o número de registros
    fseek(f, 0, SEEK_END);
    long total = ftell(f) / (long)sizeof(Endereco);
    rewind(f);
 
    printf("Tamanho do registro : %zu bytes\n", sizeof(Endereco));
    printf("Total de registros  : %ld\n\n", total);
 
    Endereco resultado;
    int comparacoes = 0;
 
    if (busca_binaria(f, total, argv[1], &resultado, &comparacoes)) {
        printf("Registro encontrado:\n");
        printf("-------------------\n");
        imprimir_endereco(&resultado);
    } else {
        printf("CEP '%s' nao encontrado.\n", argv[1]);
    }
 
    printf("\nComparacoes realizadas: %d\n", comparacoes);
 
    fclose(f);
    return 0;
}
