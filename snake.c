#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string.h>

/**
 * CONFIGURAÇÕES E DEFINIÇÕES
 * curWidth/curHeight: Dimensões atuais do mapa (expansíveis)
 * MAX_FRUITS: Quantidade de frutas simultâneas no mapa
 * MAX_OBSTACLES: Limite de obstáculos no modo Hardcore
 * QUEUE_SIZE: Tamanho do buffer de movimentos para fluidez
 */
int curWidth = 60;
int curHeight = 22;
#define MAX_FRUITS 8
#define MAX_OBSTACLES 1000
#define QUEUE_SIZE 3

// Definições de códigos de cores para o terminal Windows
#define RED 12
#define GREEN 10
#define BLUE 9
#define YELLOW 14
#define MAGENTA 13
#define WHITE 15
#define GRAY 8
#define CYAN 11

/**
 * ESTRUTURAS DE DADOS
 */
typedef struct { int dx; int dy; } Move; // Estrutura para armazenar vetores de movimento

typedef struct Node { // Nó da lista encadeada que representa o corpo da cobra
    int x; int y;
    struct Node *next;
} Node;

typedef struct { // Estrutura para as frutas
    int x; int y; int type; int points;
} Fruit;

typedef struct { // Estrutura para itens especiais (estrelas)
    int x; int y; int active;
} SpecialItem;

typedef struct { // Estrutura para obstáculos fixos
    int x; int y;
} Obstacle;

// VARIÁVEIS GLOBAIS
Node *head = NULL, *tail = NULL;
Fruit fruits[MAX_FRUITS];
Obstacle obstacles[MAX_OBSTACLES];
SpecialItem starGold = {0, 0, 0};
SpecialItem starWhite = {0, 0, 0};

char playerName[20] = "Player";
int dirX = 1, dirY = 0;
Move moveQueue[QUEUE_SIZE];
int queueCount = 0;

int score = 0, highscore = 0, lastScore = 0;
int gameOver = 0, gameSpeed = 100, level = 1;
int nextLevelThreshold = 100;
int gameMode = 0, obstacleCount = 0;
time_t startTime, lastGoldTime, lastWhiteTime;

int lastTailX = -1, lastTailY = -1;

/* ========= UTILITÁRIOS ========= */

/**
 * setColor: Altera a cor do texto no terminal.
 * @param color: Código da cor (definido nos macros acima).
 * Uso: Chame antes de um printf para colorir o caractere seguinte.
 */
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}

/**
 * gotoxy: Move o cursor do terminal para uma coordenada (x, y) específica.
 * @param x: Coluna.
 * @param y: Linha.
 * Uso: Essencial para desenhar elementos em partes específicas da tela sem limpar tudo.
 */
void gotoxy(int x, int y) {
    COORD coord = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/**
 * hideCursor: Oculta o cursor piscante do terminal Windows.
 * Uso: Chamado uma vez no main para evitar distrações visuais durante o jogo.
 */
void hideCursor() {
    CONSOLE_CURSOR_INFO info = { 100, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

/**
 * is_occupied: Verifica se uma coordenada (x, y) já possui algum objeto (cobra, fruta, etc).
 * @return 1 se ocupado, 0 se livre.
 * Uso: Utilizado pelo gerador de frutas e estrelas para evitar sobreposição de itens.
 */
int is_occupied(int x, int y) {
    Node *temp = head;
    while(temp) { if(temp->x == x && temp->y == y) return 1; temp = temp->next; }
    for(int i=0; i<MAX_FRUITS; i++) if(fruits[i].x == x && fruits[i].y == y) return 1;
    for(int i=0; i<obstacleCount; i++) if(obstacles[i].x == x && obstacles[i].y == y) return 1;
    if(starGold.active && starGold.x == x && starGold.y == y) return 1;
    if(starWhite.active && starWhite.x == x && starWhite.y == y) return 1;
    return 0;
}

/**
 * generate_fruit: Cria uma nova fruta em uma posição aleatória livre.
 * @param i: Índice da fruta no array global 'fruits'.
 * Uso: Chamado no início do jogo e toda vez que uma fruta é comida.
 */
void generate_fruit(int i) {
    int nx, ny, timeout = 0;
    do {
        nx = rand() % (curWidth - 2) + 1;
        ny = rand() % (curHeight - 2) + 1;
        timeout++;
    } while(is_occupied(nx, ny) && timeout < 100);
    fruits[i].x = nx; fruits[i].y = ny;
    fruits[i].type = rand() % 8;
    fruits[i].points = (fruits[i].type + 1) * 5;
    gotoxy(nx, ny);
    switch(fruits[i].type) {
        case 0: setColor(RED); printf("@"); break;
        case 1: setColor(GREEN); printf("P"); break;
        case 2: setColor(BLUE); printf("B"); break;
        case 3: setColor(CYAN); printf("M"); break;
        case 4: setColor(YELLOW); printf("L"); break;
        case 5: setColor(MAGENTA); printf("U"); break;
        case 6: setColor(RED); printf("C"); break;
        case 7: setColor(GREEN); printf("l"); break;
    }
}

/**
 * spawn_star: Cria uma estrela bônus (dourada ou prata) no mapa.
 * @param s: Ponteiro para a estrutura SpecialItem.
 * @param color: Cor visual da estrela.
 * Uso: Chamado periodicamente pela lógica de tempo no loop principal.
 */
void spawn_star(SpecialItem *s, int color) {
    int nx, ny, timeout = 0;
    do {
        nx = rand() % (curWidth - 2) + 1;
        ny = rand() % (curHeight - 2) + 1;
        timeout++;
    } while(is_occupied(nx, ny) && timeout < 100);
    s->x = nx; s->y = ny; s->active = 1;
    gotoxy(nx, ny); setColor(color); printf("*");
}

/* ========= DESENHO E UI ========= */

/**
 * draw_static_ui: Desenha os elementos da interface que não mudam com frequência.
 * Uso: Chamado no início de cada nível ou após limpar a tela.
 */
void draw_static_ui() {
    int sx = curWidth + 4;
    setColor(WHITE);
    for(int i=0; i<=curHeight; i++) { gotoxy(sx-2, i); printf("%c", 186); }
    setColor(BLUE); gotoxy(sx, 1); printf("SNAKE THE GAME");
    setColor(GRAY); gotoxy(sx, 2); printf("-------------------");
    setColor(WHITE); gotoxy(sx, 4); printf("JOGADOR: "); setColor(YELLOW); printf("%s", playerName);
    setColor(GRAY);  gotoxy(sx, 13); printf("--- FRUTAS ---");
    setColor(RED);    gotoxy(sx, 14); printf("@ Maca");      setColor(GREEN);  gotoxy(sx+11, 14); printf("P Pera");
    setColor(BLUE);   gotoxy(sx, 15); printf("B Blueberry"); setColor(CYAN);   gotoxy(sx+11, 15); printf("M Melancia");
    setColor(YELLOW); gotoxy(sx, 16); printf("L Laranja");   setColor(MAGENTA); gotoxy(sx+11, 16); printf("U Uva");
    setColor(RED);    gotoxy(sx, 17); printf("C Cereja");    setColor(GREEN);  gotoxy(sx+11, 17); printf("l Limao");
    setColor(YELLOW); gotoxy(sx, 19); printf("* Ouro (60s)");
    setColor(WHITE);  gotoxy(sx, 20); printf("* Prata (30s)");
}

/**
 * update_dynamic_ui: Atualiza os dados variáveis da barra lateral (score, tempo, etc).
 * Uso: Chamado a cada frame do jogo para manter as informações atualizadas.
 */
void update_dynamic_ui() {
    int sx = curWidth + 4;
    gotoxy(sx + 9, 5); setColor(GREEN);   printf("%-10d", score);
    gotoxy(sx + 9, 6); setColor(WHITE);   printf("%-10d", highscore);
    gotoxy(sx + 9, 8); setColor(MAGENTA); printf("%-10d", level);
    gotoxy(sx + 9, 9); setColor(CYAN);    printf("%-10s", gameMode==1?"Hardcore":"Classico");
    int elapsed = (int)difftime(time(NULL), startTime);
    gotoxy(sx + 9, 11); setColor(BLUE);   printf("%02d:%02d", elapsed/60, elapsed%60);
}

/**
 * draw_border: Desenha as paredes externas do campo de jogo.
 * Uso: Chamado no início do jogo e sempre que o mapa expande.
 */
void draw_border() {
    setColor(GRAY);
    for(int i=0; i<curWidth; i++) { gotoxy(i, 0); printf("%c", 219); gotoxy(i, curHeight); printf("%c", 219); }
    for(int i=0; i<=curHeight; i++) { gotoxy(0, i); printf("%c", 219); gotoxy(curWidth - 1, i); printf("%c", 219); }
    draw_static_ui();
}

/**
 * draw_snake: Desenha a cabeça e o pescoço da cobra, e apaga o rastro da cauda.
 * Uso: Chamado a cada movimento. É otimizado para não redesenhar o corpo inteiro.
 */
void draw_snake() {
    if(lastTailX != -1) { gotoxy(lastTailX, lastTailY); printf(" "); }
    setColor(GREEN);
    if(head) {
        gotoxy(head->x, head->y); printf("O"); // Desenha cabeça
        if(head->next) { gotoxy(head->next->x, head->next->y); printf("%c", 254); } // Desenha pescoço
    }
}

/**
 * draw_all_fruits_and_stars: Redesenha todos os itens coletáveis e obstáculos.
 * Uso: Chamado após limpar a tela (quando sobe de nível) para restaurar os itens.
 */
void draw_all_fruits_and_stars() {
    for(int i=0; i<MAX_FRUITS; i++) {
        if(fruits[i].x == -1) continue;
        gotoxy(fruits[i].x, fruits[i].y);
        switch(fruits[i].type) {
            case 0: setColor(RED); printf("@"); break;
            case 1: setColor(GREEN); printf("P"); break;
            case 2: setColor(BLUE); printf("B"); break;
            case 3: setColor(CYAN); printf("M"); break;
            case 4: setColor(YELLOW); printf("L"); break;
            case 5: setColor(MAGENTA); printf("U"); break;
            case 6: setColor(RED); printf("C"); break;
            case 7: setColor(GREEN); printf("l"); break;
        }
    }
    if(starGold.active) { setColor(YELLOW); gotoxy(starGold.x, starGold.y); printf("*"); }
    if(starWhite.active) { setColor(WHITE); gotoxy(starWhite.x, starWhite.y); printf("*"); }
    setColor(GRAY);
    for(int i=0; i<obstacleCount; i++) { gotoxy(obstacles[i].x, obstacles[i].y); printf("X"); }
}

/* ========= LOGICA DO JOGO ========= */

/**
 * reset_game_data: Limpa listas, zera pontuação e prepara variáveis para um novo jogo.
 * Uso: Chamado antes de cada partida para garantir que dados antigos sejam removidos.
 */
void reset_game_data() {
    Node *temp; while(head) { temp = head; head = head->next; free(temp); }
    tail = NULL; score = 0; level = 1; nextLevelThreshold = 100;
    dirX = 1; dirY = 0; queueCount = 0;
    gameOver = 0; obstacleCount = 0;
    starGold.active = starWhite.active = 0;
    curWidth = 60; curHeight = 22;
    startTime = time(NULL); lastGoldTime = startTime; lastWhiteTime = startTime;
    for(int i=0; i<MAX_FRUITS; i++) { fruits[i].x = -1; fruits[i].y = -1; }
    Node *start = malloc(sizeof(Node)); start->x = curWidth/2; start->y = curHeight/2; start->next = NULL;
    head = tail = start;
}

/**
 * start_screen: Gerencia a tela inicial, coleta o nome do jogador e opções de jogo.
 * Uso: Ponto de entrada visual de cada nova sessão.
 */
void start_screen() {
    system("cls");
    setColor(BLUE);
    printf("\n    #################################################\n");
    printf("    #                SNAKE THE GAME                 #\n");
    printf("    #################################################\n");
    setColor(WHITE);
    printf("\n    NOME ATUAL: %s", playerName);
    printf("\n    NOVO NOME (Deixe vazio para manter): ");
    char inputName[20];
    if(fgets(inputName, 20, stdin) && inputName[0] != '\n') {
        inputName[strcspn(inputName, "\n")] = 0;
        strcpy(playerName, inputName);
    }
    if(lastScore > 0) { printf("\n    ULTIMA PONTUACAO: "); setColor(YELLOW); printf("%d\n", lastScore); }
    setColor(CYAN);
    printf("\n    MODO: 1-CLASSICO  2-OBSTACULOS: ");
    char c = _getch(); gameMode = (c == '2') ? 1 : 0;
    setColor(YELLOW);
    printf("\n    DIFICULDADE: 1-FACIL  2-MEDIO  3-DIFICIL: ");
    c = _getch();
    if(c == '1') gameSpeed = 130; else if(c == '3') gameSpeed = 45; else gameSpeed = 85;
    reset_game_data(); system("cls");
    draw_border();
    for(int i=0; i<MAX_FRUITS; i++) generate_fruit(i);
}

/**
 * update: Processa a lógica principal de movimento, colisão e itens.
 * Uso: Chamado periodicamente. Move a cabeça, verifica se comeu fruta e decide se remove a cauda.
 */
void update() {
    // Processa a fila de comandos (Input Buffer) para curvas rápidas
    if(queueCount > 0) {
        dirX = moveQueue[0].dx;
        dirY = moveQueue[0].dy;
        for(int i = 0; i < queueCount - 1; i++) moveQueue[i] = moveQueue[i+1];
        queueCount--;
    }

    int nX = head->x + dirX, nY = head->y + dirY;
    
    // Verifica colisões com bordas e corpo
    if(nX <= 0 || nX >= curWidth-1 || nY <= 0 || nY >= curHeight) { gameOver = 1; return; }
    Node *t = head; while(t) { if(t->x == nX && t->y == nY) { gameOver = 1; return; } t = t->next; }
    for(int i=0; i<obstacleCount; i++) if(obstacles[i].x == nX && obstacles[i].y == nY) { gameOver = 1; return; }

    // Coleta de itens especiais
    if(starGold.active && nX == starGold.x && nY == starGold.y) { score += 50 * level; starGold.active = 0; }
    if(starWhite.active && nX == starWhite.x && nY == starWhite.y) { score += 25 * level; starWhite.active = 0; }

    // Insere novo nó (movimento da cabeça)
    Node *newNode = malloc(sizeof(Node)); newNode->x = nX; newNode->y = nY; newNode->next = head; head = newNode;

    // Lógica de comer fruta
    int fIdx = -1;
    for(int i=0; i<MAX_FRUITS; i++) if(fruits[i].x == nX && fruits[i].y == nY) fIdx = i;

    if(fIdx != -1) {
        score += fruits[fIdx].points * level; 
        if(gameMode == 1 && obstacleCount < MAX_OBSTACLES) { 
            obstacles[obstacleCount].x = tail->x; obstacles[obstacleCount].y = tail->y; obstacleCount++; 
        }
        
        // Verificação de Level Up e Expansão do Mapa
        if(score >= nextLevelThreshold) {
            level++; score += 100; nextLevelThreshold += level * 100;
            if(gameSpeed > 30) gameSpeed -= 5;
            curWidth += 5; curHeight += 2;
            system("cls"); draw_border(); draw_all_fruits_and_stars();
        }
        generate_fruit(fIdx); lastTailX = -1; // Não remove cauda se comeu
    } else {
        // Movimento normal: remove o último nó (cauda)
        Node *temp = head; while(temp->next != tail) temp = temp->next;
        lastTailX = tail->x; lastTailY = tail->y;
        free(tail); tail = temp; tail->next = NULL;
    }

    // Cronometragem para spawn de estrelas
    time_t now = time(NULL);
    if(!starGold.active && difftime(now, lastGoldTime) >= 60) { spawn_star(&starGold, YELLOW); lastGoldTime = now; }
    if(!starWhite.active && difftime(now, lastWhiteTime) >= 30) { spawn_star(&starWhite, WHITE); lastWhiteTime = now; }
}

/**
 * input: Captura as teclas pressionadas e adiciona à fila de movimentos.
 * Uso: Chamado em alta frequência no loop principal para capturar comandos instantaneamente.
 */
void input() {
    if(_kbhit()) {
        int k = _getch(); if(k == 0 || k == 224) k = _getch();
        int dx = 0, dy = 0;
        switch(k) {
            case 72: case 'w': case 'W': dx = 0; dy = -1; break;
            case 80: case 's': case 'S': dx = 0; dy = 1; break;
            case 75: case 'a': case 'A': dx = -1; dy = 0; break;
            case 77: case 'd': case 'D': dx = 1; dy = 0; break;
            default: return;
        }

        // Validação para não permitir virar 180 graus (mesmo em sequência rápida)
        int lastDX = (queueCount > 0) ? moveQueue[queueCount-1].dx : dirX;
        int lastDY = (queueCount > 0) ? moveQueue[queueCount-1].dy : dirY;

        if(queueCount < QUEUE_SIZE) {
            if((dx != 0 && dx != -lastDX) || (dy != 0 && dy != -lastDY)) {
                moveQueue[queueCount].dx = dx;
                moveQueue[queueCount].dy = dy;
                queueCount++;
            }
        }
    }
}

/**
 * main: Ponto de entrada do programa. Configura o jogo e gerencia o loop principal.
 */
int main() {
    srand((unsigned int)time(NULL)); 
    hideCursor();
    
    while(1) { // Loop de sessões de jogo
        start_screen();
        DWORD lastTick = GetTickCount();
        double moveAccumulator = 0;
        
        while(!gameOver) { // Loop do jogo (partida)
            DWORD currentTick = GetTickCount();
            DWORD deltaTime = currentTick - lastTick;
            lastTick = currentTick;
            
            input(); // Polling de input em alta frequência
            moveAccumulator += (double)deltaTime;
            
            // Lógica de tempo para compensar velocidade vertical
            int targetDelay = (dirY != 0) ? (int)(gameSpeed * 1.6) : gameSpeed;
            
            if(moveAccumulator >= (double)targetDelay) {
                update();
                draw_snake();
                update_dynamic_ui();
                moveAccumulator -= (double)targetDelay; 
            }
            Sleep(1); // Latência mínima para manter CPU estável e resposta rápida
        }
        
        // Tela de Game Over básica
        lastScore = score; if(score > highscore) highscore = score;
        setColor(RED); gotoxy(curWidth/2 - 5, curHeight/2); printf("GAME OVER!"); 
        Sleep(2000);
    }
    return 0;
}
