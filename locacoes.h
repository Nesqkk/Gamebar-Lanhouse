// Arquivo: locacoes.h | Modulo 4: Locacoes e Relatorios | Autor: Integrante D (Nome Completo e RGM)
#ifndef LOCACOES_H
#define LOCACOES_H

#include <stdio.h>
#include <string.h>
#include "clientes.h"   // para validar o cliente e debitar o saldo
#include "estacoes.h"   // para validar a estacao e mudar o status

#define MAX_LOCACOES 50

void limparTela(); // prototipo: a funcao completa fica no main.c da equipe

struct Locacao {
    int idLocacao;
    int idClienteLocador;
    int idEstacaoUsada;
    int horasContratadas;
    float valorTotal;      // horasContratadas * precoHora
};

struct Locacao historicoLocacoes[MAX_LOCACOES];
int totalLocacoes = 0;

// struct auxiliar do ranking (Bubble Sort)
struct RankingCliente {
    int idCliente;
    char nome[50];
    int horas;
};

// 1. PERSISTENCIA: regrava o vetor inteiro em locacoes.csv (modo "w" zera o arquivo)
void salvarLocacoesCSV() {
    FILE *arq = fopen("locacoes.csv", "w");
    if (arq == NULL) {
        printf("[ERRO] Nao foi possivel gravar locacoes.csv!\n");
        return;
    }
    for (int i = 0; i < totalLocacoes; i++) {
        fprintf(arq, "%d;%d;%d;%d;%.2f\n",
                historicoLocacoes[i].idLocacao,
                historicoLocacoes[i].idClienteLocador,
                historicoLocacoes[i].idEstacaoUsada,
                historicoLocacoes[i].horasContratadas,
                historicoLocacoes[i].valorTotal);
    }
    fclose(arq);
}

// 2. PERSISTENCIA: carrega locacoes.csv para o vetor ao abrir o programa
void carregarLocacoesCSV() {
    FILE *arq = fopen("locacoes.csv", "r");
    if (arq == NULL) return; // se ainda nao existir arquivo, inicia com 0 registros

    totalLocacoes = 0;
    while (totalLocacoes < MAX_LOCACOES &&
           fscanf(arq, "%d;%d;%d;%d;%f\n",
                  &historicoLocacoes[totalLocacoes].idLocacao,
                  &historicoLocacoes[totalLocacoes].idClienteLocador,
                  &historicoLocacoes[totalLocacoes].idEstacaoUsada,
                  &historicoLocacoes[totalLocacoes].horasContratadas,
                  &historicoLocacoes[totalLocacoes].valorTotal) == 5) {
        totalLocacoes++;
    }
    fclose(arq);
}

// 3. FUNCAO CHAVE-MESTRA: devolve a posicao do ID no vetor, ou -1 se nao existir
int buscarIndiceLocacao(int idProcurado) {
    for (int i = 0; i < totalLocacoes; i++) {
        if (historicoLocacoes[i].idLocacao == idProcurado) return i;
    }
    return -1;
}

// 4. AUTOINCREMENT: laco que acha o maior ID e devolve o proximo (nunca repete)
int gerarIdLocacao() {
    int maior = 0;
    for (int i = 0; i < totalLocacoes; i++) {
        if (historicoLocacoes[i].idLocacao > maior) maior = historicoLocacoes[i].idLocacao;
    }
    return maior + 1;
}

// 5. IMPRIME UMA LOCACAO COM O NOME DO CLIENTE E DA ESTACAO (cruzamento de modulos)
void imprimirLocacao(int i) {
    int ic = buscarIndiceCliente(historicoLocacoes[i].idClienteLocador);
    int ie = buscarIndiceEstacao(historicoLocacoes[i].idEstacaoUsada);
    printf("Loc %-3d | Cliente: %-22s | Estacao: %-22s | %2dh | R$ %.2f\n",
           historicoLocacoes[i].idLocacao,
           ic != -1 ? listaClientes[ic].nome : "(removido)",
           ie != -1 ? estacoes[ie].nomeEstacao : "(removida)",
           historicoLocacoes[i].horasContratadas,
           historicoLocacoes[i].valorTotal);
}

// 6. REGRA DE NEGOCIO: valida cliente e estacao, calcula o valor,
//    debita o saldo do cliente e marca a estacao como ocupada
void registrarLocacao() {
    if (totalLocacoes >= MAX_LOCACOES) {
        printf("[ERRO] Limite de %d locacoes atingido!\n", MAX_LOCACOES);
        return;
    }

    int idCliente, idEstacao, horas;

    printf("\nID do cliente: ");
    scanf("%d", &idCliente);
    int ic = buscarIndiceCliente(idCliente);
    if (ic == -1) {
        printf("[ERRO] Cliente nao encontrado!\n");
        return;
    }

    printf("ID da estacao: ");
    scanf("%d", &idEstacao);
    int ie = buscarIndiceEstacao(idEstacao);
    if (ie == -1) {
        printf("[ERRO] Estacao nao encontrada!\n");
        return;
    }
    if (estacoes[ie].ocupada) {
        printf("[ERRO] A estacao '%s' ja esta em uso!\n", estacoes[ie].nomeEstacao);
        return;
    }

    do {
        printf("Horas contratadas: ");
        scanf("%d", &horas);
        if (horas <= 0) printf("[ERRO] Informe pelo menos 1 hora!\n");
    } while (horas <= 0);

    float valor = horas * estacoes[ie].precoHora;
    if (listaClientes[ic].saldoCreditos < valor) {
        printf("[ERRO] Saldo insuficiente! Saldo: R$ %.2f | Necessario: R$ %.2f\n",
               listaClientes[ic].saldoCreditos, valor);
        return;
    }

    listaClientes[ic].saldoCreditos -= valor;   // abate os creditos do cliente
    estacoes[ie].ocupada = 1;                   // muda o status da estacao

    historicoLocacoes[totalLocacoes].idLocacao = gerarIdLocacao();
    historicoLocacoes[totalLocacoes].idClienteLocador = idCliente;
    historicoLocacoes[totalLocacoes].idEstacaoUsada = idEstacao;
    historicoLocacoes[totalLocacoes].horasContratadas = horas;
    historicoLocacoes[totalLocacoes].valorTotal = valor;
    totalLocacoes++;

    salvarLocacoesCSV();
    salvarClientesCSV();
    salvarEstacoesCSV();

    printf("[OK] Locacao registrada: %s em '%s' por %dh = R$ %.2f (saldo restante: R$ %.2f)\n",
           listaClientes[ic].nome, estacoes[ie].nomeEstacao, horas, valor,
           listaClientes[ic].saldoCreditos);
}

// 7. LISTAGEM DO HISTORICO
void listarLocacoes() {
    printf("\n--- HISTORICO DE LOCACOES (%d registros) ---\n", totalLocacoes);
    if (totalLocacoes == 0) {
        printf("Nenhuma locacao registrada.\n");
        return;
    }
    for (int i = 0; i < totalLocacoes; i++) {
        imprimirLocacao(i);
    }
}

// 8. ESTORNO: devolve o valor ao cliente, libera a estacao e remove do historico
void estornarLocacao() {
    int id;
    printf("\nDigite o ID da locacao que deseja ESTORNAR: ");
    scanf("%d", &id);

    int pos = buscarIndiceLocacao(id);
    if (pos == -1) {
        printf("[ERRO] Locacao nao encontrada!\n");
        return;
    }

    int ic = buscarIndiceCliente(historicoLocacoes[pos].idClienteLocador);
    int ie = buscarIndiceEstacao(historicoLocacoes[pos].idEstacaoUsada);
    if (ic != -1) listaClientes[ic].saldoCreditos += historicoLocacoes[pos].valorTotal;
    if (ie != -1) estacoes[ie].ocupada = 0;

    for (int j = pos; j < totalLocacoes - 1; j++) {
        historicoLocacoes[j] = historicoLocacoes[j + 1]; // puxa a fila
    }
    totalLocacoes--;

    salvarLocacoesCSV();
    salvarClientesCSV();
    salvarEstacoesCSV();
    printf("[OK] Locacao estornada: valor devolvido ao cliente e estacao liberada!\n");
}

// 9. ENCERRAR SESSAO: libera a estacao da locacao sem apagar o historico
void encerrarSessao() {
    int id;
    printf("\nDigite o ID da locacao a ENCERRAR: ");
    scanf("%d", &id);

    int pos = buscarIndiceLocacao(id);
    if (pos == -1) {
        printf("[ERRO] Locacao nao encontrada!\n");
        return;
    }

    int ie = buscarIndiceEstacao(historicoLocacoes[pos].idEstacaoUsada);
    if (ie == -1) {
        printf("[ERRO] A estacao desta locacao nao existe mais!\n");
        return;
    }
    if (!estacoes[ie].ocupada) {
        printf("A estacao '%s' ja esta livre.\n", estacoes[ie].nomeEstacao);
        return;
    }

    estacoes[ie].ocupada = 0;
    salvarEstacoesCSV();
    printf("[OK] Sessao encerrada: '%s' liberada!\n", estacoes[ie].nomeEstacao);
}

// 10. MATRIZ 2D: linhas = clientes, colunas = estacoes, celula = horas jogadas
void calcularMatrizHoras(int matriz[MAX_CLIENTES][MAX_ESTACOES]) {
    for (int c = 0; c < MAX_CLIENTES; c++) {
        for (int e = 0; e < MAX_ESTACOES; e++) {
            matriz[c][e] = 0;
        }
    }
    for (int i = 0; i < totalLocacoes; i++) {
        int c = buscarIndiceCliente(historicoLocacoes[i].idClienteLocador);
        int e = buscarIndiceEstacao(historicoLocacoes[i].idEstacaoUsada);
        if (c != -1 && e != -1) {
            matriz[c][e] += historicoLocacoes[i].horasContratadas;
        }
    }
}

// 11. RELATORIO: estacao mais utilizada (soma das colunas da matriz)
void relatorioEstacaoMaisUsada() {
    int matriz[MAX_CLIENTES][MAX_ESTACOES];
    calcularMatrizHoras(matriz);

    int melhor = -1, maiorHoras = 0;
    for (int e = 0; e < totalEstacoes; e++) {
        int soma = 0;
        for (int c = 0; c < totalClientes; c++) soma += matriz[c][e];
        if (soma > maiorHoras) {
            maiorHoras = soma;
            melhor = e;
        }
    }

    printf("\n--- [RELATORIO] ESTACAO MAIS UTILIZADA ---\n");
    if (melhor == -1) printf("Nenhuma locacao registrada.\n");
    else printf("%s (%s) com %d horas de uso no total.\n",
                estacoes[melhor].nomeEstacao, estacoes[melhor].tipo, maiorHoras);
}

// 12. RELATORIO: cliente VIP (soma das linhas da matriz)
void relatorioClienteVIP() {
    int matriz[MAX_CLIENTES][MAX_ESTACOES];
    calcularMatrizHoras(matriz);

    int melhor = -1, maiorHoras = 0;
    for (int c = 0; c < totalClientes; c++) {
        int soma = 0;
        for (int e = 0; e < totalEstacoes; e++) soma += matriz[c][e];
        if (soma > maiorHoras) {
            maiorHoras = soma;
            melhor = c;
        }
    }

    printf("\n--- [RELATORIO] CLIENTE VIP ---\n");
    if (melhor == -1) printf("Nenhuma locacao registrada.\n");
    else printf("%s (ID %d) com %d horas jogadas no total.\n",
                listaClientes[melhor].nome, listaClientes[melhor].idCliente, maiorHoras);
}

// 13. RELATORIO: locacoes de um cliente especifico (com total de horas e gasto)
void relatorioLocacoesDoCliente() {
    int id;
    printf("\nDigite o ID do cliente: ");
    scanf("%d", &id);

    int ic = buscarIndiceCliente(id);
    if (ic == -1) {
        printf("[ERRO] Cliente nao encontrado!\n");
        return;
    }

    int totalHoras = 0, quantidade = 0;
    float totalGasto = 0;

    printf("\n--- [RELATORIO] LOCACOES DE %s ---\n", listaClientes[ic].nome);
    for (int i = 0; i < totalLocacoes; i++) {
        if (historicoLocacoes[i].idClienteLocador == id) {
            imprimirLocacao(i);
            totalHoras += historicoLocacoes[i].horasContratadas;
            totalGasto += historicoLocacoes[i].valorTotal;
            quantidade++;
        }
    }

    if (quantidade == 0) printf("Este cliente ainda nao fez locacoes.\n");
    else printf("Total: %d locacao(oes) | %d horas | R$ %.2f gastos\n", quantidade, totalHoras, totalGasto);
}

// 14. [BONUS] RANKING DE CLIENTES POR HORAS COM BUBBLE SORT EM STRUCTS
void rankingClientesBubbleSort() {
    int matriz[MAX_CLIENTES][MAX_ESTACOES];
    calcularMatrizHoras(matriz);

    struct RankingCliente ranking[MAX_CLIENTES];
    for (int c = 0; c < totalClientes; c++) {
        ranking[c].idCliente = listaClientes[c].idCliente;
        strcpy(ranking[c].nome, listaClientes[c].nome);
        ranking[c].horas = 0;
        for (int e = 0; e < totalEstacoes; e++) ranking[c].horas += matriz[c][e];
    }

    // Bubble Sort decrescente: compara vizinhos e troca se o da esquerda tem menos horas
    for (int i = 0; i < totalClientes - 1; i++) {
        for (int j = 0; j < totalClientes - i - 1; j++) {
            if (ranking[j].horas < ranking[j + 1].horas) {
                struct RankingCliente aux = ranking[j];
                ranking[j] = ranking[j + 1];
                ranking[j + 1] = aux;
            }
        }
    }

    printf("\n--- [RANKING] CLIENTES POR HORAS JOGADAS ---\n");
    if (totalClientes == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }
    for (int i = 0; i < totalClientes; i++) {
        printf("%2d. %-25s | %2d horas\n", i + 1, ranking[i].nome, ranking[i].horas);
    }
}

// 15. SUBMENU (cada case chama a sua propria funcao)
void moduloLocacoesRelatorios() {
    int opcaoLocacao;
    do {
        printf("\n--- [MODULO 4: LOCACOES E RELATORIOS ANALITICOS (Integrante D)] ---\n");
        printf("1. Registrar Nova Locacao\n");
        printf("2. Listar Historico de Locacoes\n");
        printf("3. Cancelar / Estornar Locacao\n");
        printf("4. Encerrar Sessao (libera a estacao)\n");
        printf("5. [RELATORIO] Estacao Mais Utilizada\n");
        printf("6. [RELATORIO] Cliente VIP (Mais Horas Jogadas)\n");
        printf("7. [RELATORIO] Locacoes de um Cliente Especifico\n");
        printf("8. [RANKING] Clientes por Horas (Bubble Sort)\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma acao: ");
        scanf("%d", &opcaoLocacao);
        limparTela();

        switch (opcaoLocacao) {
            case 1: registrarLocacao(); break;
            case 2: listarLocacoes(); break;
            case 3: estornarLocacao(); break;
            case 4: encerrarSessao(); break;
            case 5: relatorioEstacaoMaisUsada(); break;
            case 6: relatorioClienteVIP(); break;
            case 7: relatorioLocacoesDoCliente(); break;
            case 8: rankingClientesBubbleSort(); break;
            case 0: printf("\nRetornando ao menu principal...\n"); break;
            default: printf("\n[ERRO] Opcao invalida no Modulo de Locacoes!\n");
        }
    } while (opcaoLocacao != 0);
}

#endif
