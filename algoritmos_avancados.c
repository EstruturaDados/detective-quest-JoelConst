#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===============================
// Estrutura de dados
// ===============================
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ===============================
// Função: criarSala
// Cria uma sala dinamicamente e define seu nome.
// ===============================
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a sala.\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ===============================
// Função: explorarSalas
// Permite a exploração interativa da mansão.
// ===============================
void explorarSalas(Sala *atual) {
    char escolha;

    if (atual == NULL)
        return;

    printf("\nVocê entrou em: %s\n", atual->nome);

    // Caso a sala seja um nó-folha (sem caminhos)
    if (atual->esquerda == NULL && atual->direita == NULL) {
        printf("Você chegou a um fim de caminho! Não há mais saídas.\n");
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
                printf("Encerrando a exploração...\n");
                return;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (1);
}

// ===============================
// Função: liberarArvore
// Libera toda a memória alocada da árvore.
// ===============================
void liberarArvore(Sala *raiz) {
    if (raiz == NULL)
        return;
    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);
    free(raiz);
}

// ===============================
// Função principal: main
// Monta o mapa da mansão e inicia a exploração.
// ===============================
int main() {
    // Criação manual da árvore (estrutura fixa da mansão)
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *porao = criarSala("Porão Misterioso");
    Sala *quarto = criarSala("Quarto de Hóspedes");
    Sala *escritorio = criarSala("Escritório Secreto");

    // Conexões (estrutura da árvore binária)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->esquerda = porao;
    cozinha->direita = quarto;
    quarto->direita = escritorio;

    printf("=========================================\n");
    printf("      🕵️  DETECTIVE QUEST - A MANSÃO 🏰\n");
    printf("=========================================\n");
    printf("Você começa no Hall de Entrada.\n");
    printf("Escolha caminhos (e/d) para explorar.\n");
    printf("Boa sorte na sua investigação!\n");

    explorarSalas(hall);

    liberarArvore(hall);
    printf("\nJogo encerrado. Obrigado por jogar Detective Quest!\n");
    return 0;
}
