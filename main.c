#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

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
// o vetor global, as funcoes de CSV e as subfunÃ§Ãµes do submenu:
//   clientes.h  -> Integrante A (struct Cliente)
//   cardapio.h  -> Integrante B (struct ItemCardapio)
//   estacoes.h  -> Integrante C (struct Estacao e #define PRECO_HORA 1.00)
//   locacoes.h  -> Integrante D (struct Locacao, relatorios e Matriz 2D)
// ============================================================================

// ============================================================================
// PARTE 1: MODULOS DA EQUIPE (1 ARQUIVO .h POR INTEGRANTE)
// A ordem importa: o locacoes.h usa o clientes.h e o estacoes.h
// ============================================================================
#include "clientes.h"
#include "cardapio.h"
#include "estacoes.h"
#include "locacoes.h"

// ============================================================================
// PARTE 2: FUNCOES DE INTERFACE COMUNS DA EQUIPE
// ============================================================================

// Limpa a tela a cada troca de menu (cls no Windows, clear no Linux/Mac)
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Funcao Visual Comum da Equipe
void exibirCabecalho() {
    printf("\n======================================================\n");
    printf("      GAME BAR - SISTEMA DE GESTAO DA LAN HOUSE       \n");
    printf("======================================================\n");
}

// ============================================================================
// PARTE 3: FUNCAO PRINCIPAL (MAIN) - DESPACHANTE DE MENUS
// ============================================================================
int main() {
    int opcaoPrincipal;

    setlocale(LC_CTYPE, "Portuguese");

    // Carrega os 4 arquivos .csv ao abrir o programa
    carregarClientesCSV();
    carregarCardapioCSV();
    carregarEstacoesCSV();
    carregarLocacoesCSV();

    do {
        exibirCabecalho();
        printf("SELECIONE A PERSPECTIVA DO SISTEMA:\n");
        printf("1. Modulo de Clientes (Ruan Pablo)\n");
        printf("2. Modulo de Cardapio - Bar e Lanchonete (Nicolas Marques)\n");
        printf("3. Modulo de Estacoes de Jogo (João Paulo)\n");
        printf("4. Modulo de Locacoes e Relatorios (João Pedro)\n");
        printf("0. Encerrar o Sistema\n");
        printf("------------------------------------------------------\n");
        printf("Digite a opcao desejada: ");
        scanf("%d", &opcaoPrincipal);
        limparTela();

        switch (opcaoPrincipal) {
            case 1:
                moduloClientes();
                break;
            case 2:
                moduloCardapio();
                break;
            case 3:
                moduloEstacoes();
                break;
            case 4:
                moduloLocacoesRelatorios();
                break;
            case 0:
                printf("\nEncerrando o sistema Game Bar... Ate logo!\n");
                break;
            default:
                printf("\n[ALERTA] Opcao invalida! Digite um numero de 0 a 4.\n");
        }
    } while (opcaoPrincipal != 0);

    return 0;
}
