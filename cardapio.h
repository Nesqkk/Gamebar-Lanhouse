// Arquivo: cardapio.h | Modulo 2: Cardapio (Bar e Lanchonete) | Autor: Nicolas Marques
#ifndef CARDAPIO_H
#define CARDAPIO_H

#include <stdio.h>
#include <string.h>

#define MAX_ITENS 50

void limparTela(); // prototipo: a funcao completa fica no main.c da equipe

struct ItemCardapio {
    int idItem;
    char nomeItem[40];
    char categoria[20];     // "Bebida", "Salgado" ou "Doce"
    float preco;
    int quantidadeEstoque;
    int disponivel;         // 1 = tem estoque, 0 = esgotado (calculado a partir do estoque)
};

struct ItemCardapio cardapio[MAX_ITENS];
int totalItens = 0;

// 1. PERSISTENCIA: regrava o vetor inteiro em cardapio.csv (modo "w" zera o arquivo)
void salvarCardapioCSV() {
    FILE *arq = fopen("cardapio.csv", "w");
    if (arq == NULL) {
        printf("[ERRO] Nao foi possivel gravar cardapio.csv!\n");
        return;
    }
    for (int i = 0; i < totalItens; i++) {
        fprintf(arq, "%d;%s;%s;%.2f;%d\n",
                cardapio[i].idItem,
                cardapio[i].nomeItem,
                cardapio[i].categoria,
                cardapio[i].preco,
                cardapio[i].quantidadeEstoque);
    }
    fclose(arq);
}

// 2. PERSISTENCIA: carrega cardapio.csv para o vetor ao abrir o programa
void carregarCardapioCSV() {
    FILE *arq = fopen("cardapio.csv", "r");
    if (arq == NULL) return; // se ainda nao existir arquivo, inicia com 0 registros

    totalItens = 0;
    while (totalItens < MAX_ITENS &&
           fscanf(arq, "%d;%39[^;];%19[^;];%f;%d\n",
                  &cardapio[totalItens].idItem,
                  cardapio[totalItens].nomeItem,
                  cardapio[totalItens].categoria,
                  &cardapio[totalItens].preco,
                  &cardapio[totalItens].quantidadeEstoque) == 5) {
        cardapio[totalItens].disponivel = (cardapio[totalItens].quantidadeEstoque > 0);
        totalItens++;
    }
    fclose(arq);
}

// 3. FUNCAO CHAVE-MESTRA: devolve a posicao do ID no vetor, ou -1 se nao existir
int buscarIndiceItem(int idProcurado) {
    for (int i = 0; i < totalItens; i++) {
        if (cardapio[i].idItem == idProcurado) return i;
    }
    return -1;
}

// 4. AUTOINCREMENT: laco que acha o maior ID e devolve o proximo (nunca repete)
int gerarIdItem() {
    int maior = 0;
    for (int i = 0; i < totalItens; i++) {
        if (cardapio[i].idItem > maior) maior = cardapio[i].idItem;
    }
    return maior + 1;
}

// 5. PROTECAO DO CSV: troca ';' digitado pelo usuario por espaco
void trocarPontoVirgulaItem(char texto[]) {
    for (int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';') texto[i] = ' ';
    }
}

// 6. ESCOLHA DA CATEGORIA (so aceita Bebida, Salgado ou Doce)
void escolherCategoria(char destino[]) {
    int op;
    do {
        printf("Categoria (1-Bebida | 2-Salgado | 3-Doce): ");
        scanf("%d", &op);
        if (op == 1) strcpy(destino, "Bebida");
        else if (op == 2) strcpy(destino, "Salgado");
        else if (op == 3) strcpy(destino, "Doce");
        else printf("[ERRO] Categoria invalida!\n");
    } while (op < 1 || op > 3);
}

// 7. CADASTRO (ID automatico + validacoes + gravacao no CSV)
void cadastrarItem() {
    if (totalItens >= MAX_ITENS) {
        printf("[ERRO] Limite de %d itens atingido!\n", MAX_ITENS);
        return;
    }

    cardapio[totalItens].idItem = gerarIdItem();
    printf("\nNovo item (ID gerado: %d)\n", cardapio[totalItens].idItem);

    printf("Nome do item (com espacos): ");
    scanf(" %39[^\n]", cardapio[totalItens].nomeItem);
    trocarPontoVirgulaItem(cardapio[totalItens].nomeItem);

    escolherCategoria(cardapio[totalItens].categoria);

    do {
        printf("Preco (R$): ");
        scanf("%f", &cardapio[totalItens].preco);
        if (cardapio[totalItens].preco <= 0) printf("[ERRO] O preco deve ser maior que zero!\n");
    } while (cardapio[totalItens].preco <= 0);

    do {
        printf("Quantidade em estoque: ");
        scanf("%d", &cardapio[totalItens].quantidadeEstoque);
        if (cardapio[totalItens].quantidadeEstoque < 0) printf("[ERRO] O estoque nao pode ser negativo!\n");
    } while (cardapio[totalItens].quantidadeEstoque < 0);

    cardapio[totalItens].disponivel = (cardapio[totalItens].quantidadeEstoque > 0);
    totalItens++;
    salvarCardapioCSV();
    printf("[OK] Item salvo com sucesso em 'cardapio.csv'!\n");
}

// 8. LISTAGEM FORMATADA
void listarCardapio() {
    printf("\n--- CARDAPIO DO GAME BAR (%d itens) ---\n", totalItens);
    if (totalItens == 0) {
        printf("Nenhum item cadastrado.\n");
        return;
    }
    for (int i = 0; i < totalItens; i++) {
        printf("ID: %-3d | %-20s | %-8s | R$ %6.2f | Estoque: %3d | %s\n",
               cardapio[i].idItem, cardapio[i].nomeItem, cardapio[i].categoria,
               cardapio[i].preco, cardapio[i].quantidadeEstoque,
               cardapio[i].disponivel ? "Disponivel" : "Esgotado");
    }
}

// 9. BUSCA TEXTUAL COM strstr (parte do nome)
void buscarItemPorNome() {
    char termo[30];
    int achou = 0;

    printf("\nDigite parte do nome do item: ");
    scanf(" %29[^\n]", termo);

    for (int i = 0; i < totalItens; i++) {
        if (strstr(cardapio[i].nomeItem, termo) != NULL) {
            printf(" -> Encontrado: [%d] %s (%s) | R$ %.2f | Estoque: %d\n",
                   cardapio[i].idItem, cardapio[i].nomeItem, cardapio[i].categoria,
                   cardapio[i].preco, cardapio[i].quantidadeEstoque);
            achou = 1;
        }
    }
    if (!achou) printf("Nenhum item encontrado com \"%s\".\n", termo);
}

// 10. [BONUS] ALTERAR PRECO E ESTOQUE NA MEMORIA + REESCRITA TOTAL DO CSV
void alterarItem() {
    int id;
    printf("\nDigite o ID do item que deseja ALTERAR: ");
    scanf("%d", &id);

    int pos = buscarIndiceItem(id);
    if (pos == -1) {
        printf("[ERRO] Item nao encontrado!\n");
        return;
    }

    printf("Alterando '%s'.\n", cardapio[pos].nomeItem);
    do {
        printf("Novo preco (R$): ");
        scanf("%f", &cardapio[pos].preco);
        if (cardapio[pos].preco <= 0) printf("[ERRO] O preco deve ser maior que zero!\n");
    } while (cardapio[pos].preco <= 0);

    do {
        printf("Novo estoque: ");
        scanf("%d", &cardapio[pos].quantidadeEstoque);
        if (cardapio[pos].quantidadeEstoque < 0) printf("[ERRO] O estoque nao pode ser negativo!\n");
    } while (cardapio[pos].quantidadeEstoque < 0);

    cardapio[pos].disponivel = (cardapio[pos].quantidadeEstoque > 0);
    salvarCardapioCSV();
    printf("[OK] Item atualizado na memoria e no arquivo CSV!\n");
}

// 11. [BONUS] REMOCAO FISICA NA MEMORIA + REESCRITA TOTAL DO CSV
void excluirItem() {
    int id;
    printf("\nDigite o ID do item que deseja EXCLUIR: ");
    scanf("%d", &id);

    int pos = buscarIndiceItem(id);
    if (pos == -1) {
        printf("[ERRO] Item nao encontrado!\n");
        return;
    }

    for (int j = pos; j < totalItens - 1; j++) {
        cardapio[j] = cardapio[j + 1]; // puxa a fila para cobrir o item removido
    }
    totalItens--;
    salvarCardapioCSV();
    printf("[OK] Item removido do cardapio e do CSV!\n");
}

// 12. SUBMENU (cada case chama a sua propria funcao)
void moduloCardapio() {
    int opcaoItem;
    do {
        printf("\n--- [MODULO 2: CARDAPIO BAR E LANCHONETE (Integrante B)] ---\n");
        printf("1. Cadastrar | 2. Listar | 3. Buscar por Nome | 4. Alterar Preco/Estoque | 5. Excluir | 0. Voltar: ");
        scanf("%d", &opcaoItem);
        limparTela();

        switch (opcaoItem) {
            case 1: cadastrarItem(); break;
            case 2: listarCardapio(); break;
            case 3: buscarItemPorNome(); break;
            case 4: alterarItem(); break;
            case 5: excluirItem(); break;
            case 0: printf("\nRetornando ao menu principal...\n"); break;
            default: printf("\n[ERRO] Opcao invalida no Modulo de Cardapio!\n");
        }
    } while (opcaoItem != 0);
}

#endif
