// Arquivo: estacoes.h | Modulo 3: Estacoes de Jogo | Autor: Joao Paulo Rocha Trindade 49222767
#ifndef ESTACOES_H
#define ESTACOES_H

#include "utils.h"

#define MAX_ESTACOES 50
#define PRECO_HORA 1.00  // valor da hora de uso de qualquer estacao (R$)

int estacaoTemLocacoes(int idEstacao); // prototipo: a funcao completa fica no locacoes.h

struct Estacao {
    int idEstacao;
    char nomeEstacao[30];  // Ex: "PC Gamer 01", "PS5 Mesa 2"
    char tipo[20];         // "Computador", "Console", "Celular" ou "VR"
    float precoHora;       // usa PRECO_HORA (R$ 1,00)
    int ocupada;           // 1 = Em uso, 0 = Livre
};

struct Estacao listaEstacoes[MAX_ESTACOES];
int totalEstacoes = 0;

// 1. PERSISTENCIA: regrava o vetor inteiro em estacoes.csv (modo "w" zera o arquivo)
void salvarEstacoesCSV() {
    FILE *arq = fopen("estacoes.csv", "w");
    if (arq == NULL) {
        printf("[ERRO] Nao foi possivel gravar estacoes.csv!\n");
        return;
    }
    for (int i = 0; i < totalEstacoes; i++) {
        fprintf(arq, "%d;%s;%s;%.2f;%d\n",
                listaEstacoes[i].idEstacao,
                listaEstacoes[i].nomeEstacao,
                listaEstacoes[i].tipo,
                listaEstacoes[i].precoHora,
                listaEstacoes[i].ocupada);
    }
    fclose(arq);
}

// 2. PERSISTENCIA: carrega estacoes.csv para o vetor ao abrir o programa
void carregarEstacoesCSV() {
    FILE *arq = fopen("estacoes.csv", "r");
    if (arq == NULL) return; // se ainda nao existir arquivo, inicia com 0 registros

    char linha[200];
    totalEstacoes = 0;
    while (totalEstacoes < MAX_ESTACOES && fgets(linha, sizeof(linha), arq) != NULL) {
        int i = totalEstacoes;
        int lidos = sscanf(linha, "%d;%29[^;];%19[^;];%f;%d",
                           &listaEstacoes[i].idEstacao,
                           listaEstacoes[i].nomeEstacao,
                           listaEstacoes[i].tipo,
                           &listaEstacoes[i].precoHora,
                           &listaEstacoes[i].ocupada);
        if (lidos == 5) totalEstacoes++; // linha com defeito e ignorada
    }
    fclose(arq);
}

// 3. FUNCAO CHAVE-MESTRA: devolve a posicao do ID no vetor, ou -1 se nao existir
int buscarIndiceEstacao(int idProcurado) {
    for (int i = 0; i < totalEstacoes; i++) {
        if (listaEstacoes[i].idEstacao == idProcurado) return i;
    }
    return -1;
}

// 4. AUTOINCREMENT: acha o maior ID e devolve o proximo
int gerarIdEstacao() {
    int maior = 0;
    for (int i = 0; i < totalEstacoes; i++) {
        if (listaEstacoes[i].idEstacao > maior) maior = listaEstacoes[i].idEstacao;
    }
    return maior + 1;
}

// 5. ESCOLHA DO TIPO (so aceita Computador, Console, Celular ou VR)
void escolherTipoEstacao(char destino[]) {
    int op;
    do {
        printf("Tipo (1-Computador | 2-Console | 3-Celular | 4-VR): ");
        op = lerInteiro();
        if (op == 1) strcpy(destino, "Computador");
        else if (op == 2) strcpy(destino, "Console");
        else if (op == 3) strcpy(destino, "Celular");
        else if (op == 4) strcpy(destino, "VR");
        else printf("[ERRO] Tipo invalido!\n");
    } while (op < 1 || op > 4);
}

// 6. CADASTRO (ID automatico + preco fixo PRECO_HORA + gravacao no CSV)
void cadastrarEstacao() {
    if (totalEstacoes >= MAX_ESTACOES) {
        printf("[ERRO] Limite de %d estacoes atingido!\n", MAX_ESTACOES);
        return;
    }

    int pos = totalEstacoes;
    listaEstacoes[pos].idEstacao = gerarIdEstacao();
    printf("\nNova estacao (ID gerado: %d)\n", listaEstacoes[pos].idEstacao);

    printf("Nome da estacao (ex: PC Gamer 05): ");
    lerTexto(listaEstacoes[pos].nomeEstacao, 30);

    escolherTipoEstacao(listaEstacoes[pos].tipo);

    listaEstacoes[pos].precoHora = PRECO_HORA;
    listaEstacoes[pos].ocupada = 0;
    totalEstacoes++;
    salvarEstacoesCSV();
    printf("[OK] Estacao salva em 'estacoes.csv' (hora a R$ %.2f)!\n", PRECO_HORA);
}

// 7. LISTAGEM FORMATADA (mostra se esta livre ou em uso)
void listarEstacoes() {
    printf("\n--- ESTACOES DE JOGO (%d cadastradas) ---\n", totalEstacoes);
    if (totalEstacoes == 0) {
        printf("Nenhuma estacao cadastrada.\n");
        return;
    }
    for (int i = 0; i < totalEstacoes; i++) {
        printf("ID: %-3d | %-24s | %-10s | R$ %.2f/h | %s\n",
               listaEstacoes[i].idEstacao, listaEstacoes[i].nomeEstacao, listaEstacoes[i].tipo,
               listaEstacoes[i].precoHora, listaEstacoes[i].ocupada ? "EM USO" : "Livre");
    }
}

// 8. BUSCA POR PARTE DO NOME (nao diferencia maiuscula de minuscula)
void buscarEstacaoPorNome() {
    char termo[30];
    int achou = 0;

    printf("\nDigite parte do nome da estacao: ");
    lerTexto(termo, 30);

    for (int i = 0; i < totalEstacoes; i++) {
        if (contemTexto(listaEstacoes[i].nomeEstacao, termo)) {
            printf(" -> Encontrada: [%d] %s (%s) | %s\n",
                   listaEstacoes[i].idEstacao, listaEstacoes[i].nomeEstacao, listaEstacoes[i].tipo,
                   listaEstacoes[i].ocupada ? "EM USO" : "Livre");
            achou = 1;
        }
    }
    if (!achou) printf("Nenhuma estacao encontrada com \"%s\".\n", termo);
}

// 9. [BONUS] EDICAO NA MEMORIA + REESCRITA TOTAL DO CSV
void editarEstacao() {
    printf("\nDigite o ID da estacao que deseja EDITAR: ");
    int id = lerInteiro();

    int pos = buscarIndiceEstacao(id);
    if (pos == -1) {
        printf("[ERRO] Estacao nao encontrada!\n");
        return;
    }

    printf("Editando '%s'.\n", listaEstacoes[pos].nomeEstacao);
    printf("Novo nome: ");
    lerTexto(listaEstacoes[pos].nomeEstacao, 30);

    escolherTipoEstacao(listaEstacoes[pos].tipo);

    int situacao;
    do {
        printf("Situacao (0-Livre | 1-Em uso): ");
        situacao = lerInteiro();
        if (situacao != 0 && situacao != 1) printf("[ERRO] Digite 0 ou 1!\n");
    } while (situacao != 0 && situacao != 1);
    listaEstacoes[pos].ocupada = situacao;

    salvarEstacoesCSV();
    printf("[OK] Estacao atualizada na memoria e no arquivo CSV!\n");
}

// 10. [BONUS] REMOCAO FISICA NA MEMORIA + REESCRITA TOTAL DO CSV
void removerEstacao() {
    printf("\nDigite o ID da estacao que deseja REMOVER: ");
    int id = lerInteiro();

    int pos = buscarIndiceEstacao(id);
    if (pos == -1) {
        printf("[ERRO] Estacao nao encontrada!\n");
        return;
    }
    if (listaEstacoes[pos].ocupada) {
        printf("[ERRO] Estacao em uso! Encerre a sessao antes de remover.\n");
        return;
    }
    if (estacaoTemLocacoes(id)) {
        printf("[ERRO] Esta estacao tem locacoes no historico! Estorne as locacoes antes de remover.\n");
        return;
    }

    for (int j = pos; j < totalEstacoes - 1; j++) {
        listaEstacoes[j] = listaEstacoes[j + 1]; // puxa a fila para cobrir o item removido
    }
    totalEstacoes--;
    salvarEstacoesCSV();
    printf("[OK] Estacao removida do sistema e do CSV!\n");
}

// 11. SUBMENU (cada case chama a sua propria funcao)
void moduloEstacoes() {
    int opcao;
    do {
        printf("\n--- [MODULO 3: ESTACOES DE JOGO (Integrante C)] ---\n");
        printf("1. Cadastrar Estacao\n");
        printf("2. Listar Estacoes\n");
        printf("3. Buscar por Nome\n");
        printf("4. Editar Estacao\n");
        printf("5. Remover Estacao\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma acao: ");
        opcao = lerInteiro();
        limparTela();

        switch (opcao) {
            case 1: cadastrarEstacao(); break;
            case 2: listarEstacoes(); break;
            case 3: buscarEstacaoPorNome(); break;
            case 4: editarEstacao(); break;
            case 5: removerEstacao(); break;
            case 0: printf("\nRetornando ao menu principal...\n"); break;
            default: printf("\n[ERRO] Opcao invalida no Modulo de Estacoes!\n");
        }
    } while (opcao != 0);
}

#endif
