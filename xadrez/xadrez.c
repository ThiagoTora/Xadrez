/*  XADREZ + MENU / CADASTRO / RANKING / RETOMADA  */
/* ------------------------------------------------
   Arquivos usados:
     usuarios.txt        -> cadastro + ranking
     partida_salva.txt   -> retomar partida
   Cada conta  : nome senha vitorias derrotas empates
   Pontuação   : 3 × vit + 1 × emp
--------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------- CONSTANTES ANSI PARA COR ---------- */
#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_YELLOW  "\x1b[33m"

/* --------- PROTÓTIPOS DE FUNÇÕES ----------- */
int emXeque(char cor);
int emXequeMate(char cor);

/* ---------- ESTRUTURAS BÁSICAS -------------- */
typedef struct {
    char simbolo;    /* T C B R K P */
    char cor;        /* 'B' ou 'P'  */
} Pecas;

typedef struct {
    Pecas* peca;
} Casa;

typedef struct {
    char nome[32];
    char senha[32];
    int  vitorias;
    int  derrotas;
    int  empates;
} Usuario;

/* -------------- VARIÁVEIS GLOBAIS ----------- */
Casa* tabuleiro[8][8];
int   jogoEmAndamento = 0;
char  corVencedor      = 0;
char  turnoAtual      = 'B';
char  jogadorBranco[32] = "Branco";
char  jogadorPreto[32] = "Preto";
int   usaLogin = 0;      /* 0 = anônimo; 1 = jogadores logados */

/* -------------- FUNÇÕES AUXILIARES ---------- */
int meuAbs(int x) { return x < 0 ? -x : x; }

/* ---------- FUNÇÕES DE CADASTRO/RANKING ----- */
int lerUsuarios(Usuario usuarios[], int max) {
    FILE* arquivo = fopen("usuarios.txt", "r");
    if (!arquivo) return 0;
    int n = 0;
    while (n < max && fscanf(arquivo, "%31s %31s %d %d %d",
           usuarios[n].nome, usuarios[n].senha,
           &usuarios[n].vitorias, &usuarios[n].derrotas, &usuarios[n].empates) == 5) n++;
    fclose(arquivo);
    return n;
}

void salvarUsuarios(Usuario usuarios[], int n) {
    FILE* arquivo = fopen("usuarios.txt", "w");
    for (int i = 0; i < n; i++)
        fprintf(arquivo, "%s %s %d %d %d\n",
                usuarios[i].nome, usuarios[i].senha,
                usuarios[i].vitorias, usuarios[i].derrotas, usuarios[i].empates);
    fclose(arquivo);
}

int buscarUsuario(Usuario usuarios[], int n, const char* nome) {
    for (int i = 0; i < n; i++)
        if (strcmp(usuarios[i].nome, nome) == 0) return i;
    return -1;
}

void ordenarPorPontos(Usuario usuarios[], int n) {
    for (int i = 0; i < n-1; i++)
        for (int j = i+1; j < n; j++) {
            int pontosI = 3 * usuarios[i].vitorias + usuarios[i].empates;
            int pontosJ = 3 * usuarios[j].vitorias + usuarios[j].empates;
            if (pontosJ > pontosI) { Usuario temp = usuarios[i]; usuarios[i] = usuarios[j]; usuarios[j] = temp; }
        }
}

void mostrarRanking() {
    Usuario usuarios[256]; 
    int n = lerUsuarios(usuarios, 256);
    ordenarPorPontos(usuarios, n);
    puts("\n=== RANKING ===");
    puts("Pontos | Vit | Emp | Der | Nome");
    for (int i = 0; i < n; i++) {
        int pontos = 3 * usuarios[i].vitorias + usuarios[i].empates;
        printf(" %5d | %3d | %3d | %3d | %s\n",
               pontos, usuarios[i].vitorias, usuarios[i].empates, usuarios[i].derrotas, usuarios[i].nome);
    }
    puts("");
}

void atualizarRanking(Usuario usuarios[], int n, int idxVencedor, int idxPerdedor, int empate) {
    if (empate) {
        usuarios[idxVencedor].empates++;
        usuarios[idxPerdedor].empates++;
    } else {
        usuarios[idxVencedor].vitorias++;
        usuarios[idxPerdedor].derrotas++;
    }
    salvarUsuarios(usuarios, n);
}

/* -------------- CRIAÇÃO DE CONTA ------------ */
void criarCadastro() {
    Usuario usuarios[256]; 
    int n = lerUsuarios(usuarios, 256);
    char nome[32], senha[32];
    printf("Novo usuario - digite nome: ");
    scanf("%31s", nome);
    if (buscarUsuario(usuarios, n, nome) != -1) {
        puts("Nome ja existe. Voltando ao menu.\n");
        return;
    }
    printf("Digite senha: ");  
    scanf("%31s", senha);
    strcpy(usuarios[n].nome, nome);
    strcpy(usuarios[n].senha, senha);
    usuarios[n].vitorias = usuarios[n].derrotas = usuarios[n].empates = 0;
    n++;
    salvarUsuarios(usuarios, n);
    puts("Cadastro criado!\n");
}

/* -------------- LOGIN ----------------------- */
int efetuarLogin(char* saidaNome) {
    Usuario usuarios[256]; 
    int n = lerUsuarios(usuarios, 256);
    char nome[32], senha[32];
    printf("Login - nome: ");  
    scanf("%31s", nome);
    printf("Senha: ");         
    scanf("%31s", senha);
    int idx = buscarUsuario(usuarios, n, nome);
    if (idx == -1 || strcmp(usuarios[idx].senha, senha) != 0) {
        puts("Credenciais invalidas. Retornando ao menu.\n");
        return -1;
    }
    strcpy(saidaNome, nome);
    puts("Login ok!\n");
    return idx;   /* retorna índice do usuário */
}

/* --------- FUNÇÕES DE XADREZ ORIGINAIS ------ */
Pecas* criarPeca(char simbolo, char cor) {
    Pecas* p = malloc(sizeof(Pecas));
    p->simbolo = simbolo; 
    p->cor = cor;
    return p;
}

void liberarTabuleiro() {    /* free antes de sair */
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            if (tabuleiro[i][j]) {
                if (tabuleiro[i][j]->peca) free(tabuleiro[i][j]->peca);
                free(tabuleiro[i][j]);
            }
        }
}

void inicializarTabuleiro() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            tabuleiro[i][j] = malloc(sizeof(Casa));
            tabuleiro[i][j]->peca = NULL;
        }
    char pecas[] = { 'T', 'C', 'B', 'R', 'K', 'B', 'C', 'T' };
    for (int j = 0; j < 8; j++) {
        tabuleiro[0][j]->peca = criarPeca(pecas[j], 'P');
        tabuleiro[1][j]->peca = criarPeca('P', 'P');
        tabuleiro[6][j]->peca = criarPeca('P', 'B');
        tabuleiro[7][j]->peca = criarPeca(pecas[j], 'B');
    }
}

void imprimirTabuleiro() {
    printf("\n    "); 
    for (char c = 'A'; c <= 'H'; c++) printf(" %c  ", c); 
    puts("");
    for (int i = 0; i < 8; i++) {
        puts("  +---+---+---+---+---+---+---+---+");
        printf("%d ", 8 - i);
        for (int j = 0; j < 8; j++) {
            Casa* casa = tabuleiro[i][j]; 
            printf("|");
            if (casa->peca) {
                const char* cor = (casa->peca->cor == 'B') ? ANSI_YELLOW : ANSI_RED;
                printf(" %s%c" ANSI_RESET " ", cor, casa->peca->simbolo);
            } else {
                printf("   ");
            }
        }
        printf("| %d\n", 8 - i);
    }
    puts("  +---+---+---+---+---+---+---+---+");
    printf("    "); 
    for (char c = 'A'; c <= 'H'; c++) printf(" %c  ", c); 
    puts("\n");
}

int caminhoLivre(int li, int ci, int lf, int cf) {
    int dli = (lf - li) ? ((lf - li) / meuAbs(lf - li)) : 0;
    int dci = (cf - ci) ? ((cf - ci) / meuAbs(cf - ci)) : 0;
    int i = li + dli, j = ci + dci;
    while (i != lf || j != cf) {
        if (tabuleiro[i][j]->peca) return 0;
        i += dli; j += dci;
    }
    return 1;
}

int movimentoValido(Pecas* p, int li, int ci, int lf, int cf) {
    if (!p) return 0;
    if (lf < 0 || lf > 7 || cf < 0 || cf > 7) return 0;
    if (li == lf && ci == cf) return 0;
    Pecas* destino = tabuleiro[lf][cf]->peca;
    if (destino && destino->cor == p->cor) return 0;

    int dl = lf - li, dc = cf - ci;
    switch(p->simbolo) {
        case 'P': // peão
            if (p->cor == 'B') {
                if (dc == 0 && ((dl == -1 && !destino) || (li == 6 && dl == -2 && !destino && !tabuleiro[li-1][ci]->peca))) return 1;
                if (meuAbs(dc) == 1 && dl == -1 && destino && destino->cor == 'P') return 1;
            } else {
                if (dc == 0 && ((dl == 1 && !destino) || (li == 1 && dl == 2 && !destino && !tabuleiro[li+1][ci]->peca))) return 1;
                if (meuAbs(dc) == 1 && dl == 1 && destino && destino->cor == 'B') return 1;
            }
            return 0;

        case 'T': // torre
            if ((dl == 0 || dc == 0) && caminhoLivre(li, ci, lf, cf)) return 1;
            return 0;

        case 'C': // cavalo
            if ((meuAbs(dl) == 2 && meuAbs(dc) == 1) || (meuAbs(dl) == 1 && meuAbs(dc) == 2)) return 1;
            return 0;

        case 'B': // bispo
            if (meuAbs(dl) == meuAbs(dc) && caminhoLivre(li, ci, lf, cf)) return 1;
            return 0;

        case 'R': // rainha
            if (((dl == 0 || dc == 0) || (meuAbs(dl) == meuAbs(dc))) && caminhoLivre(li, ci, lf, cf)) return 1;
            return 0;

        case 'K': // rei
            if (meuAbs(dl) <= 1 && meuAbs(dc) <= 1) return 1;
            return 0;

        default:
            return 0;
    }
}

/* --------- FUNÇÃO PARA MOVIMENTAR PEÇA --------- */
int moverPeca(int li, int ci, int lf, int cf) {
    if (li < 0 || li > 7 || ci < 0 || ci > 7 || lf < 0 || lf > 7 || cf < 0 || cf > 7) {
        puts("Posição invalida!");
        return 0;
    }
    Pecas* p = tabuleiro[li][ci]->peca;
    if (!p) {
        puts("Não há peça na posição origem.");
        return 0;
    }
    if (p->cor != turnoAtual) {
        puts("Não é a vez dessa cor.");
        return 0;
    }
    if (!movimentoValido(p, li, ci, lf, cf)) {
        puts("Movimento invalido.");
        return 0;
    }

    // Teste se a jogada coloca o próprio rei em xeque
    Pecas* destino = tabuleiro[lf][cf]->peca;
    tabuleiro[lf][cf]->peca = p;
    tabuleiro[li][ci]->peca = NULL;

    if (emXeque(p->cor)) {
        tabuleiro[li][ci]->peca = p;
        tabuleiro[lf][cf]->peca = destino;
        puts("Jogada ilegal: seu rei ficaria em xeque.");
        return 0;
    }

    if (destino) free(destino);

    // Promoção do peão
    if (p->simbolo == 'P' && (lf == 0 || lf == 7)) {
        p->simbolo = 'R';
        printf("Peao promovido a Rainha!\n");
    }

    // Após a jogada, verificar xeque-mate
    char adversario = (p->cor == 'B') ? 'P' : 'B';
    if (emXequeMate(adversario)) {
        imprimirTabuleiro();
        printf("Xeque-mate! %s venceu!\n", (p->cor == 'B') ? jogadorBranco : jogadorPreto);
        corVencedor = p->cor;
        jogoEmAndamento = 0;
    } else if (emXeque(adversario)) {
        printf("%s esta em xeque!\n", (adversario == 'B') ? jogadorBranco : jogadorPreto);
    }

    return 1;
}


/* --------- AVALIAÇÃO PARA IA (HEURÍSTICA) --------- */
int valorPeca(char simbolo) {
    switch(simbolo) {
        case 'P': return 1;
        case 'C': return 3;
        case 'B': return 3;
        case 'T': return 5;
        case 'R': return 9;
        case 'K': return 1000;
        default:  return 0;
    }
}

int avaliarTabuleiro(char corComputador) {
    int soma = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Pecas* p = tabuleiro[i][j]->peca;
            if (!p) continue;

            int val = valorPeca(p->simbolo);
            int bonus = 0;

            if (p->cor == corComputador) {
                // Menor bônus para peões avançados (máx 2)
                if (p->simbolo == 'P') {
                    bonus += ((p->cor == 'P') ? i : (7 - i)) >= 3 ? 2 : 1;
                }

                // Maior incentivo para desenvolver cavalos, bispos e rainha
                if (p->simbolo == 'C' || p->simbolo == 'B') {
                    if ((p->cor == 'P' && i > 1) || (p->cor == 'B' && i < 6)) {
                        bonus += 2;
                    }
                }

                // Leve bônus para a rainha fora da base
                if (p->simbolo == 'R' && ((p->cor == 'P' && i > 1) || (p->cor == 'B' && i < 6))) {
                    bonus += 1;
                }
            }

            soma += (p->cor == corComputador) ? (val + bonus) : -(val);
        }
    }
    return soma;
}


/* --------- FUNÇÃO DA IA (JOGADA COMPUTADOR) --------- */
void jogadaComputador(char corComputador) {
    int melhorVal = -1000000;
    int melhorLi = -1, melhorCi = -1, melhorLf = -1, melhorCf = -1;

    for (int li=0; li<8; li++) {
        for (int ci=0; ci<8; ci++) {
            Pecas* p = tabuleiro[li][ci]->peca;
            if (!p || p->cor != corComputador) continue;

            for (int lf=0; lf<8; lf++) {
                for (int cf=0; cf<8; cf++) {
                    if (movimentoValido(p, li, ci, lf, cf)) {
                        Casa* origem = tabuleiro[li][ci];
                        Casa* destino = tabuleiro[lf][cf];
                        Pecas* capturada = destino->peca;

                        // Faz a jogada
                        destino->peca = p;
                        origem->peca = NULL;

                        // Só considera jogada que não deixa em xeque (não implementado, assume falso)
                        if (!emXeque(corComputador)) {
                            int val = avaliarTabuleiro(corComputador);
                            if (val > melhorVal) {
                                melhorVal = val;
                                melhorLi = li; melhorCi = ci; melhorLf = lf; melhorCf = cf;
                            }
                        }

                        // Desfaz jogada
                        origem->peca = p;
                        destino->peca = capturada;
                    }
                }
            }
        }
    }

    if (melhorLi != -1) {
        printf("Computador move: %c%d para %c%d\n",
               'a' + melhorCi, 8 - melhorLi,
               'a' + melhorCf, 8 - melhorLf);
        moverPeca(melhorLi, melhorCi, melhorLf, melhorCf);
    } else {
        printf("Computador não tem jogadas validas!\n");
    }
}

/* --------- FUNÇÃO SALVAR PARTIDA ------------- */
void salvarPartida() {
    FILE* f = fopen("partida_salva.txt", "w");
    if (!f) {
        puts("Erro ao salvar partida.");
        return;
    }
    fprintf(f, "%c\n", turnoAtual);
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            Pecas* p = tabuleiro[i][j]->peca;
            if (p) fprintf(f, "%c %c\n", p->simbolo, p->cor);
            else fprintf(f, "- -\n");
        }
    }
    fclose(f);
    puts("Partida salva.\n");
}

/* --------- FUNÇÃO RETOMAR PARTIDA ----------- */
int retomarPartida() {
    FILE* f = fopen("partida_salva.txt", "r");
    if (!f) {
        puts("Nenhuma partida salva encontrada.\n");
        return 0;
    }
    char linha[32];
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }
    turnoAtual = linha[0];
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            char sim, cor;
            if (fscanf(f, " %c %c\n", &sim, &cor) != 2) {
                fclose(f);
                return 0;
            }
            if (tabuleiro[i][j]->peca) {
                free(tabuleiro[i][j]->peca);
                tabuleiro[i][j]->peca = NULL;
            }
            if (sim != '-') {
                tabuleiro[i][j]->peca = criarPeca(sim, cor);
            }
        }
    }
    fclose(f);
    puts("Partida retomada.\n");
    return 1;
}


/* --------- FUNÇÃO XEQUE/XEQUE-MATE ----------- */

int emXeque(char cor) {
    int reiLi = -1, reiCi = -1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Pecas* p = tabuleiro[i][j]->peca;
            if (p && p->simbolo == 'K' && p->cor == cor) {
                reiLi = i;
                reiCi = j;
                break;
            }
        }
    }

    if (reiLi == -1 || reiCi == -1) return 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Pecas* p = tabuleiro[i][j]->peca;
            if (p && p->cor != cor) {
                if (movimentoValido(p, i, j, reiLi, reiCi)) return 1;
            }
        }
    }

    return 0;
}

int emXequeMate(char cor) {
    if (!emXeque(cor)) return 0;

    for (int li = 0; li < 8; li++) {
        for (int ci = 0; ci < 8; ci++) {
            Pecas* p = tabuleiro[li][ci]->peca;
            if (!p || p->cor != cor) continue;

            for (int lf = 0; lf < 8; lf++) {
                for (int cf = 0; cf < 8; cf++) {
                    if (movimentoValido(p, li, ci, lf, cf)) {
                        Pecas* capturada = tabuleiro[lf][cf]->peca;
                        tabuleiro[lf][cf]->peca = p;
                        tabuleiro[li][ci]->peca = NULL;

                        int aindaEmXeque = emXeque(cor);

                        tabuleiro[li][ci]->peca = p;
                        tabuleiro[lf][cf]->peca = capturada;

                        if (!aindaEmXeque) return 0;
                    }
                }
            }
        }
    }

    return 1;
}


/* ---------- LOOP DA PARTIDA -------------- */
void loopPartida(Usuario* usuario1, int idx1, Usuario* usuario2, int idx2) {
    char entrada[32], origem[3], destino[3];
    jogoEmAndamento = 1; 
    corVencedor = 0;
    turnoAtual = 'B';

    while (jogoEmAndamento) {
        imprimirTabuleiro();
        printf("Turno: %s (%c)\n", (turnoAtual == 'B' ? jogadorBranco : jogadorPreto), turnoAtual);

        if (turnoAtual == 'P') {  // IA joga sempre com peças pretas
            jogadaComputador('P');
            turnoAtual = 'B';
            continue;
        }

        // Jogador humano faz o movimento:
        printf("Digite movimento (ex: a2 a3), ou 'salvar', 'retomar', 'sair': ");
        fgets(entrada, sizeof(entrada), stdin);
        entrada[strcspn(entrada, "\n")] = 0;

        if (strcmp(entrada, "sair") == 0) {
            jogoEmAndamento = 0;

            // Atualiza ranking como empate ao sair, se ambos estiverem logados
            if (usaLogin && idx1 != -1 && idx2 != -1) {
                    Usuario usuarios[256];
                    int n = lerUsuarios(usuarios, 256);
                    atualizarRanking(usuarios, n, idx1, idx2, 1);
                }

            break;
        } else if (strcmp(entrada, "salvar") == 0) {
            salvarPartida();
            continue;
        } else if (strcmp(entrada, "retomar") == 0) {
            if (!retomarPartida()) {
                puts("Falha ao retomar partida.");
            }
            continue;
        }

        if (sscanf(entrada, "%2s %2s", origem, destino) != 2) {
            puts("Entrada invalida.");
            continue;
        }
        int li = 8 - (origem[1] - '0');
        int ci = origem[0] - 'a';
        int lf = 8 - (destino[1] - '0');
        int cf = destino[0] - 'a';

        if (moverPeca(li, ci, lf, cf)) {
            turnoAtual = 'P';  // passa para IA
        }
    }

    // Após fim da partida, atualizar ranking se houve vencedor
    if (usaLogin && corVencedor && idx1 != -1 && idx2 != -1) {
        Usuario usuarios[256];
        int n = lerUsuarios(usuarios, 256);
        if (corVencedor == 'B') {
            atualizarRanking(usuarios, n, idx1, idx2, 0); // branco venceu
        } else {
            atualizarRanking(usuarios, n, idx2, idx1, 0); // preto venceu
        }
    }

    printf("Jogo finalizado.\n");
}

   

/* ---------- FUNÇÃO PRINCIPAL ---------- */
int main() {
    setbuf(stdout, NULL);

    Usuario usuarios[256];
    int nUsuarios;
    Usuario* usuario1 = NULL;
    Usuario* usuario2 = NULL;
    int idx1 = -1, idx2 = -1;
    int opcao;

    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Jogar nova partida\n");
        printf("2. Retomar partida salva\n");
        printf("3. Ver ranking\n");
        printf("4. Cadastrar novo usuario\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // limpar '\n'

        switch (opcao) {
            case 1: {
                inicializarTabuleiro();
                printf("Deseja jogar com login? (s/n): ");
                char loginResp = getchar();
                getchar(); // limpar '\n'

                nUsuarios = lerUsuarios(usuarios, 256);
                if (loginResp == 's' || loginResp == 'S') {
                    usaLogin = 1;

                    printf("Digite nome do jogador Branco: ");
                    char nome1[32];
                    fgets(nome1, sizeof(nome1), stdin);
                    nome1[strcspn(nome1, "\n")] = 0;
                    idx1 = buscarUsuario(usuarios, nUsuarios, nome1);
                    if (idx1 == -1) {
                        printf("Usuario não encontrado, jogando como anonimo.\n");
                    } else {
                        usuario1 = &usuarios[idx1];
                        strcpy(jogadorBranco, usuario1->nome);
                    }

                    printf("Digite nome do jogador Preto: ");
                    char nome2[32];
                    fgets(nome2, sizeof(nome2), stdin);
                    nome2[strcspn(nome2, "\n")] = 0;
                    idx2 = buscarUsuario(usuarios, nUsuarios, nome2);
                    if (idx2 == -1) {
                        printf("Usuario não encontrado, jogando como anonimo.\n");
                    } else {
                        usuario2 = &usuarios[idx2];
                        strcpy(jogadorPreto, usuario2->nome);
                    }
                } else {
                    strcpy(jogadorBranco, "Branco");
                    strcpy(jogadorPreto, "Preto");
                }

                loopPartida(usuario1, idx1, usuario2, idx2);
                break;
            }

            case 2: {
                inicializarTabuleiro();
                if (retomarPartida()) {
                    loopPartida(usuario1, idx1, usuario2, idx2);
                } else {
                    printf("Não foi possivel retomar partida salva.\n");
                }
                break;
            }

            case 3:
                mostrarRanking();
                break;

            case 4:
                criarCadastro();
                break;

            case 5:
                printf("Saindo... Ate logo!\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
        }

    } while (opcao != 5);

    liberarTabuleiro();
    return 0;
}
