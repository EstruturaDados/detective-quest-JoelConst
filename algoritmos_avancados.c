#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_TEXTO 256
#define HASH_SIZE 101

// ==============================================
// ESTRUTURAS
// ==============================================

// Nó da BST que guarda pistas coletadas (ordenadas por texto)
typedef struct PistaNode {
    char texto[MAX_TEXTO];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// Nó da lista encadeada para cada bucket da tabela hash
typedef struct HashEntry {
    char *chave;          // texto da pista (key)
    char *suspeito;       // nome do suspeito (value)
    struct HashEntry *proximo;
} HashEntry;

// Tabela hash simples (encadeamento)
typedef struct HashTable {
    HashEntry *buckets[HASH_SIZE];
} HashTable;

// Estrutura da sala (nó da árvore binária da mansão)
typedef struct Sala {
    char nome[MAX_NOME];
    char pista[MAX_TEXTO];    // pista estática associada à sala (se vazia, nenhuma pista)
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ==============================================
// PROTÓTIPOS
// ==============================================

// Salas
Sala* criarSala(const char *nome, const char *pista);
// exploração
void explorarSalas(Sala *atual, PistaNode **pistasColetadas, HashTable *hash);
// liberação de salas
void liberarMansao(Sala *raiz);

// Pistas BST
PistaNode* criarPistaNode(const char *texto);
PistaNode* inserirPistaBST(PistaNode *raiz, const char *texto);
void exibirPistasInOrder(PistaNode *raiz);
void liberarPistasBST(PistaNode *raiz);

// Hash
void inicializarHash(HashTable *h);
unsigned long hash_djb2(const char *str);
void inserirNaHash(HashTable *h, const char *chave, const char *suspeito);
char* encontrarSuspeito(HashTable *h, const char *chave);
void liberarHash(HashTable *h);

// Funções auxiliares
void limparEntrada();
void trimNL(char *s);

// Verificação final (julgamento)
int contarPistasPorSuspeito(PistaNode *raiz, HashTable *h, const char *acusado);
void verificarSuspeitoFinal(PistaNode *pistasColetadas, HashTable *h);

// ==============================================
// IMPLEMENTAÇÃO
// ==============================================

// ========================= SALAS =========================

/*
 * criarSala()
 * Cria dinamicamente um cômodo (Sala) com nome e pista associada.
 * Retorna ponteiro para a Sala criada (alocada com malloc).
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro: falha na alocação de memória para Sala.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, MAX_NOME - 1);
    s->nome[MAX_NOME - 1] = '\0';
    if (pista) {
        strncpy(s->pista, pista, MAX_TEXTO - 1);
        s->pista[MAX_TEXTO - 1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esquerda = s->direita = NULL;
    return s;
}

// Libera toda a árvore de salas
void liberarMansao(Sala *raiz) {
    if (!raiz) return;
    liberarMansao(raiz->esquerda);
    liberarMansao(raiz->direita);
    free(raiz);
}

// ========================= PISTAS BST =========================

PistaNode* criarPistaNode(const char *texto) {
    PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
    if (!n) {
        fprintf(stderr, "Erro: falha na alocação de memória para PistaNode.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(n->texto, texto, MAX_TEXTO - 1);
    n->texto[MAX_TEXTO - 1] = '\0';
    n->esquerda = n->direita = NULL;
    return n;
}

/*
 * inserirPista()
 * Insere a pista coletada na BST (ordenada por texto).
 * Evita duplicatas (se já existe, não insere novamente).
 */
PistaNode* inserirPistaBST(PistaNode *raiz, const char *texto) {
    if (raiz == NULL) {
        return criarPistaNode(texto);
    }
    int cmp = strcmp(texto, raiz->texto);
    if (cmp == 0) {
        // já coletada
        return raiz;
    } else if (cmp < 0) {
        raiz->esquerda = inserirPistaBST(raiz->esquerda, texto);
    } else {
        raiz->direita = inserirPistaBST(raiz->direita, texto);
    }
    return raiz;
}

// Exibe pistas em ordem (in-order traversal)
void exibirPistasInOrder(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistasInOrder(raiz->esquerda);
    printf(" - %s\n", raiz->texto);
    exibirPistasInOrder(raiz->direita);
}

void liberarPistasBST(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistasBST(raiz->esquerda);
    liberarPistasBST(raiz->direita);
    free(raiz);
}

// ========================= HASH =========================

void inicializarHash(HashTable *h) {
    for (int i = 0; i < HASH_SIZE; ++i) h->buckets[i] = NULL;
}

// djb2 (string hash)
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_SIZE;
}

/*
 * inserirNaHash()
 * Insere associação pista -> suspeito na tabela hash.
 * Faz cópia das strings (aloca dinamicamente).
 */
void inserirNaHash(HashTable *h, const char *chave, const char *suspeito) {
    unsigned long idx = hash_djb2(chave);
    HashEntry *ent = (HashEntry*) malloc(sizeof(HashEntry));
    if (!ent) {
        fprintf(stderr, "Erro: falha na alocação de memória para HashEntry.\n");
        exit(EXIT_FAILURE);
    }
    ent->chave = (char*) malloc(strlen(chave) + 1);
    ent->suspeito = (char*) malloc(strlen(suspeito) + 1);
    if (!ent->chave || !ent->suspeito) {
        fprintf(stderr, "Erro: falha na alocação de memória para strings da hash.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(ent->chave, chave);
    strcpy(ent->suspeito, suspeito);
    ent->proximo = h->buckets[idx];
    h->buckets[idx] = ent;
}

/*
 * encontrarSuspeito()
 * Retorna o nome do suspeito associado à chave (pista),
 * ou NULL se não encontrar.
 */
char* encontrarSuspeito(HashTable *h, const char *chave) {
    unsigned long idx = hash_djb2(chave);
    HashEntry *it = h->buckets[idx];
    while (it) {
        if (strcmp(it->chave, chave) == 0)
            return it->suspeito;
        it = it->proximo;
    }
    return NULL;
}

// Libera tabela hash e todas as entradas
void liberarHash(HashTable *h) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashEntry *it = h->buckets[i];
        while (it) {
            HashEntry *tmp = it;
            it = it->proximo;
            free(tmp->chave);
            free(tmp->suspeito);
            free(tmp);
        }
        h->buckets[i] = NULL;
    }
}

// ========================= AUXILIARES =========================

void limparEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void trimNL(char *s) {
    size_t L = strlen(s);
    if (L == 0) return;
    if (s[L-1] == '\n') s[L-1] = '\0';
}

// ========================= EXPLORAÇÃO =========================

/*
 * explorarSalas()
 * Permite a navegação do jogador pela árvore de salas.
 * Ao entrar numa sala, exibe a pista associada (se houver),
 * insere a pista na BST de pistas coletadas e continua recursivamente
 * conforme escolha do jogador (e/d) ou retorna se escolher 's'.
 *
 * Parâmetros:
 *  - atual: ponteiro para sala atual
 *  - pistasColetadas: endereço do ponteiro para a raiz da BST de pistas
 *  - hash: tabela hash já preenchida (usada apenas para associação/consulta posterior)
 */
void explorarSalas(Sala *atual, PistaNode **pistasColetadas, HashTable *hash) {
    if (!atual) return;

    char escolha;

    printf("\nVocê entrou em: %s\n", atual->nome);
    if (strlen(atual->pista) > 0) {
        printf("Pista encontrada: %s\n", atual->pista);
        // adiciona na BST (evita duplicatas)
        *pistasColetadas = inserirPistaBST(*pistasColetadas, atual->pista);
    } else {
        printf("Nenhuma pista visível nesta sala.\n");
    }

    // Se não houver caminhos, retornar (nó-folha)
    if (!atual->esquerda && !atual->direita) {
        printf("Fim do corredor. Retorne ou saia para finalizar.\n");
        return;
    }

    // Menu de navegação
    do {
        printf("\nEscolha um caminho:\n");
        if (atual->esquerda) printf(" (e) Esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita) printf(" (d) Direita  -> %s\n", atual->direita->nome);
        printf(" (s) Sair da exploração (voltar/julgamento final)\n");
        printf("Opção: ");
        if (scanf(" %c", &escolha) != 1) {
            limparEntrada();
            escolha = 'x';
        } else {
            limparEntrada();
        }

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda) {
                explorarSalas(atual->esquerda, pistasColetadas, hash);
            } else {
                printf("Não existe caminho à esquerda.\n");
            }
            return;
        } else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita) {
                explorarSalas(atual->direita, pistasColetadas, hash);
            } else {
                printf("Não existe caminho à direita.\n");
            }
            return;
        } else if (escolha == 's' || escolha == 'S') {
            printf("Você optou por sair da exploração.\n");
            return;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    } while (1);
}

// ========================= JULGAMENTO FINAL =========================

/*
 * contarPistasPorSuspeito()
 * Percorre a BST de pistas coletadas e conta quantas pistas
 * apontam para o suspeito 'acusado' consultando a hash.
 * É recursiva.
 */
int contarPistasPorSuspeito(PistaNode *raiz, HashTable *h, const char *acusado) {
    if (!raiz) return 0;
    int count = 0;
    // esquerda
    count += contarPistasPorSuspeito(raiz->esquerda, h, acusado);
    // atual
    char *sus = encontrarSuspeito(h, raiz->texto);
    if (sus && strcmp(sus, acusado) == 0) count++;
    // direita
    count += contarPistasPorSuspeito(raiz->direita, h, acusado);
    return count;
}

/*
 * verificarSuspeitoFinal()
 * Solicita ao jogador que acuse um suspeito e verifica se existem
 * pelo menos 2 pistas que apontem para esse suspeito.
 * Exibe o desfecho.
 */
void verificarSuspeitoFinal(PistaNode *pistasColetadas, HashTable *h) {
    if (!pistasColetadas) {
        printf("\nVocê não coletou nenhuma pista. Não há elementos para acusação.\n");
        return;
    }

    printf("\n=== FASE DE ACUSAÇÃO ===\n");
    printf("Pistas coletadas (ordenadas):\n");
    exibirPistasInOrder(pistasColetadas);

    char acusado[MAX_NOME];
    printf("\nDigite o nome do suspeito que você acusa: ");
    if (!fgets(acusado, sizeof(acusado), stdin)) {
        printf("Entrada inválida.\n");
        return;
    }
    trimNL(acusado);
    if (strlen(acusado) == 0) {
        printf("Nome do suspeito vazio. Acusação cancelada.\n");
        return;
    }

    int num = contarPistasPorSuspeito(pistasColetadas, h, acusado);
    printf("\nNúmero de pistas que ligam ao suspeito '%s': %d\n", acusado, num);

    if (num >= 2) {
        printf("Veredito: Acusação procedente! Há evidências suficientes para prender %s.\n", acusado);
    } else {
        printf("Veredito: Acusação insuficiente. %s NÃO pode ser preso com base nas pistas coletadas.\n", acusado);
    }
}

// ==============================================
// MAIN - monta mansão, hash e inicia exploração
// ==============================================
int main() {
    // Montagem fixa da mansão (árvore de salas)
    Sala *hall = criarSala("Hall de Entrada", "Pegadas molhadas que vêm do jardim");
    Sala *salaEstar = criarSala("Sala de Estar", "Copo quebrado com resquícios de vinho");
    Sala *cozinha = criarSala("Cozinha", "Pano com cheiro forte de amônia");
    Sala *biblioteca = criarSala("Biblioteca", "Livro faltando com marca de dobra");
    Sala *jardim = criarSala("Jardim", "Pegadas que levam ao portão dos fundos");
    Sala *porao = criarSala("Porão Misterioso", "Uma caixa trancada com arranhões");
    Sala *quarto = criarSala("Quarto de Hóspedes", "Fios arrancados do abajur");
    Sala *escritorio = criarSala("Escritório Secreto", "Diário rasgado com palavras mencionando 'remoção'");

    // Conexões (árvore binária)
    hall->esquerda = salaEstar;
    hall->direita  = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;
    cozinha->esquerda = porao;
    cozinha->direita  = quarto;
    quarto->direita = escritorio;

    // Inicializa hash de pistas->suspeito
    HashTable ht;
    inicializarHash(&ht);

    // Associações pista -> suspeito (fixas, definidas no código)
    // OBS: as chaves devem corresponder exatamente ao texto das pistas das salas
    inserirNaHash(&ht, "Pegadas molhadas que vêm do jardim", "Sr. Verde");
    inserirNaHash(&ht, "Copo quebrado com resquícios de vinho", "Sra. Rosa");
    inserirNaHash(&ht, "Pano com cheiro forte de amônia", "Dr. Azul");
    inserirNaHash(&ht, "Livro faltando com marca de dobra", "Sra. Rosa");
    inserirNaHash(&ht, "Pegadas que levam ao portão dos fundos", "Sr. Verde");
    inserirNaHash(&ht, "Uma caixa trancada com arranhões", "Dr. Azul");
    inserirNaHash(&ht, "Fios arrancados do abajur", "Sra. Amarela");
    inserirNaHash(&ht, "Diário rasgado com palavras mencionando 'remoção'", "Sra. Amarela");

    // Raiz da BST de pistas coletadas (inicialmente vazia)
    PistaNode *pistasColetadas = NULL;

    // Introdução e início da exploração a partir do Hall
    printf("=========================================\n");
    printf("      🕵️  DETECTIVE QUEST - EPÍLOGO 🏰\n");
    printf("=========================================\n");
    printf("Você começa no Hall de Entrada.\n");
    printf("Explore a mansão (escolhas: e = esquerda, d = direita, s = sair).\n");
    printf("Colete pistas; no final você poderá acusar um suspeito.\n");

    // Exploração interativa (recursiva)
    explorarSalas(hall, &pistasColetadas, &ht);

    // Fase final: acusação e verificação
    verificarSuspeitoFinal(pistasColetadas, &ht);

    // Limpeza de memória
    liberarPistasBST(pistasColetadas);
    liberarHash(&ht);
    liberarMansao(hall);

    printf("\nJogo encerrado. Memória liberada. Obrigado por jogar!\n");
    return 0;
}
