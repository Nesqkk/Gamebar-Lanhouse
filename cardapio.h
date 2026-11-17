// Arquivo: cardapio.h | Modulo 2: Cardapio (Bar e Lanchonete) | Autor: Nicolas Marques
#ifndef CARDAPIO_H
#define CARDAPIO_H

#include "utils.h"

#define MAX_ITENS 50

struct ItemCardapio {
    int idItem;
    char nomeItem[40];
    char categoria[20];     // "Bebida", "Salgado" ou "Doce"
    float preco;
    int quantidadeEstoque;  // 0 = esgotado
};

struct ItemCardapio listaItens[MAX_ITENS];
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
                listaItens[i].idItem,
                listaItens[i].nomeItem,
                listaItens[i].categoria,
                listaItens[i].preco,
                listaItens[i].quantidadeEstoque);
    }
    fclose(arq);
}

// 2. PERSISTENCIA: carrega cardapio.csv para o vetor ao abrir o programa
void carregarCardapioCSV() {
    FILE *arq = fopen("cardapio.csv", "r");
    if (arq == NULL) return; // se ainda nao existir arquivo, inicia com 0 registros

    char linha[200];
    totalItens = 0;
    while (totalItens < MAX_ITENS && fgets(linha, sizeof(linha), arq) != NULL) {
        int i = totalItens;
        int lidos = sscanf(linha, "%d;%39[^;];%19[^;];%f;%d",
                           &listaItens[i].idItem,
                           listaItens[i].nomeItem,
                           listaItens[i].categoria,
                           &listaItens[i].preco,
                           &listaItens[i].quantidadeEstoque);
        if (lidos == 5) totalItens++; // linha com defeito e ignorada
    }
    fclose(arq);
}

// 3. FUNCAO CHAVE-MESTRA: devolve a posicao do ID no vetor, ou -1 se nao existir
int buscarIndiceItem(int idProcurado) {
    for (int i = 0; i < totalItens; i++) {
        if (listaItens[i].idItem == idProcurado) return i;
    }
    return -1;
}

// 4. AUTOINCREMENT: acha o maior ID e devolve o proximo
int gerarIdItem() {
    int maior = 0;
    for (int i = 0; i < totalItens; i++) {
        if (listaItens[i].idItem > maior) maior = listaItens[i].idItem;
    }
    return maior + 1;
}

// 5. ESCOLHA DA CATEGORIA (so aceita Bebida, Salgado ou Doce)
void escolherCategoria(char destino[]) {
    int op;
    do {
        printf("Categoria (1-Bebida | 2-Salgado | 3-Doce): ");
        op = lerInteiro();
        if (op == 1) strcpy(destino, "Bebida");
        else if (op == 2) strcpy(destino, "Salgado");
        else if (op == 3) strcpy(destino, "Doce");
        else printf("[ERRO] Categoria invalida!\n");
    } while (op < 1 || op > 3);
}

// 6. CADASTRO (ID automatico + validacoes + gravacao no CSV)
void cadastrarItem() {
    if (totalItens >= MAX_ITENS) {
        printf("[ERRO] Limite de %d itens atingido!\n", MAX_ITENS);
        return;
    }

    int pos = totalItens;
    listaItens[pos].idItem = gerarIdItem();
    printf("\nNovo item (ID gerado: %d)\n", listaItens[pos].idItem);

    printf("Nome do item (com espacos): ");
    lerTexto(listaItens[pos].nomeItem, 40);

    escolherCategoria(listaItens[pos].categoria);

    do {
        printf("Preco (R$): ");
        listaItens[pos].preco = lerDecimal();
        if (listaItens[pos].preco <= 0) printf("[ERRO] O preco deve ser maior que zero!\n");
    } while (listaItens[pos].preco <= 0);

    do {
        printf("Quantidade em estoque: ");
        listaItens[pos].quantidadeEstoque = lerInteiro();
        if (listaItens[pos].quantidadeEstoque < 0) printf("[ERRO] O estoque nao pode ser negativo!\n");
    } while (listaItens[pos].quantidadeEstoque < 0);

    totalItens++;
    salvarCardapioCSV();
    printf("[OK] Item salvo com sucesso em 'cardapio.csv'!\n");
}

// 7. LISTAGEM FORMATADA
void listarCardapio() {
    printf("\n--- CARDAPIO DO GAME BAR (%d itens) ---\n", totalItens);
    if (totalItens == 0) {
        printf("Nenhum item cadastrado.\n");
        return;
    }
    for (int i = 0; i < totalItens; i++) {
        printf("ID: %-3d | %-25s | %-8s | R$ %6.2f | Estoque: %3d | %s\n",
               listaItens[i].idItem, listaItens[i].nomeItem, listaItens[i].categoria,
               listaItens[i].preco, listaItens[i].quantidadeEstoque,
               listaItens[i].quantidadeEstoque > 0 ? "Disponivel" : "Esgotado");
    }
}

// 8. BUSCA POR PARTE DO NOME (nao diferencia maiuscula de minuscula)
void buscarItemPorNome() {
    char termo[30];
    int achou = 0;

    printf("\nDigite parte do nome do item: ");
    lerTexto(termo, 30);

    for (int i = 0; i < totalItens; i++) {
        if (contemTexto(listaItens[i].nomeItem, termo)) {
            printf(" -> Encontrado: [%d] %s (%s) | R$ %.2f | Estoque: %d\n",
                   listaItens[i].idItem, listaItens[i].nomeItem, listaItens[i].categoria,
                   listaItens[i].preco, listaItens[i].quantidadeEstoque);
            achou = 1;
        }
    }
    if (!achou) printf("Nenhum item encontrado com \"%s\".\n", termo);
}

// 9. [BONUS] EDICAO NA MEMORIA + REESCRITA TOTAL DO CSV
void editarItem() {
    printf("\nDigite o ID do item que deseja EDITAR: ");
    int id = lerInteiro();

    int pos = buscarIndiceItem(id);
    if (pos == -1) {
        printf("[ERRO] Item nao encontrado!\n");
        return;
    }

    printf("Editando '%s'.\n", listaItens[pos].nomeItem);
    do {
        printf("Novo preco (R$): ");
        listaItens[pos].preco = lerDecimal();
        if (listaItens[pos].preco <= 0) printf("[ERRO] O preco deve ser maior que zero!\n");
    } while (listaItens[pos].preco <= 0);

    do {
        printf("Novo estoque: ");
        listaItens[pos].quantidadeEstoque = lerInteiro();
        if (listaItens[pos].quantidadeEstoque < 0) printf("[ERRO] O estoque nao pode ser negativo!\n");
    } while (listaItens[pos].quantidadeEstoque < 0);

    salvarCardapioCSV();
    printf("[OK] Item atualizado na memoria e no arquivo CSV!\n");
}

// 10. [BONUS] REMOCAO FISICA NA MEMORIA + REESCRITA TOTAL DO CSV
void removerItem() {
    printf("\nDigite o ID do item que deseja REMOVER: ");
    int id = lerInteiro();

    int pos = buscarIndiceItem(id);
    if (pos == -1) {
        printf("[ERRO] Item nao encontrado!\n");
        return;
    }

    for (int j = pos; j < totalItens - 1; j++) {
        listaItens[j] = listaItens[j + 1]; // puxa a fila para cobrir o item removido
    }
    totalItens--;
    salvarCardapioCSV();
    printf("[OK] Item removido do cardapio e do CSV!\n");
}

// 11. SUBMENU (cada case chama a sua propria funcao)
void moduloCardapio() {
    int opcao;
    do {
        printf("\n--- [MODULO 2: CARDAPIO BAR E LANCHONETE (Integrante B)] ---\n");
        printf("1. Cadastrar Item\n");
        printf("2. Listar Cardapio\n");
        printf("3. Buscar por Nome\n");
        printf("4. Editar Preco/Estoque\n");
        printf("5. Remover Item\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma acao: ");
        opcao = lerInteiro();
        limparTela();

        switch (opcao) {
            case 1: cadastrarItem(); break;
            case 2: listarCardapio(); break;
            case 3: buscarItemPorNome(); break;
            case 4: editarItem(); break;
            case 5: removerItem(); break;
            case 0: printf("\nRetornando ao menu principal...\n"); break;
            default: printf("\n[ERRO] Opcao invalida no Modulo de Cardapio!\n");
        }
    } while (opcao != 0);
}

#endif
