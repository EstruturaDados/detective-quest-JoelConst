#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==============================================
// ESTRUTURAS DE DADOS
// ==============================================

// Estrutura para armazenar uma pista em árvore binária
typedef struct PistaNode {
    char texto[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// Estrutura principal da mansão (árvore binária de salas)
typedef struct Sala {
    char nome[50];
    PistaNode *pistas;         // Raiz da árvore de pistas desta sala
    struct Sala *esquerda;     // Caminho à esquerda (outra sala)
    struct Sala *direita;      // Caminho à direita (outra sala)
} Sala;

// ==============================================
// FUNÇÕES PARA MANIPULAÇÃO DE PISTAS
// ==============================================

// Cria uma nova pista dinamicamente
PistaNode* criarPista(const char *texto) {
    PistaNode *nova = (PistaNode*) malloc(sizeof(PistaNode));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a pista.\n");
        exit(1);
    }
    strcpy(nova->texto, texto);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// Insere uma pista na árvore binária (ordem alfabética)
PistaNode* inserirPista(PistaNode *raiz, const char *texto) {
    if (raiz == NULL)
        return criarPista(texto);

    if (strcmp(texto, raiz->texto) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, texto);
    else
        raiz->direita = inserirPista(raiz->direita, texto);

    return raiz;
}

// Exibe as pistas em ordem alfabética (recursivo)
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL)
        return;

    exibirPistas(raiz->esquerda);
    printf("  • %s\n", raiz->texto);
    exibirPistas(raiz->direita);
}

// Libera toda a árvore de pistas
void liberarPistas(PistaNode *raiz) {
    if (raiz == NULL)
        return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

// ==============================================
// FUNÇÕES PARA MANIPULAÇÃO DE SALAS
// ==============================================

// Cria dinamicamente uma sala com nome e sem conexões
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a sala.\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    nova->pistas = NULL; // Nenhuma pista no início
    return nova;
}

// Exibe as pistas de uma sala (se houver)
void mostrarPistasSala(Sala *sala) {
    if (sala->pistas == NULL) {
        printf("Nenhuma pista encontrada nesta sala.\n");
    } else {
        printf("\nPistas encontradas em %s:\n", sala->nome);
        exibirPistas(sala->pistas);
    }
}

// Exploração recursiva da mansão
void explorarSalas(Sala *atual) {
    char escolha;

    if (atual == NULL)
        return;

    printf("\nVocê entrou em: %s\n", atual->nome);
    mostrarPistasSala(atual);

    // Caso seja um nó-folha
    if (atual->esquerda == NULL && atual->direita == NULL) {
        printf("Você chegou ao fim deste caminho. Volte e investigue outras salas!\n");
        return;
    }

    do {
        printf("\nEscolha um caminho:\n");
        if (atual->esquerda != NULL) printf(" (e) Esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita != NULL) printf(" (d) Direita  -> %s\n", atual->direita->nome);
        printf(" (s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &escolha);

        switch (escolha) {
            case 'e':
            case 'E':
                if (atual->esquerda != NULL)
                    explorarSalas(atual->esquerda);
                else
                    printf("Não há caminho à esquerda!\n");
                return;
            case 'd':
            case 'D':
                if (atual->direita != NULL)
                    explorarSalas(atual->direita);
                else
                    printf("Não há caminho à direita!\n");
                return;
            case 's':
            case 'S':
                printf("Encerrando exploração...\n");
                return;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (1);
}

// Libera toda a árvore de salas (e suas pistas)
void liberarMansao(Sala *raiz) {
    if (raiz == NULL)
        return;
    liberarMansao(raiz->esquerda);
    liberarMansao(raiz->direita);
    liberarPistas(raiz->pistas);
    free(raiz);
}

// ==============================================
// FUNÇÃO PRINCIPAL
// ==============================================
int main() {
    // Criação da estrutura fixa da mansão
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *porao = criarSala("Porão Misterioso");
    Sala *quarto = criarSala("Quarto de Hóspedes");
    Sala *escritorio = criarSala("Escritório Secreto");

    // Conexões (árvore binária de salas)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->esquerda = porao;
    cozinha->direita = quarto;
    quarto->direita = escritorio;

    // Inserindo pistas nas salas
    hall->pistas = inserirPista(hall->pistas, "Um mapa antigo com marcações misteriosas.");
    hall->pistas = inserirPista(hall->pistas, "Pegadas de lama levam à cozinha.");

    salaEstar->pistas = inserirPista(salaEstar->pistas, "Um copo quebrado perto da lareira.");
    salaEstar->pistas = inserirPista(salaEstar->pistas, "Um perfume diferente no ar.");

    biblioteca->pistas = inserirPista(biblioteca->pistas, "Um livro faltando na estante.");
    biblioteca->pistas = inserirPista(biblioteca->pistas, "Anotações sobre enigmas e códigos.");

    porao->pistas = inserirPista(porao->pistas, "Uma caixa trancada sem chave.");
    porao->pistas = inserirPista(porao->pistas, "Ruídos estranhos vindos do fundo.");

    escritorio->pistas = inserirPista(escritorio->pistas, "Um diário com páginas arrancadas.");
    escritorio->pistas = inserirPista(escritorio->pistas, "Uma luva ensanguentada.");

    // Introdução
    printf("=========================================\n");
    printf("      🕵️  DETECTIVE QUEST - A MANSÃO 🏰\n");
    printf("=========================================\n");
    printf("Você começa no Hall de Entrada.\n");
    printf("Explore cada sala, colete pistas e descubra o culpado!\n");

    // Inicia exploração
    explorarSalas(hall);

    // Libera memória
    liberarMansao(hall);
    printf("\nJogo encerrado. Todas as memórias foram liberadas.\n");
    return 0;
}
