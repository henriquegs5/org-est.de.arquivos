// Trabalho 3 - Ordenacao Externa com K Blocos
// Integrante: Henrique Guimaraes Silva

// Descricao:
//  Implementa ordenacao externa em tres fases:
//    Fase 1 - Divisao  : divide 'cep.dat' em K blocos de tamanho aproximado igual, ordena cada bloco em memoria e o salva em disco.
//    Fase 2 - Intercala: combina os blocos dois a dois em rodadas sucessivas ate restar um unico arquivo ordenado.
//    Fase 3 - Resultado: renomeia o arquivo final para 'cep_ordenado.dat'.

// - K deve ser potencia de 2 (padrao: 4).
// Uso: ./ordenacao_externa [K]
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
// - Configuracoes
#define ARQUIVO_ENTRADA "cep.dat"
#define ARQUIVO_SAIDA   "cep_ordenado.dat"
#define K_PADRAO        4     /* potencia de 2 */
#define NOME_MAX        64
 
typedef struct {
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
} Endereco;
 
// - Comparacao para qsort
static int comparar_cep(const void *a, const void *b)
{
    return strncmp(((const Endereco *)a)->cep,
                   ((const Endereco *)b)->cep, 8);
}
 
// - Monta nomes de arquivos temporarios
static void nome_bloco(char *buf, int bloco)
{
    sprintf(buf, "bloco_%02d.tmp", bloco);
}
 
static void nome_temp(char *buf, int rodada, int par)
{
    sprintf(buf, "merge_%02d_%02d.tmp", rodada, par);
}
 
// - Fase 1: divisao e ordenacao interna
static int fase_divisao(FILE *entrada, long total, int k)
{
    long base        = total / k;
    long excedente   = total % k;       // os primeiros 'excedente' blocos recebem um registro a mais
 
    for (int b = 0; b < k; b++) {
        long tam_bloco = base + (b < excedente ? 1 : 0);
 
        Endereco *buf = malloc(tam_bloco * sizeof(Endereco));
        if (!buf) {
            fprintf(stderr, "Erro: sem memoria para o bloco %d.\n", b);
            return 0;
        }
 
        long lidos = (long)fread(buf, sizeof(Endereco), tam_bloco, entrada);
        if (lidos == 0) { free(buf); break; }
 
        qsort(buf, lidos, sizeof(Endereco), comparar_cep);
 
        char nome[NOME_MAX];
        nome_bloco(nome, b);
        FILE *f = fopen(nome, "wb");
        if (!f) {
            fprintf(stderr, "Erro: nao foi possivel criar '%s'.\n", nome);
            free(buf); return 0;
        }
        fwrite(buf, sizeof(Endereco), lidos, f);
        fclose(f);
        free(buf);
 
        printf("  Bloco %d criado: '%s' (%ld registros).\n", b, nome, lidos);
    }
    return 1;
}
 
// - Intercalacao de dois arquivos abertos numa saida
static void intercalar(FILE *f1, FILE *f2, FILE *saida)
{
    Endereco e1, e2;
    int tem1 = (fread(&e1, sizeof(Endereco), 1, f1) == 1);
    int tem2 = (fread(&e2, sizeof(Endereco), 1, f2) == 1);
 
    while (tem1 || tem2) {
        Endereco *escolhido;
 
        if (tem1 && tem2) {
            escolhido = (strncmp(e1.cep, e2.cep, 8) <= 0) ? &e1 : &e2;
        } else {
            escolhido = tem1 ? &e1 : &e2;
        }
 
        fwrite(escolhido, sizeof(Endereco), 1, saida);
 
        // Avanca no arquivo do registro escrito
        if (escolhido == &e1)
            tem1 = (fread(&e1, sizeof(Endereco), 1, f1) == 1);
        else
            tem2 = (fread(&e2, sizeof(Endereco), 1, f2) == 1);
    }
}
 
// - Fase 2: intercalacao em rodadas
// Retorna o nome do arquivo final gerado (alocado em buf_saida).
// buf_saida deve ter pelo menos NOME_MAX bytes.

static int fase_intercalacao(int k, char *buf_saida)
{
    int ativos  = k;   // quantos arquivos existem nesta rodada
    int rodada  = 0;
 
    while (ativos > 1) {
        int proximos = 0;  // arquivos que sobram para a proxima rodada
 
        for (int par = 0; par < ativos; par += 2) {
            char n1[NOME_MAX], n2[NOME_MAX], ns[NOME_MAX];
 
            // Monta os nomes dos arquivos de entrada desta rodada
            if (rodada == 0) {
                nome_bloco(n1, par);
                nome_bloco(n2, par + 1);
            } else {
                nome_temp(n1, rodada - 1, par);
                nome_temp(n2, rodada - 1, par + 1);
            }
            nome_temp(ns, rodada, proximos);
 
            // Se existir apenas um arquivo sem par, passa direto
            if (par + 1 >= ativos) {
                // renomeia n1 -> ns
                rename(n1, ns);
                proximos++;
                continue;
            }
 
            FILE *f1 = fopen(n1, "rb");
            FILE *f2 = fopen(n2, "rb");
            FILE *fs = fopen(ns, "wb");
 
            if (!f1 || !f2 || !fs) {
                fprintf(stderr, "Erro ao abrir arquivos na rodada %d par %d.\n",
                        rodada, par);
                if (f1) fclose(f1);
                if (f2) fclose(f2);
                if (fs) fclose(fs);
                return 0;
            }
 
            intercalar(f1, f2, fs);
 
            fclose(f1); fclose(f2); fclose(fs);
 
            // Remove temporarios anteriores
            remove(n1);
            remove(n2);
 
            printf("  Rodada %d: '%s' + '%s' -> '%s'\n",
                   rodada, n1, n2, ns);
            proximos++;
        }
 
        ativos = proximos;
        rodada++;
    }
 
    // O unico arquivo restante e o resultado
    if (rodada == 0)
        nome_bloco(buf_saida, 0);           // K == 1, nao houve intercalacao
    else
        nome_temp(buf_saida, rodada - 1, 0);
 
    return 1;
}
 
// - main
 
int main(int argc, char **argv)
{
    int k = K_PADRAO;
    if (argc == 2) {
        k = atoi(argv[1]);
        if (k < 2 || (k & (k - 1)) != 0) {
            fprintf(stderr, "K deve ser uma potencia de 2 maior que 1.\n");
            return 1;
        }
    }
 
    printf("=== Ordenacao Externa (K = %d) ===\n\n", k);
 
    // Abre arquivo de entrada
    FILE *entrada = fopen(ARQUIVO_ENTRADA, "rb");
    if (!entrada) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", ARQUIVO_ENTRADA);
        return 1;
    }
 
    fseek(entrada, 0, SEEK_END);
    long total = ftell(entrada) / (long)sizeof(Endereco);
    rewind(entrada);
 
    printf("Arquivo    : %s\n", ARQUIVO_ENTRADA);
    printf("Registros  : %ld\n", total);
    printf("Tam. struct: %zu bytes\n\n", sizeof(Endereco));
 
    // Fase 1
    printf("-- Fase 1: divisao e ordenacao interna --\n");
    if (!fase_divisao(entrada, total, k)) {
        fclose(entrada); return 1;
    }
    fclose(entrada);
 
    // Fase 2
    printf("\n-- Fase 2: intercalacao --\n");
    char arquivo_final[NOME_MAX];
    if (!fase_intercalacao(k, arquivo_final)) return 1;
 
    // Fase 3: resultado final
    remove(ARQUIVO_SAIDA);
    if (rename(arquivo_final, ARQUIVO_SAIDA) != 0) {
        fprintf(stderr, "Aviso: nao foi possivel renomear para '%s'.\n",
                ARQUIVO_SAIDA);
        printf("Arquivo final: '%s'\n", arquivo_final);
    } else {
        printf("\nArquivo ordenado salvo em '%s'.\n", ARQUIVO_SAIDA);
    }
 
    printf("Ordenacao concluida com sucesso.\n");
    return 0;
}
