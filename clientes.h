// Arquivo: clientes.h | Modulo 1: Gestao de Clientes | Autor: Ruan Pablo Dos Santos Da Silva 49723421
#ifndef CLIENTES_H
#define CLIENTES_H

#include <stdio.h>
#include <string.h>

#define MAX_CLIENTES 50

void limparTela(); // prototipo: a funcao completa fica no main.c da equipe

struct Cliente {
    int idCliente;
    char nome[50];
    char telefone[20];
    float saldoCreditos;   // creditos pre-pagos em R$
    int ativo;             // 1 = Ativo (a remocao e fisica, entao fica sempre 1)
};

struct Cliente listaClientes[MAX_CLIENTES];
int totalClientes = 0;

// ---------------------------------------------------------------------------
// 1. PERSISTENCIA: regrava o vetor inteiro em clientes.csv (modo "w" zera o arquivo)
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// 2. PERSISTENCIA: carrega clientes.csv para o vetor ao abrir o programa
// ---------------------------------------------------------------------------
void carregarClientesCSV() {
    FILE *arq = fopen("clientes.csv", "r");
    if (arq == NULL) return; // se ainda nao existir arquivo, inicia com 0 registros

    totalClientes = 0;
    while (totalClientes < MAX_CLIENTES &&
           fscanf(arq, "%d;%49[^;];%19[^;];%f\n",
                  &listaClientes[totalClientes].idCliente,
                  listaClientes[totalClientes].nome,
                  listaClientes[totalClientes].telefone,
                  &listaClientes[totalClientes].saldoCreditos) == 4) {
        listaClientes[totalClientes].ativo = 1;
        totalClientes++;
    }
    fclose(arq);
}

// ---------------------------------------------------------------------------
// 3. FUNCAO CHAVE-MESTRA: devolve a posicao do ID no vetor, ou -1 se nao existir
//    (o Modulo 4 usa esta funcao para validar o cliente na locacao)
// ---------------------------------------------------------------------------
int buscarIndiceCliente(int idProcurado) {
    for (int i = 0; i < totalClientes; i++) {
        if (listaClientes[i].idCliente == idProcurado) return i;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// 4. AUTOINCREMENT: laco que acha o maior ID e devolve o proximo (nunca repete)
// ---------------------------------------------------------------------------
int gerarIdCliente() {
    int maior = 0;
    for (int i = 0; i < totalClientes; i++) {
        if (listaClientes[i].idCliente > maior) maior = listaClientes[i].idCliente;
    }
    return maior + 1;
}

// ---------------------------------------------------------------------------
// 5. PROTECAO DO CSV: troca ';' digitado pelo usuario por espaco
// ---------------------------------------------------------------------------
void trocarPontoVirgula(char texto[]) {
    for (int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';') texto[i] = ' ';
    }
}

// ---------------------------------------------------------------------------
// 6. CADASTRO (ID automatico + validacao do saldo + gravacao no CSV)
// ---------------------------------------------------------------------------
void cadastrarCliente() {
    if (totalClientes >= MAX_CLIENTES) {
        printf("[ERRO] Limite de %d clientes atingido!\n", MAX_CLIENTES);
        return;
    }

    listaClientes[totalClientes].idCliente = gerarIdCliente();
    printf("\nNovo cliente (ID gerado: %d)\n", listaClientes[totalClientes].idCliente);

    printf("Nome (com espacos): ");
    scanf(" %49[^\n]", listaClientes[totalClientes].nome);
    trocarPontoVirgula(listaClientes[totalClientes].nome);

    printf("Telefone: ");
    scanf(" %19[^\n]", listaClientes[totalClientes].telefone);
    trocarPontoVirgula(listaClientes[totalClientes].telefone);

    do {
        printf("Saldo inicial de creditos (R$): ");
        scanf("%f", &listaClientes[totalClientes].saldoCreditos);
        if (listaClientes[totalClientes].saldoCreditos < 0) {
            printf("[ERRO] O saldo nao pode ser negativo!\n");
        }
    } while (listaClientes[totalClientes].saldoCreditos < 0);

    listaClientes[totalClientes].ativo = 1;
    totalClientes++;
    salvarClientesCSV();
    printf("[OK] Cliente salvo com sucesso em 'clientes.csv'!\n");
}

// ---------------------------------------------------------------------------
// 7. LISTAGEM FORMATADA
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// 8. BUSCA TEXTUAL COM strstr (parte do nome)
// ---------------------------------------------------------------------------
void buscarClientePorNome() {
    char termo[30];
    int achou = 0;

    printf("\nDigite parte do nome para buscar: ");
    scanf(" %29[^\n]", termo);

    for (int i = 0; i < totalClientes; i++) {
        if (strstr(listaClientes[i].nome, termo) != NULL) {
            printf(" -> Encontrado: [%d] %s | Tel: %s | Saldo: R$ %.2f\n",
                   listaClientes[i].idCliente, listaClientes[i].nome,
                   listaClientes[i].telefone, listaClientes[i].saldoCreditos);
            achou = 1;
        }
    }
    if (!achou) printf("Nenhum cliente encontrado com \"%s\".\n", termo);
}

// ---------------------------------------------------------------------------
// 9. [BONUS] EDICAO NA MEMORIA + REESCRITA TOTAL DO CSV
// ---------------------------------------------------------------------------
void editarCliente() {
    int id;
    printf("\nDigite o ID do cliente que deseja EDITAR: ");
    scanf("%d", &id);

    int pos = buscarIndiceCliente(id);
    if (pos == -1) {
        printf("[ERRO] Cliente nao encontrado!\n");
        return;
    }

    printf("Editando '%s'.\n", listaClientes[pos].nome);
    printf("Novo nome: ");
    scanf(" %49[^\n]", listaClientes[pos].nome);
    trocarPontoVirgula(listaClientes[pos].nome);

    printf("Novo telefone: ");
    scanf(" %19[^\n]", listaClientes[pos].telefone);
    trocarPontoVirgula(listaClientes[pos].telefone);

    do {
        printf("Novo saldo de creditos (R$): ");
        scanf("%f", &listaClientes[pos].saldoCreditos);
        if (listaClientes[pos].saldoCreditos < 0) {
            printf("[ERRO] O saldo nao pode ser negativo!\n");
        }
    } while (listaClientes[pos].saldoCreditos < 0);

    salvarClientesCSV();
    printf("[OK] Cliente atualizado na memoria e no arquivo CSV!\n");
}

// ---------------------------------------------------------------------------
// 10. [BONUS] REMOCAO FISICA NA MEMORIA + REESCRITA TOTAL DO CSV
// ---------------------------------------------------------------------------
void removerCliente() {
    int id;
    printf("\nDigite o ID do cliente que deseja REMOVER: ");
    scanf("%d", &id);

    int pos = buscarIndiceCliente(id);
    if (pos == -1) {
        printf("[ERRO] Cliente nao encontrado!\n");
        return;
    }

    for (int j = pos; j < totalClientes - 1; j++) {
        listaClientes[j] = listaClientes[j + 1]; // puxa a fila para cobrir o item removido
    }
    totalClientes--;
    salvarClientesCSV();
    printf("[OK] Cliente removido do sistema e do CSV!\n");
}

// ---------------------------------------------------------------------------
// 11. SUBMENU (cada case chama a sua propria funcao)
// ---------------------------------------------------------------------------
void moduloClientes() {
    int opcaoCliente;
    do {
        printf("\n--- [MODULO 1: GESTAO DE CLIENTES (Integrante A)] ---\n");
        printf("1. Cadastrar | 2. Listar | 3. Buscar por Nome | 4. Editar | 5. Remover | 0. Voltar: ");
        scanf("%d", &opcaoCliente);
        limparTela();

        switch (opcaoCliente) {
            case 1: cadastrarCliente(); break;
            case 2: listarClientes(); break;
            case 3: buscarClientePorNome(); break;
            case 4: editarCliente(); break;
            case 5: removerCliente(); break;
            case 0: printf("\nRetornando ao menu principal...\n"); break;
            default: printf("\n[ERRO] Opcao invalida no Modulo de Clientes!\n");
        }
    } while (opcaoCliente != 0);
}

#endif
