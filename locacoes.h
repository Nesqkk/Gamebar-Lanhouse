// Arquivo: locacoes.h | Modulo 4: Locacoes e Relatorios | Autor: Joao Pedro Porto Lopes 49004913
#ifndef LOCACOES_H
#define LOCACOES_H

#include "utils.h"
#include "clientes.h"   // para validar o cliente e debitar o saldo
#include "estacoes.h"   // para validar a estacao e mudar o status

#define MAX_LOCACOES 500

struct Locacao {
    int idLocacao;
    int idClienteLocador;
    int idEstacaoUsada;
    int horasContratadas;
    float valorTotal;      // horasContratadas * precoHora
    int emAndamento;       // 1 = sessao em andamento, 0 = sessao encerrada
};

struct Locacao listaLocacoes[MAX_LOCACOES];
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
        fprintf(arq, "%d;%d;%d;%d;%.2f;%d\n",
                listaLocacoes[i].idLocacao,
                listaLocacoes[i].idClienteLocador,
                listaLocacoes[i].idEstacaoUsada,
                listaLocacoes[i].horasContratadas,
                listaLocacoes[i].valorTotal,
                listaLocacoes[i].emAndamento);
    }
    fclose(arq);
}

// 2. PERSISTENCIA: carrega locacoes.csv para o vetor ao abrir o programa
void carregarLocacoesCSV() {
    FILE *arq = fopen("locacoes.csv", "r");
    if (arq == NULL) return; // se ainda nao existir arquivo, inicia com 0 registros

    char linha[200];
    totalLocacoes = 0;
    while (totalLocacoes < MAX_LOCACOES && fgets(linha, sizeof(linha), arq) != NULL) {
        int i = totalLocacoes;
        int lidos = sscanf(linha, "%d;%d;%d;%d;%f;%d",
                           &listaLocacoes[i].idLocacao,
                           &listaLocacoes[i].idClienteLocador,
                           &listaLocacoes[i].idEstacaoUsada,
                           &listaLocacoes[i].horasContratadas,
                           &listaLocacoes[i].valorTotal,
                           &listaLocacoes[i].emAndamento);
        if (lidos == 5) listaLocacoes[i].emAndamento = 0; // arquivo antigo (sem a 6a coluna)
        if (lidos >= 5) totalLocacoes++;                  // linha com defeito e ignorada
    }
    fclose(arq);
}

// 3. FUNCAO CHAVE-MESTRA: devolve a posicao do ID no vetor, ou -1 se nao existir
int buscarIndiceLocacao(int idProcurado) {
    for (int i = 0; i < totalLocacoes; i++) {
        if (listaLocacoes[i].idLocacao == idProcurado) return i;
    }
    return -1;
}

// 4. AUTOINCREMENT: acha o maior ID e devolve o proximo
int gerarIdLocacao() {
    int maior = 0;
    for (int i = 0; i < totalLocacoes; i++) {
        if (listaLocacoes[i].idLocacao > maior) maior = listaLocacoes[i].idLocacao;
    }
    return maior + 1;
}

// 5. FUNCOES USADAS PELOS MODULOS 1 E 3 (impedem remover quem tem historico)
int clienteTemLocacoes(int idCliente) {
    for (int i = 0; i < totalLocacoes; i++) {
        if (listaLocacoes[i].idClienteLocador == idCliente) return 1;
    }
    return 0;
}

int estacaoTemLocacoes(int idEstacao) {
    for (int i = 0; i < totalLocacoes; i++) {
        if (listaLocacoes[i].idEstacaoUsada == idEstacao) return 1;
    }
    return 0;
}

// 6. IMPRIME UMA LOCACAO COM O NOME DO CLIENTE E DA ESTACAO
void imprimirLocacao(int i) {
    int ic = buscarIndiceCliente(listaLocacoes[i].idClienteLocador);
    int ie = buscarIndiceEstacao(listaLocacoes[i].idEstacaoUsada);
    printf("Loc %-3d | Cliente: %-22s | Estacao: %-22s | %2dh | R$ %.2f | %s\n",
           listaLocacoes[i].idLocacao,
           ic != -1 ? listaClientes[ic].nome : "(removido)",
           ie != -1 ? listaEstacoes[ie].nomeEstacao : "(removida)",
           listaLocacoes[i].horasContratadas,
           listaLocacoes[i].valorTotal,
           listaLocacoes[i].emAndamento ? "Em andamento" : "Encerrada");
}

// 7. REGRA DE NEGOCIO: valida cliente e estacao, calcula o valor,
//    debita o saldo do cliente e marca a estacao como ocupada
void registrarLocacao() {
    if (totalLocacoes >= MAX_LOCACOES) {
        printf("[ERRO] Limite de %d locacoes atingido!\n", MAX_LOCACOES);
        return;
    }

    printf("\nID do cliente: ");
    int idCliente = lerInteiro();
    int ic = buscarIndiceCliente(idCliente);
    if (ic == -1) {
        printf("[ERRO] Cliente nao encontrado!\n");
        return;
    }

    printf("ID da estacao: ");
    int idEstacao = lerInteiro();
    int ie = buscarIndiceEstacao(idEstacao);
    if (ie == -1) {
        printf("[ERRO] Estacao nao encontrada!\n");
        return;
    }
    if (listaEstacoes[ie].ocupada) {
        printf("[ERRO] A estacao '%s' ja esta em uso!\n", listaEstacoes[ie].nomeEstacao);
        return;
    }

    int horas;
    do {
        printf("Horas contratadas: ");
        horas = lerInteiro();
        if (horas <= 0) printf("[ERRO] Informe pelo menos 1 hora!\n");
    } while (horas <= 0);

    float valor = horas * listaEstacoes[ie].precoHora;
    if (listaClientes[ic].saldoCreditos < valor) {
        printf("[ERRO] Saldo insuficiente! Saldo: R$ %.2f | Necessario: R$ %.2f\n",
               listaClientes[ic].saldoCreditos, valor);
        return;
    }

    listaClientes[ic].saldoCreditos -= valor;   // abate os creditos do cliente
    listaEstacoes[ie].ocupada = 1;              // muda o status da estacao

    int pos = totalLocacoes;
    listaLocacoes[pos].idLocacao = gerarIdLocacao();
    listaLocacoes[pos].idClienteLocador = idCliente;
    listaLocacoes[pos].idEstacaoUsada = idEstacao;
    listaLocacoes[pos].horasContratadas = horas;
    listaLocacoes[pos].valorTotal = valor;
    listaLocacoes[pos].emAndamento = 1;
    totalLocacoes++;

    salvarLocacoesCSV();
    salvarClientesCSV();
    salvarEstacoesCSV();

    printf("[OK] Locacao registrada: %s em '%s' por %dh = R$ %.2f (saldo restante: R$ %.2f)\n",
           listaClientes[ic].nome, listaEstacoes[ie].nomeEstacao, horas, valor,
           listaClientes[ic].saldoCreditos);
}

// 8. LISTAGEM DO HISTORICO
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

// 9. ESTORNO: devolve o valor ao cliente, libera a estacao (se a sessao ainda
//    estiver em andamento) e remove do historico
void estornarLocacao() {
    printf("\nDigite o ID da locacao que deseja ESTORNAR: ");
    int id = lerInteiro();

    int pos = buscarIndiceLocacao(id);
    if (pos == -1) {
        printf("[ERRO] Locacao nao encontrada!\n");
        return;
    }

    int ic = buscarIndiceCliente(listaLocacoes[pos].idClienteLocador);
    int ie = buscarIndiceEstacao(listaLocacoes[pos].idEstacaoUsada);
    if (ic != -1) listaClientes[ic].saldoCreditos += listaLocacoes[pos].valorTotal;
    if (ie != -1 && listaLocacoes[pos].emAndamento) listaEstacoes[ie].ocupada = 0;

    for (int j = pos; j < totalLocacoes - 1; j++) {
        listaLocacoes[j] = listaLocacoes[j + 1]; // puxa a fila para cobrir o item removido
    }
    totalLocacoes--;

    salvarLocacoesCSV();
    salvarClientesCSV();
    salvarEstacoesCSV();
    printf("[OK] Locacao estornada: valor devolvido ao cliente!\n");
}

// 10. ENCERRAR SESSAO: libera a estacao da locacao sem apagar o historico
void encerrarSessao() {
    printf("\nDigite o ID da locacao a ENCERRAR: ");
    int id = lerInteiro();

    int pos = buscarIndiceLocacao(id);
    if (pos == -1) {
        printf("[ERRO] Locacao nao encontrada!\n");
        return;
    }
    if (!listaLocacoes[pos].emAndamento) {
        printf("Esta sessao ja foi encerrada.\n");
        return;
    }

    int ie = buscarIndiceEstacao(listaLocacoes[pos].idEstacaoUsada);
    if (ie != -1) listaEstacoes[ie].ocupada = 0;
    listaLocacoes[pos].emAndamento = 0;

    salvarLocacoesCSV();
    salvarEstacoesCSV();
    printf("[OK] Sessao encerrada e estacao liberada!\n");
}

// 11. MATRIZ 2D: linhas = clientes, colunas = estacoes, celula = horas jogadas
void calcularMatrizHoras(int matriz[MAX_CLIENTES][MAX_ESTACOES]) {
    for (int c = 0; c < MAX_CLIENTES; c++) {
        for (int e = 0; e < MAX_ESTACOES; e++) {
            matriz[c][e] = 0;
        }
    }
    for (int i = 0; i < totalLocacoes; i++) {
        int c = buscarIndiceCliente(listaLocacoes[i].idClienteLocador);
        int e = buscarIndiceEstacao(listaLocacoes[i].idEstacaoUsada);
        if (c != -1 && e != -1) {
            matriz[c][e] += listaLocacoes[i].horasContratadas;
        }
    }
}

// 12. RELATORIO: estacao mais utilizada (soma das colunas da matriz)
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
                listaEstacoes[melhor].nomeEstacao, listaEstacoes[melhor].tipo, maiorHoras);
}

// 13. RELATORIO: cliente VIP (soma das linhas da matriz)
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

// 14. RELATORIO: locacoes de um cliente especifico (com total de horas e gasto)
void relatorioLocacoesDoCliente() {
    printf("\nDigite o ID do cliente: ");
    int id = lerInteiro();

    int ic = buscarIndiceCliente(id);
    if (ic == -1) {
        printf("[ERRO] Cliente nao encontrado!\n");
        return;
    }

    int totalHoras = 0, quantidade = 0;
    float totalGasto = 0;

    printf("\n--- [RELATORIO] LOCACOES DE %s ---\n", listaClientes[ic].nome);
    for (int i = 0; i < totalLocacoes; i++) {
        if (listaLocacoes[i].idClienteLocador == id) {
            imprimirLocacao(i);
            totalHoras += listaLocacoes[i].horasContratadas;
            totalGasto += listaLocacoes[i].valorTotal;
            quantidade++;
        }
    }

    if (quantidade == 0) printf("Este cliente ainda nao fez locacoes.\n");
    else printf("Total: %d locacao(oes) | %d horas | R$ %.2f gastos\n", quantidade, totalHoras, totalGasto);
}

// 15. [BONUS] RANKING DE CLIENTES POR HORAS COM BUBBLE SORT EM STRUCTS
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

// 16. SUBMENU (cada case chama a sua propria funcao)
void moduloLocacoesRelatorios() {
    int opcao;
    do {
        printf("\n--- [MODULO 4: LOCACOES E RELATORIOS (Integrante D)] ---\n");
        printf("1. Registrar Nova Locacao\n");
        printf("2. Listar Historico de Locacoes\n");
        printf("3. Estornar Locacao\n");
        printf("4. Encerrar Sessao (libera a estacao)\n");
        printf("5. [RELATORIO] Estacao Mais Utilizada\n");
        printf("6. [RELATORIO] Cliente VIP (Mais Horas Jogadas)\n");
        printf("7. [RELATORIO] Locacoes de um Cliente\n");
        printf("8. [RANKING] Clientes por Horas (Bubble Sort)\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma acao: ");
        opcao = lerInteiro();
        limparTela();

        switch (opcao) {
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
    } while (opcao != 0);
}

#endif
