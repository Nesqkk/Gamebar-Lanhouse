// Arquivo: main.c | Programa principal do Game Bar
// ============================================================================
// GAME BAR - SISTEMA DE GESTAO (LAN HOUSE + BAR + LANCHONETE)
// Checkpoint 2 - Algoritmos e Pensamento Computacional
//
// Servicos do Game Bar:
//  - Bar com cardapio de bebidas
//  - Lanchonete com salgados e doces
//  - Computadores, consoles, celulares e VR (hora de uso = R$ 1,00)
//
// Cada integrante tem o seu modulo em um arquivo .h proprio, com a struct,
// o vetor global, as funcoes de CSV e as funcoes do submenu:
//   utils.h     -> Funcoes comuns (limparTela, lerInteiro, lerDecimal, lerTexto)
//   clientes.h  -> Integrante A (struct Cliente)
//   cardapio.h  -> Integrante B (struct ItemCardapio)
//   estacoes.h  -> Integrante C (struct Estacao e #define PRECO_HORA 1.00)
//   locacoes.h  -> Integrante D (struct Locacao, relatorios e Matriz 2D)
//
// Para compilar: gcc main.c -o gamebar
// ============================================================================

// 1. MODULOS DA EQUIPE (a ordem importa: o locacoes.h usa clientes.h e estacoes.h)
#include "utils.h"
#include "clientes.h"
#include "cardapio.h"
#include "estacoes.h"
#include "locacoes.h"

// 2. FUNCAO VISUAL COMUM DA EQUIPE
void exibirCabecalho() {
    printf("\n======================================================\n");
    printf("      GAME BAR - SISTEMA DE GESTAO DA LAN HOUSE       \n");
    printf("======================================================\n");
}

// 3. FUNCAO PRINCIPAL - DESPACHANTE DE MENUS
int main() {
    int opcao;

    // Carrega os 4 arquivos .csv ao abrir o programa
    carregarClientesCSV();
    carregarCardapioCSV();
    carregarEstacoesCSV();
    carregarLocacoesCSV();

    do {
        exibirCabecalho();
        printf("SELECIONE O MODULO DO SISTEMA:\n");
        printf("1. Modulo de Clientes (Ruan Pablo)\n");
        printf("2. Modulo de Cardapio - Bar e Lanchonete (Nicolas Marques)\n");
        printf("3. Modulo de Estacoes de Jogo (Joao Paulo)\n");
        printf("4. Modulo de Locacoes e Relatorios (Joao Pedro)\n");
        printf("0. Encerrar o Sistema\n");
        printf("------------------------------------------------------\n");
        printf("Escolha uma acao: ");
        opcao = lerInteiro();
        limparTela();

        switch (opcao) {
            case 1: moduloClientes(); break;
            case 2: moduloCardapio(); break;
            case 3: moduloEstacoes(); break;
            case 4: moduloLocacoesRelatorios(); break;
            case 0: printf("\nEncerrando o sistema Game Bar... Ate logo!\n"); break;
            default: printf("\n[ERRO] Opcao invalida! Digite um numero de 0 a 4.\n");
        }
    } while (opcao != 0);

    return 0;
}
