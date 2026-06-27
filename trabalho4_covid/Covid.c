// para dar run no arquivo é so usar esse comando:
// gcc Covid.c CSVParser.c -o Covid -lm
//  ./Covid
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "CSVParser.h"

#define READ_BUF_SIZE 8192

struct DadosPais
{
    char location[64];
    double totalCases;
    double totalDeaths;
};

struct Acumulador
{
    int linhaCount;
    int headerPulado;
    struct DadosPais paises[20];
    int qtdPaises;
};

int getPaisIndex(struct Acumulador *acc, const char *nome)
{
    for (int i = 0; i < acc->qtdPaises; i++)
    {
        if (strcmp(acc->paises[i].location, nome) == 0)
            return i;
    }
    int i = acc->qtdPaises++;
    strncpy(acc->paises[i].location, nome, 63);
    acc->paises[i].totalCases = 0;
    acc->paises[i].totalDeaths = 0;
    return i;
}

void callback(char **cols, int ncols, void *userData)
{
    struct Acumulador *acc = (struct Acumulador *)userData;

    if (!acc->headerPulado)
    {
        acc->headerPulado = 1;
        return;
    }

    if (ncols < 8)
        return;

    if (strcmp(cols[1], "South America") != 0)
        return;

    double cases = (cols[4][0] != '\0') ? atof(cols[4]) : 0;
    double deaths = (cols[7][0] != '\0') ? atof(cols[7]) : 0;

    int idx = getPaisIndex(acc, cols[2]);
    if (cases > acc->paises[idx].totalCases)
        acc->paises[idx].totalCases = cases;
    if (deaths > acc->paises[idx].totalDeaths)
        acc->paises[idx].totalDeaths = deaths;

    acc->linhaCount++;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    struct Acumulador acc;
    acc.linhaCount = 0;
    acc.headerPulado = 0;
    acc.qtdPaises = 0;

    char *buf = (char *)malloc(READ_BUF_SIZE);
    CSVParser csv;
    CSVParser_init(&csv);

    FILE *f = fopen("owid-covid-data.csv", "rb");
    if (!f)
    {
        printf("Erro: nao foi possivel abrir o arquivo owid-covid-data.csv\n");
        free(buf);
        return 1;
    }

    int qt = fread(buf, 1, READ_BUF_SIZE, f);
    while (qt > 0)
    {
        CSVParser_processLines(&csv, buf, qt, callback, &acc);
        qt = fread(buf, 1, READ_BUF_SIZE, f);
    }
    fclose(f);

    CSVParser_processLines(&csv, "\n", 1, callback, &acc);

    free(buf);

    printf("============================================================\n");
    printf("  TOTAL DE CASOS E MORTES - AMERICA DO SUL (COVID-19)\n");
    printf("============================================================\n");
    printf("%-30s %15s %15s\n", "País", "Total Casos", "Total Mortes");
    printf("------------------------------------------------------------\n");

    double somaTotal_cases = 0;
    double somaTotal_deaths = 0;

    for (int i = 0; i < acc.qtdPaises; i++)
    {
        printf("%-30s %15.0f %15.0f\n",
               acc.paises[i].location,
               acc.paises[i].totalCases,
               acc.paises[i].totalDeaths);
        somaTotal_cases += acc.paises[i].totalCases;
        somaTotal_deaths += acc.paises[i].totalDeaths;
    }

    printf("============================================================\n");
    printf("%-30s %15.0f %15.0f\n", "TOTAL GERAL", somaTotal_cases, somaTotal_deaths);
    printf("============================================================\n");
    printf("Linhas de dados processadas: %d\n", acc.linhaCount);
    printf("Paises encontrados: %d\n", acc.qtdPaises);

    return 0;
}
