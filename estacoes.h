// Arquivo: estacoes.h | Modulo 3: Estacoes de Jogo | Autor: João Paulo Rocha Trindade 49222767
#ifndef ESTACOES_H
#define ESTACOES_H

#include <stdio.h>
#include <string.h>

#define MAX_ESTACOES 50
#define PRECO_HORA 1.00  // valor da hora de uso de qualquer estacao (R$)

void limparTela(); // prototipo: a funcao completa fica no main.c da equipe

struct Estacao {
    int idEstacao;
    char nomeEstacao[30];  // Ex: "PC Gamer 01", "PS5 Mesa 2"
    char tipo[20];         // "Computador", "Console", "Celular" ou "VR"
    float precoHora;       // usa PRECO_HORA (R$ 1,00)
    int ocupada;           // 1 = Em uso, 0 = Livre
    int ativa;             // 1 = Ativa (a baixa e fisica, entao fica sempre 1)
};

struct Estacao estacoes[MAX_ESTACOES];
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
                estacoes[i].idEstacao,
                estacoes[i].nomeEstacao,
                estacoes[i].tipo,
                estacoes[i].precoHora,
                estacoes[i].ocupada);
    }
    fclose(arq);
}

// 2. PERSISTENCIA: carrega estacoes.csv para o vetor ao abrir o programa
void carregarEstacoesCSV() {
    FILE *arq = fopen("estacoes.csv", "r");
    if (arq == NULL) return; // se ainda nao existir arquivo, inicia com 0 registros

    totalEstacoes = 0;
    while (totalEstacoes < MAX_ESTACOES &&
           fscanf(arq, "%d;%29[^;];%19[^;];%f;%d\n",
                  &estacoes[totalEstacoes].idEstacao,
                  estacoes[totalEstacoes].nomeEstacao,
                  estacoes[totalEstacoes].tipo,
                  &estacoes[totalEstacoes].precoHora,
                  &estacoes[totalEstacoes].ocupada) == 5) {
        estacoes[totalEstacoes].ativa = 1;
        totalEstacoes++;
    }
    fclose(arq);
}

// 3. FUNCAO CHAVE-MESTRA: devolve a posicao do ID no vetor, ou -1 se nao existir
//    (o Modulo 4 usa esta funcao para validar a estacao na locacao)
int buscarIndiceEstacao(int idProcurado) {
    for (int i = 0; i < totalEstacoes; i++) {
        if (estacoes[i].idEstacao == idProcurado) return i;
    }
    return -1;
}

// 4. AUTOINCREMENT: laco que acha o maior ID e devolve o proximo (nunca repete)
int gerarIdEstacao() {
    int maior = 0;
    for (int i = 0; i < totalEstacoes; i++) {
        if (estacoes[i].idEstacao > maior) maior = estacoes[i].idEstacao;
    }
    return maior + 1;
}

// 5. PROTECAO DO CSV: troca ';' digitado pelo usuario por espaco
void trocarPontoVirgulaEstacao(char texto[]) {
    for (int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';') texto[i] = ' ';
    }
}

// 6. ESCOLHA DO TIPO (so aceita Computador, Console, Celular ou VR)
void escolherTipoEstacao(char destino[]) {
    int op;
    do {
        printf("Tipo (1-Computador | 2-Console | 3-Celular | 4-VR): ");
        scanf("%d", &op);
        if (op == 1) strcpy(destino, "Computador");
        else if (op == 2) strcpy(destino, "Console");
        else if (op == 3) strcpy(destino, "Celular");
        else if (op == 4) strcpy(destino, "VR");
        else printf("[ERRO] Tipo invalido!\n");
    } while (op < 1 || op > 4);
}

// 7. CADASTRO (ID automatico + preco fixo PRECO_HORA + gravacao no CSV)
void cadastrarEstacao() {
    if (totalEstacoes >= MAX_ESTACOES) {
        printf("[ERRO] Limite de %d estacoes atingido!\n", MAX_ESTACOES);
        return;
    }

    estacoes[totalEstacoes].idEstacao = gerarIdEstacao();
    printf("\nNova estacao (ID gerado: %d)\n", estacoes[totalEstacoes].idEstacao);

    printf("Nome da estacao (ex: PC Gamer 05): ");
    scanf(" %29[^\n]", estacoes[totalEstacoes].nomeEstacao);
    trocarPontoVirgulaEstacao(estacoes[totalEstacoes].nomeEstacao);

    escolherTipoEstacao(estacoes[totalEstacoes].tipo);

    estacoes[totalEstacoes].precoHora = PRECO_HORA;
    estacoes[totalEstacoes].ocupada = 0;
    estacoes[totalEstacoes].ativa = 1;
    totalEstacoes++;
    salvarEstacoesCSV();
    printf("[OK] Estacao salva em 'estacoes.csv' (hora a R$ %.2f)!\n", PRECO_HORA);
}

// 8. LISTAGEM FORMATADA (mostra se esta livre ou em uso)
void listarEstacoes() {
    printf("\n--- ESTACOES DE JOGO (%d cadastradas) ---\n", totalEstacoes);
    if (totalEstacoes == 0) {
        printf("Nenhuma estacao cadastrada.\n");
        return;
    }
    for (int i = 0; i < totalEstacoes; i++) {
        printf("ID: %-3d | %-24s | %-10s | R$ %.2f/h | %s\n",
               estacoes[i].idEstacao, estacoes[i].nomeEstacao, estacoes[i].tipo,
               estacoes[i].precoHora, estacoes[i].ocupada ? "EM USO" : "Livre");
    }
}

// 9. BUSCA TEXTUAL COM strstr (parte do nome)
void buscarEstacaoPorNome() {
    char termo[30];
    int achou = 0;

    printf("\nDigite parte do nome da estacao: ");
    scanf(" %29[^\n]", termo);

    for (int i = 0; i < totalEstacoes; i++) {
        if (strstr(estacoes[i].nomeEstacao, termo) != NULL) {
            printf(" -> Encontrada: [%d] %s (%s) | %s\n",
                   estacoes[i].idEstacao, estacoes[i].nomeEstacao, estacoes[i].tipo,
                   estacoes[i].ocupada ? "EM USO" : "Livre");
            achou = 1;
        }
    }
    if (!achou) printf("Nenhuma estacao encontrada com \"%s\".\n", termo);
}

// 10. [BONUS] EDICAO NA MEMORIA + REESCRITA TOTAL DO CSV
void editarEstacao() {
    int id;
    printf("\nDigite o ID da estacao que deseja EDITAR: ");
    scanf("%d", &id);

    int pos = buscarIndiceEstacao(id);
    if (pos == -1) {
        printf("[ERRO] Estacao nao encontrada!\n");
        return;
    }

    printf("Editando '%s'.\n", estacoes[pos].nomeEstacao);
    printf("Novo nome: ");
    scanf(" %29[^\n]", estacoes[pos].nomeEstacao);
    trocarPontoVirgulaEstacao(estacoes[pos].nomeEstacao);

    escolherTipoEstacao(estacoes[pos].tipo);

    int situacao;
    do {
        printf("Situacao (0-Livre | 1-Em uso): ");
        scanf("%d", &situacao);
        if (situacao != 0 && situacao != 1) printf("[ERRO] Digite 0 ou 1!\n");
    } while (situacao != 0 && situacao != 1);
    estacoes[pos].ocupada = situacao;

    salvarEstacoesCSV();
    printf("[OK] Estacao atualizada na memoria e no arquivo CSV!\n");
}

// 11. [BONUS] BAIXA FISICA NA MEMORIA + REESCRITA TOTAL DO CSV
void removerEstacao() {
    int id;
    printf("\nDigite o ID da estacao que deseja REMOVER: ");
    scanf("%d", &id);

    int pos = buscarIndiceEstacao(id);
    if (pos == -1) {
        printf("[ERRO] Estacao nao encontrada!\n");
        return;
    }
    if (estacoes[pos].ocupada) {
        printf("[ERRO] Estacao em uso! Encerre a sessao antes de remover.\n");
        return;
    }

    for (int j = pos; j < totalEstacoes - 1; j++) {
        estacoes[j] = estacoes[j + 1]; // puxa a fila para cobrir o item removido
    }
    totalEstacoes--;
    salvarEstacoesCSV();
    printf("[OK] Estacao removida do sistema e do CSV!\n");
}

// 12. SUBMENU (cada case chama a sua propria funcao)
void moduloEstacoes() {
    int opcaoEstacao;
    do {
        printf("\n--- [MODULO 3: ESTACOES DE JOGO (Integrante C)] ---\n");
        printf("1. Cadastrar | 2. Listar | 3. Buscar por Nome | 4. Editar | 5. Remover | 0. Voltar: ");
        scanf("%d", &opcaoEstacao);
        limparTela();

        switch (opcaoEstacao) {
            case 1: cadastrarEstacao(); break;
            case 2: listarEstacoes(); break;
            case 3: buscarEstacaoPorNome(); break;
            case 4: editarEstacao(); break;
            case 5: removerEstacao(); break;
            case 0: printf("\nRetornando ao menu principal...\n"); break;
            default: printf("\n[ERRO] Opcao invalida no Modulo de Estacoes!\n");
        }
    } while (opcaoEstacao != 0);
}

#endif
