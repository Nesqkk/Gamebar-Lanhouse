// Arquivo: utils.h | Funcoes comuns da equipe (tela e leitura do teclado)
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 1. LIMPA A TELA (cls no Windows, clear no Linux/Mac)
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// 2. LE UM NUMERO INTEIRO (se digitar letra, avisa e pede de novo)
int lerInteiro() {
    char linha[50];
    int numero;
    while (1) {
        if (fgets(linha, sizeof(linha), stdin) == NULL) exit(0);
        if (sscanf(linha, "%d", &numero) == 1) return numero;
        printf("[ERRO] Digite um numero inteiro: ");
    }
}

// 3. LE UM NUMERO COM VIRGULA (aceita 2,50 ou 2.50)
float lerDecimal() {
    char linha[50];
    float numero;
    while (1) {
        if (fgets(linha, sizeof(linha), stdin) == NULL) exit(0);
        for (int i = 0; linha[i] != '\0'; i++) {
            if (linha[i] == ',') linha[i] = '.';
        }
        if (sscanf(linha, "%f", &numero) == 1) return numero;
        printf("[ERRO] Digite um numero (ex: 2.50): ");
    }
}

// 4. LE UM TEXTO COM ESPACOS (nao aceita vazio e troca ';' por espaco,
//    pois o ';' separa as colunas do arquivo .csv)
void lerTexto(char destino[], int tamanho) {
    while (1) {
        if (fgets(destino, tamanho, stdin) == NULL) exit(0);

        int achouFim = 0;
        for (int i = 0; destino[i] != '\0'; i++) {
            if (destino[i] == '\n' || destino[i] == '\r') {
                destino[i] = '\0';
                achouFim = 1;
                break;
            }
            if (destino[i] == ';') destino[i] = ' ';
        }
        if (!achouFim) {  // texto maior que o limite: descarta o que sobrou
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        if (destino[0] != '\0') return;
        printf("[ERRO] Este campo nao pode ficar vazio! Digite novamente: ");
    }
}

// 5. BUSCA DE TEXTO SEM DIFERENCIAR MAIUSCULA DE MINUSCULA
//    (devolve 1 se "termo" aparece dentro de "texto", senao 0)
int contemTexto(char texto[], char termo[]) {
    char t1[100], t2[100];
    int i;
    for (i = 0; texto[i] != '\0' && i < 99; i++) t1[i] = tolower((unsigned char)texto[i]);
    t1[i] = '\0';
    for (i = 0; termo[i] != '\0' && i < 99; i++) t2[i] = tolower((unsigned char)termo[i]);
    t2[i] = '\0';
    return strstr(t1, t2) != NULL;
}

#endif
