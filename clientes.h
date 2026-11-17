// Arquivo: clientes.h | Modulo 1: Gestao de Clientes | Autor: Ruan Pablo Dos Santos Da Silva 49723421
#ifndef CLIENTES_H
#define CLIENTES_H

#include "utils.h"

#define MAX_CLIENTES 50

int clienteTemLocacoes(int idCliente); // prototipo: a funcao completa fica no locacoes.h

struct Cliente {
    int idCliente;
    char nome[50];
    char telefone[20];
    float saldoCreditos;   // creditos pre-pagos em R$
};

struct Cliente listaClientes[MAX_CLIENTES];
int totalClientes = 0;

// 1. PERSISTENCIA: regrava o vetor inteiro em clientes.csv (modo "w" zera o arquivo)
void salvarClientesCSV() {
    FILE *arq = fopen("clientes.csv", "w");
    if (arq == NULL) {
        printf("[ERRO] Nao foi possivel gravar clientes.csv!\n");
        return;
    }
    for (int i = 0; i < totalClientes; i++) {
        fprintf(arq, "%d;%s;%s;%.2f\n",
                listaClientes[i].idCliente,
                listaClientes[i].nome,
                listaClientes[i].telefone,
                listaClientes[i].saldoCreditos);
    }
    fclose(arq);
}

// 2. PERSISTENCIA: carrega clientes.csv para o vetor ao abrir o programa
void carregarClientesCSV() {
    FILE *arq = fopen("clientes.csv", "r");
    if (arq == NULL) return; // se ainda nao existir arquivo, inicia com 0 registros

    char linha[200];
    totalClientes = 0;
    while (totalClientes < MAX_CLIENTES && fgets(linha, sizeof(linha), arq) != NULL) {
        int i = totalClientes;
        int lidos = sscanf(linha, "%d;%49[^;];%19[^;];%f",
                           &listaClientes[i].idCliente,
                           listaClientes[i].nome,
                           listaClientes[i].telefone,
                           &listaClientes[i].saldoCreditos);
        if (lidos == 4) totalClientes++; // linha com defeito e ignorada
    }
    fclose(arq);
}

// 3. FUNCAO CHAVE-MESTRA: devolve a posicao do ID no vetor, ou -1 se nao existir
int buscarIndiceCliente(int idProcurado) {
    for (int i = 0; i < totalClientes; i++) {
        if (listaClientes[i].idCliente == idProcurado) return i;
    }
    return -1;
}

// 4. AUTOINCREMENT: acha o maior ID e devolve o proximo
int gerarIdCliente() {
    int maior = 0;
    for (int i = 0; i < totalClientes; i++) {
        if (listaClientes[i].idCliente > maior) maior = listaClientes[i].idCliente;
    }
    return maior + 1;
}

// 5. CADASTRO (ID automatico + validacoes + gravacao no CSV)
void cadastrarCliente() {
    if (totalClientes >= MAX_CLIENTES) {
        printf("[ERRO] Limite de %d clientes atingido!\n", MAX_CLIENTES);
        return;
    }

    int pos = totalClientes;
    listaClientes[pos].idCliente = gerarIdCliente();
    printf("\nNovo cliente (ID gerado: %d)\n", listaClientes[pos].idCliente);

    printf("Nome (com espacos): ");
    lerTexto(listaClientes[pos].nome, 50);

    printf("Telefone: ");
    lerTexto(listaClientes[pos].telefone, 20);

    do {
        printf("Saldo inicial de creditos (R$): ");
        listaClientes[pos].saldoCreditos = lerDecimal();
        if (listaClientes[pos].saldoCreditos < 0) printf("[ERRO] O saldo nao pode ser negativo!\n");
    } while (listaClientes[pos].saldoCreditos < 0);

    totalClientes++;
    salvarClientesCSV();
    printf("[OK] Cliente salvo com sucesso em 'clientes.csv'!\n");
}

// 6. LISTAGEM FORMATADA
void listarClientes() {
    printf("\n--- CLIENTES DO GAME BAR (%d cadastrados) ---\n", totalClientes);
    if (totalClientes == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }
    for (int i = 0; i < totalClientes; i++) {
        printf("ID: %-3d | %-25s | Tel: %-16s | Saldo: R$ %.2f\n",
               listaClientes[i].idCliente, listaClientes[i].nome,
               listaClientes[i].telefone, listaClientes[i].saldoCreditos);
    }
}

// 7. BUSCA POR PARTE DO NOME (nao diferencia maiuscula de minuscula)
void buscarClientePorNome() {
    char termo[30];
    int achou = 0;

    printf("\nDigite parte do nome do cliente: ");
    lerTexto(termo, 30);

    for (int i = 0; i < totalClientes; i++) {
        if (contemTexto(listaClientes[i].nome, termo)) {
            printf(" -> Encontrado: [%d] %s | Tel: %s | Saldo: R$ %.2f\n",
                   listaClientes[i].idCliente, listaClientes[i].nome,
                   listaClientes[i].telefone, listaClientes[i].saldoCreditos);
            achou = 1;
        }
    }
    if (!achou) printf("Nenhum cliente encontrado com \"%s\".\n", termo);
}

// 8. [BONUS] EDICAO NA MEMORIA + REESCRITA TOTAL DO CSV
void editarCliente() {
    printf("\nDigite o ID do cliente que deseja EDITAR: ");
    int id = lerInteiro();

    int pos = buscarIndiceCliente(id);
    if (pos == -1) {
        printf("[ERRO] Cliente nao encontrado!\n");
        return;
    }

    printf("Editando '%s'.\n", listaClientes[pos].nome);
    printf("Novo nome: ");
    lerTexto(listaClientes[pos].nome, 50);

    printf("Novo telefone: ");
    lerTexto(listaClientes[pos].telefone, 20);

    do {
        printf("Novo saldo de creditos (R$): ");
        listaClientes[pos].saldoCreditos = lerDecimal();
        if (listaClientes[pos].saldoCreditos < 0) printf("[ERRO] O saldo nao pode ser negativo!\n");
    } while (listaClientes[pos].saldoCreditos < 0);

    salvarClientesCSV();
    printf("[OK] Cliente atualizado na memoria e no arquivo CSV!\n");
}

// 9. [BONUS] REMOCAO FISICA NA MEMORIA + REESCRITA TOTAL DO CSV
void removerCliente() {
    printf("\nDigite o ID do cliente que deseja REMOVER: ");
    int id = lerInteiro();

    int pos = buscarIndiceCliente(id);
    if (pos == -1) {
        printf("[ERRO] Cliente nao encontrado!\n");
        return;
    }
    if (clienteTemLocacoes(id)) {
        printf("[ERRO] Este cliente tem locacoes no historico! Estorne as locacoes antes de remover.\n");
        return;
    }

    for (int j = pos; j < totalClientes - 1; j++) {
        listaClientes[j] = listaClientes[j + 1]; // puxa a fila para cobrir o item removido
    }
    totalClientes--;
    salvarClientesCSV();
    printf("[OK] Cliente removido do sistema e do CSV!\n");
}

// 10. SUBMENU (cada case chama a sua propria funcao)
void moduloClientes() {
    int opcao;
    do {
        printf("\n--- [MODULO 1: GESTAO DE CLIENTES (Integrante A)] ---\n");
        printf("1. Cadastrar Cliente\n");
        printf("2. Listar Clientes\n");
        printf("3. Buscar por Nome\n");
        printf("4. Editar Cliente\n");
        printf("5. Remover Cliente\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma acao: ");
        opcao = lerInteiro();
        limparTela();

        switch (opcao) {
            case 1: cadastrarCliente(); break;
            case 2: listarClientes(); break;
            case 3: buscarClientePorNome(); break;
            case 4: editarCliente(); break;
            case 5: removerCliente(); break;
            case 0: printf("\nRetornando ao menu principal...\n"); break;
            default: printf("\n[ERRO] Opcao invalida no Modulo de Clientes!\n");
        }
    } while (opcao != 0);
}

#endif
