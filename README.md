# Organização de Estruturas de Arquivos
 
**Integrante:** Henrique Guimarães Silva
 
Repositório com os trabalhos da primeira etapa da disciplina de Organização de Arquivos.
 
---
 
## Estrutura do repositório
 
```
.
├── trabalho1_busca_binaria/
│   └── busca_binaria.c
├── trabalho2_indice/
│   └── indice.c
└── trabalho3_ordenacao_externa/
    └── ordenacao_externa.c
```
 
---
 
## Trabalho 1 — Busca Binária em Arquivo de CEP Ordenado
 
**Arquivo:** `trabalho1_busca_binaria/busca_binaria.c`
 
Realiza busca binária diretamente em `cep_ordenado.dat` (arquivo binário de registros `Endereco` previamente ordenado por CEP).
 
**Como compilar e executar:**
```bash
gcc busca_binaria.c -o busca_binaria
./busca_binaria 01310100
```
 
---
 
## Trabalho 2 — Criação e Uso de Índice
 
**Arquivo:** `trabalho2_indice/indice.c`
 
1. Lê `cep.dat` e cria um vetor de índices `{ cep, posicao_em_bytes }`.
2. Ordena o vetor com `qsort` e salva em `indice.dat`.
3. Realiza busca binária no `indice.dat`.
4. Usa o offset armazenado para recuperar o registro no arquivo original.
**Como compilar e executar:**
```bash
gcc indice.c -o indice
./indice 01310100
```
 
---
 
## Trabalho 3 — Ordenação Externa com K Blocos
 
**Arquivo:** `trabalho3_ordenacao_externa/ordenacao_externa.c`
 
Ordena `cep.dat` em três fases:
 
| Fase | Descrição |
|------|-----------|
| **Divisão** | Divide o arquivo em K blocos, ordena cada um em memória e salva em disco. |
| **Intercalação** | Combina os blocos dois a dois em rodadas sucessivas. |
| **Resultado** | Salva o arquivo final em `cep_ordenado.dat`. |
 
K deve ser uma potência de 2 (padrão: 4).
 
**Como compilar e executar:**
```bash
gcc ordenacao_externa.c -o ordenacao_externa
./ordenacao_externa        # usa K = 4
./ordenacao_externa 8      # usa K = 8
```
 
---
 
## Arquivo de dados
 
Todos os programas esperam um arquivo binário chamado `cep.dat` (ou `cep_ordenado.dat` para o Trabalho 1) no mesmo diretório do executável, com registros de tamanho fixo correspondentes à estrutura:
 
```c
typedef struct {
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
} Endereco; // 300 bytes
```
