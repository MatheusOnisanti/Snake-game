#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

/* ========================= DIMENSOES ========================= */

#define WIDTH       40
#define HEIGHT      26
#define TOP          3

#define ARENA_LEFT   1
#define ARENA_RIGHT (ARENA_LEFT + WIDTH*2 + 1)

#define PANEL_X     (ARENA_RIGHT + 3)
#define PANEL_W     24

#define CON_COLS    (PANEL_X + PANEL_W + 2)
#define CON_ROWS    35

/* ========================= JOGO ========================= */

#define FRUIT_COUNT     6
#define MAX_SNAKE     800
#define MAX_RANKING     5
#define RANKING_FILE  "ranking.txt"
#define MAX_OBSTACLES   400
#define INITIAL_OBSTACLES 20

#define INITIAL_SPEED  140
#define MIN_SPEED       50
#define LEVEL_STEP      80

/* ========================= BFS ========================= */

#define BFS_MAX     (WIDTH * HEIGHT)

typedef struct { int x, y; } BfsNode;

BfsNode bfs_queue[BFS_MAX];
int     bfs_front = 0;
int     bfs_rear  = 0;

int     bfs_visited[WIDTH + 1][HEIGHT + 1];
BfsNode bfs_parent[WIDTH + 1][HEIGHT + 1];

/* Caminho encontrado pela BFS */
BfsNode bfs_path[BFS_MAX];
int     bfs_path_len = 0;

/* Celulas visitadas pela BFS (para visualizacao) */
BfsNode bfs_visited_cells[BFS_MAX];
int     bfs_visited_count = 0;

/* Direcoes: baixo, cima, direita, esquerda */
int dx[4] = { 1, -1, 0,  0 };
int dy[4] = { 0,  0, 1, -1 };

/* ========================= UNICODE / EMOJI ========================= */

/* Cobra */
#define SYM_HEAD        "\xF0\x9F\x9F\xA2"
#define SYM_HEAD_AUTO   "\xF0\x9F\xA4\x96"
#define SYM_BODY        "\xF0\x9F\x9F\xA9"
#define SYM_TAIL        "\xF0\x9F\x9F\xA8"
#define SYM_BODY_FAST   "\xF0\x9F\x94\xB5"

/* Frutas */
#define SYM_APPLE       "\xF0\x9F\x8D\x8E"
#define SYM_GRAPE       "\xF0\x9F\x8D\x87"
#define SYM_CHERRY      "\xF0\x9F\x8D\x92"
#define SYM_STAR        "\xE2\xAD\x90"
#define SYM_GEM         "\xF0\x9F\x92\x8E"
#define SYM_RAINBOW     "\xF0\x9F\x8C\x88"

/* HUD / efeitos */
#define SYM_PAUSE       "\xE2\x8F\xB8"
#define SYM_SKULL       "\xF0\x9F\x92\x80"
#define SYM_TROPHY      "\xF0\x9F\x8F\x86"
#define SYM_SPEED_SLOW  "\xF0\x9F\x90\xA2"
#define SYM_SPEED_MED   "\xF0\x9F\x9A\xB6"
#define SYM_SPEED_FAST  "\xF0\x9F\x9A\x80"
#define SYM_SPEED_INS   "\xE2\x9A\xA1"
#define SYM_SNAKE_TITLE "\xF0\x9F\x90\x8D"
#define SYM_CROWN       "\xF0\x9F\x91\x91"
#define SYM_CLOCK       "\xE2\x8F\xB1"
#define SYM_CTRL        "\xF0\x9F\x8E\xAE"
#define SYM_FIRE        "\xF0\x9F\x94\xA5"
#define SYM_SNAKE_EAT   "\xF0\x9F\x8D\xBD"
#define SYM_HEART       "\xE2\x9D\xA4"
#define SYM_ROBOT       "\xF0\x9F\xA4\x96"
#define SYM_BRAIN       "\xF0\x9F\xA7\xA0"
#define SYM_PATH        "\xC2\xB7"
#define SYM_OBSTACLE    "\xE2\x96\xA0"

/* Bordas */
#define BOX_TL   "\xE2\x94\x8C"
#define BOX_TR   "\xE2\x94\x90"
#define BOX_BL   "\xE2\x94\x94"
#define BOX_BR   "\xE2\x94\x98"
#define BOX_H    "\xE2\x94\x80"
#define BOX_V    "\xE2\x94\x82"
#define BOX_TLH  "\xE2\x95\x94"
#define BOX_TRH  "\xE2\x95\x97"
#define BOX_BLH  "\xE2\x95\x9A"
#define BOX_BRH  "\xE2\x95\x9D"
#define BOX_DH   "\xE2\x95\x90"
#define BOX_DV   "\xE2\x95\x91"
#define BOX_LM   "\xE2\x95\xA0"
#define BOX_RM   "\xE2\x95\xA3"
#define BLOCK    "\xE2\x96\x93"
#define DOT_BG   "\xC2\xB7"

/* Barra de progresso */
#define BAR_FULL  "\xE2\x96\x88"
#define BAR_HALF  "\xE2\x96\x91"

/* ========================= CORES ========================= */
#define C_RESET      7
#define C_BORDER     11
#define C_PANEL      11
#define C_PANEL_HDR  14
#define C_PANEL_VAL  15
#define C_SCORE      14
#define C_LEVEL      10
#define C_TIME       13
#define C_SPEED      12
#define C_RECORD     10
#define C_RUNNING    10
#define C_PAUSED     12
#define C_BG         8
#define C_TITLE      14
#define C_SELECT     14
#define C_NORMAL     7
#define C_FLASH      15
#define C_FLASH2     14
#define C_GAMEOVER   12
#define C_BEST       10
#define C_WALL       8
#define C_COMBO      12
#define C_BAR_FILL   10
#define C_BAR_EMPTY  8
#define C_BORDER_L1  11
#define C_BORDER_L2  10
#define C_BORDER_L3  14
#define C_BORDER_L4  12
#define C_BORDER_L5  13

/* Cores BFS */
#define C_BFS_PATH   9     /* azul claro */
#define C_BFS_VISIT  2     /* verde escuro */
#define C_OBSTACLE   4     /* vermelho escuro */
#define C_MODE_MAN   10    /* verde */
#define C_MODE_AUTO  9     /* azul claro */
#define C_BFS_INFO   3     /* ciano escuro */

/* ========================= TIPOS ========================= */

typedef struct { int x, y; } Segment;

typedef struct {
    int   x, y, value;
    const char *symbol;
    int   color, rarity;
} Fruit;

typedef struct {
    char name[32];
    int  score;
} RankEntry;

typedef enum {
    STATE_MENU = 0,
    STATE_PLAYING,
    STATE_RANKING,
    STATE_GAMEOVER,
    STATE_EXIT
} GameState;

typedef enum {
    DIR_UP = 0, DIR_RIGHT, DIR_DOWN, DIR_LEFT
} Direction;

/* ========================= GLOBAIS ========================= */

HANDLE hConsole;

Segment   snake[MAX_SNAKE];
Fruit     fruits[FRUIT_COUNT];
RankEntry ranking[MAX_RANKING];

/* Obstaculos */
Segment obstacles[MAX_OBSTACLES];
int     obstacleCount = 0;
int     obstacleType  = 0; /* 0: Nenhum, 1: Por fruta, 2: Fixo */
int     reverseMode   = 0; /* 0: Nao, 1: Sim (inverte ao comer) */

int snakeLen  = 0;
Direction dir     = DIR_RIGHT;
Direction nextDir = DIR_RIGHT;

int score         = 0;
int level         = 1;
int speedMs       = INITIAL_SPEED;
int paused        = 0;
int gameOver      = 0;
int fruitsEaten   = 0;
int comboCount    = 0;
int lastComboShow = -1;

int menuIndex      = 0;
int gameOverIndex  = 0;

static int menuNeedsRedraw     = 1;
static int menuLastIndex       = -1;
static int gameoverNeedsRedraw = 1;
static int gameoverLastIndex   = -1;
static int rankingNeedsRedraw  = 1;

time_t startTime;
time_t pauseStart;
int    pausedSeconds = 0;

static int lastElapsed  = -1;
static int lastLevel    = -1;
static int lastScore    = -1;
static int lastFruits   = -1;
static int lastCombo    = -1;

/* === Modo de jogo e BFS === */
int autoMode      = 0;   /* 0 = manual, 1 = automatico (BFS) */
int showBfsPath   = 1;   /* 1 = mostra caminho BFS no tabuleiro */
int showBfsVisit  = 0;   /* 1 = mostra celulas visitadas */

/* ========================= CONSOLE ========================= */

void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, c);
}

void set_color(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

void hide_cursor(void) {
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hConsole, &ci);
}

void show_cursor(void) {
    CONSOLE_CURSOR_INFO ci = { 20, TRUE };
    SetConsoleCursorInfo(hConsole, &ci);
}

void clear_screen(void) { system("cls"); }

void write_at(int x, int y, int color, const char *text) {
    gotoxy(x, y);
    set_color(color);
    printf("%s", text);
    set_color(C_RESET);
    fflush(stdout);
}

void fill_spaces(int x, int y, int n) {
    gotoxy(x, y);
    for (int i = 0; i < n; i++) putchar(' ');
    fflush(stdout);
}

void trim_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

/* ========================= CAIXAS UNICODE ========================= */

void draw_box_unicode(int left, int top, int right, int bottom, int color) {
    set_color(color);
    gotoxy(left, top);
    printf("%s", BOX_TL);
    for (int x = left + 1; x < right; x++) printf("%s", BOX_H);
    printf("%s", BOX_TR);
    for (int y = top + 1; y < bottom; y++) {
        gotoxy(left,  y); printf("%s", BOX_V);
        gotoxy(right, y); printf("%s", BOX_V);
    }
    gotoxy(left, bottom);
    printf("%s", BOX_BL);
    for (int x = left + 1; x < right; x++) printf("%s", BOX_H);
    printf("%s", BOX_BR);
    set_color(C_RESET);
    fflush(stdout);
}

void draw_box_double(int left, int top, int right, int bottom, int color) {
    set_color(color);
    gotoxy(left, top);
    printf("%s", BOX_TLH);
    for (int x = left + 1; x < right; x++) printf("%s", BOX_DH);
    printf("%s", BOX_TRH);
    for (int y = top + 1; y < bottom; y++) {
        gotoxy(left,  y); printf("%s", BOX_DV);
        gotoxy(right, y); printf("%s", BOX_DV);
    }
    gotoxy(left, bottom);
    printf("%s", BOX_BLH);
    for (int x = left + 1; x < right; x++) printf("%s", BOX_DH);
    printf("%s", BOX_BRH);
    set_color(C_RESET);
    fflush(stdout);
}

void draw_separator_double(int left, int y, int right, int color) {
    set_color(color);
    gotoxy(left, y);
    printf("%s", BOX_LM);
    for (int x = left + 1; x < right; x++) printf("%s", BOX_DH);
    printf("%s", BOX_RM);
    set_color(C_RESET);
    fflush(stdout);
}

void clear_inside_box(int left, int top, int right, int bottom) {
    for (int y = top + 1; y < bottom; y++) {
        gotoxy(left + 1, y);
        for (int x = left + 1; x < right; x++) putchar(' ');
    }
    fflush(stdout);
}

void print_centered_range(int left, int right, int y, int color, const char *text) {
    int width   = right - left - 1;
    int textlen = (int)strlen(text);
    int x = left + 1 + (width - textlen) / 2;
    if (x < left + 1) x = left + 1;
    write_at(x, y, color, text);
}

/* ========================= COR DE BORDA POR NIVEL ========================= */

int border_color_for_level(void) {
    switch (((level - 1) % 5)) {
        case 0: return C_BORDER_L1;
        case 1: return C_BORDER_L2;
        case 2: return C_BORDER_L3;
        case 3: return C_BORDER_L4;
        case 4: return C_BORDER_L5;
    }
    return C_BORDER_L1;
}

/* ========================= RANKING ========================= */

void load_ranking(int *count) {
    FILE *f = fopen(RANKING_FILE, "r");
    *count = 0;
    if (!f) return;
    while (*count < MAX_RANKING &&
           fscanf(f, "%31[^;];%d\n", ranking[*count].name, &ranking[*count].score) == 2)
        (*count)++;
    fclose(f);
}

void save_ranking(int count) {
    FILE *f = fopen(RANKING_FILE, "w");
    if (!f) return;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s;%d\n", ranking[i].name, ranking[i].score);
    fclose(f);
}

int best_score(void) {
    int count = 0;
    load_ranking(&count);
    return count ? ranking[0].score : 0;
}

void insert_ranking(const char *name, int sc) {
    int count = 0;
    load_ranking(&count);
    if (count < MAX_RANKING) {
        strncpy(ranking[count].name, name, 31);
        ranking[count].name[31] = '\0';
        ranking[count].score    = sc;
        count++;
    } else if (sc > ranking[count-1].score) {
        strncpy(ranking[count-1].name, name, 31);
        ranking[count-1].name[31] = '\0';
        ranking[count-1].score    = sc;
    } else return;

    for (int i = 0; i < count - 1; i++)
        for (int j = i + 1; j < count; j++)
            if (ranking[j].score > ranking[i].score) {
                RankEntry t = ranking[i]; ranking[i] = ranking[j]; ranking[j] = t;
            }

    if (count > MAX_RANKING) count = MAX_RANKING;
    save_ranking(count);
}

/* ========================= UTIL ========================= */

int is_on_snake(int x, int y) {
    for (int i = 0; i < snakeLen; i++)
        if (snake[i].x == x && snake[i].y == y) return 1;
    return 0;
}

int is_obstacle(int x, int y) {
    for (int i = 0; i < obstacleCount; i++)
        if (obstacles[i].x == x && obstacles[i].y == y) return 1;
    return 0;
}

int fruit_at(int x, int y) {
    for (int i = 0; i < FRUIT_COUNT; i++)
        if (fruits[i].x == x && fruits[i].y == y) return i;
    return -1;
}

static int arena_col(int logical_x) {
    return ARENA_LEFT + 1 + (logical_x - 1) * 2;
}

/* Verifica se celula e' livre para BFS (nao parede, nao cobra, nao obstaculo) */
int is_free_for_bfs(int x, int y) {
    if (x < 1 || x > WIDTH || y <= TOP || y >= HEIGHT) return 0;
    if (is_obstacle(x, y)) return 0;
    /* Corpo da cobra bloqueia (exceto a cabeca, que e' o ponto de partida) */
    for (int i = 1; i < snakeLen; i++)
        if (snake[i].x == x && snake[i].y == y) return 0;
    return 1;
}

/* ========================= BFS (conforme aula) ========================= */
/*
 * Implementacao identica ao slide "Algoritmo BFS Completo":
 *  - Fila com front/rear
 *  - visited[][] como hash espacial
 *  - parent[][] como lista encadeada implicita
 *  - dx[]/dy[] para adjacencia
 *
 * Retorna 1 se encontrou caminho, 0 caso contrario.
 * O caminho fica em bfs_path[] com bfs_path_len posicoes.
 */

void bfs_enqueue(int x, int y) {
    bfs_queue[bfs_rear].x = x;
    bfs_queue[bfs_rear].y = y;
    bfs_rear++;
}

BfsNode bfs_dequeue(void) {
    return bfs_queue[bfs_front++];
}

int run_bfs(int sx, int sy, int fx, int fy) {
    /* Limpa estruturas */
    bfs_front = 0;
    bfs_rear  = 0;
    bfs_path_len = 0;
    bfs_visited_count = 0;

    memset(bfs_visited, 0, sizeof(bfs_visited));

    /* Insere inicio na fila e marca como visitado */
    bfs_enqueue(sx, sy);
    bfs_visited[sx][sy] = 1;

    /* Registra celula visitada para visualizacao */
    bfs_visited_cells[bfs_visited_count].x = sx;
    bfs_visited_cells[bfs_visited_count].y = sy;
    bfs_visited_count++;

    while (bfs_front != bfs_rear) {
        BfsNode cur = bfs_dequeue();

        /* Chegou na fruta! */
        if (cur.x == fx && cur.y == fy) {
            /* Reconstroi caminho: segue parents de volta ate o inicio */
            BfsNode temp_path[BFS_MAX];
            int     temp_len = 0;
            BfsNode node = { fx, fy };

            while (!(node.x == sx && node.y == sy)) {
                temp_path[temp_len++] = node;
                node = bfs_parent[node.x][node.y];
            }

            /* Inverte o caminho (de cobra -> fruta) */
            bfs_path_len = 0;
            for (int i = temp_len - 1; i >= 0; i--) {
                bfs_path[bfs_path_len++] = temp_path[i];
            }

            return 1; /* caminho encontrado! */
        }

        /* Explora os 4 vizinhos */
        for (int i = 0; i < 4; i++) {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];

            /* Verificacoes em cascata (conforme slide):
             * 1. Dentro dos limites?
             * 2. Nao visitado?
             * 3. Celula livre (nao obstaculo, nao corpo)? */
            if (nx >= 1 && ny > TOP && nx <= WIDTH && ny < HEIGHT &&
                !bfs_visited[nx][ny] &&
                is_free_for_bfs(nx, ny)) {

                bfs_enqueue(nx, ny);
                bfs_visited[nx][ny] = 1;
                bfs_parent[nx][ny] = cur;

                /* Registra para visualizacao */
                if (bfs_visited_count < BFS_MAX) {
                    bfs_visited_cells[bfs_visited_count].x = nx;
                    bfs_visited_cells[bfs_visited_count].y = ny;
                    bfs_visited_count++;
                }
            }
        }
    }

    return 0; /* caminho nao existe */
}

/* Encontra a fruta mais proxima e calcula BFS ate ela */
int find_nearest_fruit_bfs(void) {
    int best_len = 999999;
    int best_idx = -1;
    BfsNode best_path[BFS_MAX];
    int     best_path_len = 0;
    BfsNode best_visited[BFS_MAX];
    int     best_visited_count = 0;

    for (int i = 0; i < FRUIT_COUNT; i++) {
        if (run_bfs(snake[0].x, snake[0].y, fruits[i].x, fruits[i].y)) {
            if (bfs_path_len < best_len) {
                best_len = bfs_path_len;
                best_idx = i;
                best_path_len = bfs_path_len;
                memcpy(best_path, bfs_path, sizeof(BfsNode) * bfs_path_len);
                best_visited_count = bfs_visited_count;
                memcpy(best_visited, bfs_visited_cells, sizeof(BfsNode) * bfs_visited_count);
            }
        }
    }

    if (best_idx >= 0) {
        bfs_path_len = best_path_len;
        memcpy(bfs_path, best_path, sizeof(BfsNode) * best_path_len);
        bfs_visited_count = best_visited_count;
        memcpy(best_visited, bfs_visited_cells, sizeof(BfsNode) * bfs_visited_count);
        return 1;
    }

    bfs_path_len = 0;
    bfs_visited_count = 0;
    return 0;
}

/* Verifica se uma posicao esta no caminho BFS */
int is_on_bfs_path(int x, int y) {
    for (int i = 0; i < bfs_path_len; i++)
        if (bfs_path[i].x == x && bfs_path[i].y == y) return 1;
    return 0;
}

/* Verifica se posicao foi visitada pelo BFS */
int is_bfs_visited(int x, int y) {
    for (int i = 0; i < bfs_visited_count; i++)
        if (bfs_visited_cells[i].x == x && bfs_visited_cells[i].y == y) return 1;
    return 0;
}

/* ========================= BARRA DE PROGRESSO ========================= */

void draw_progress_bar(int x, int y, int bar_w, int value, int maxval) {
    if (maxval <= 0) maxval = 1;
    int filled = (value * bar_w) / maxval;
    if (filled > bar_w) filled = bar_w;

    gotoxy(x, y);
    set_color(C_BAR_FILL);
    for (int i = 0; i < filled; i++) printf("%s", BAR_FULL);
    set_color(C_BAR_EMPTY);
    for (int i = filled; i < bar_w; i++) printf("%s", BAR_HALF);
    set_color(C_RESET);
    fflush(stdout);
}

/* ========================= OBSTACULOS ========================= */

void spawn_obstacles(void) {
    obstacleCount = 0;
    int mx = WIDTH / 2;
    int my = (TOP + HEIGHT) / 2;

    for (int i = 0; i < INITIAL_OBSTACLES && obstacleCount < INITIAL_OBSTACLES; i++) {
        int x, y, ok;
        int attempts = 0;
        do {
            ok = 1;
            x = rand() % WIDTH + 1;
            y = rand() % (HEIGHT - TOP - 1) + TOP + 1;

            /* Nao colocar perto do centro (onde a cobra inicia) */
            if (abs(x - mx) <= 5 && abs(y - my) <= 2) { ok = 0; continue; }

            if (is_on_snake(x, y)) { ok = 0; continue; }
            if (is_obstacle(x, y)) { ok = 0; continue; }

            attempts++;
            if (attempts > 200) break;
        } while (!ok);

        if (attempts <= 200) {
            obstacles[obstacleCount].x = x;
            obstacles[obstacleCount].y = y;
            obstacleCount++;
        }
    }
}

void draw_obstacle(int x, int y) {
    int col = arena_col(x);
    gotoxy(col, y);
    set_color(C_OBSTACLE);
    printf("%s ", SYM_OBSTACLE);
    set_color(C_RESET);
    fflush(stdout);
}

void draw_all_obstacles(void) {
    for (int i = 0; i < obstacleCount; i++)
        draw_obstacle(obstacles[i].x, obstacles[i].y);
}

/* ========================= FUNDO DA ARENA ========================= */

void draw_board_border(void) {
    int bc = border_color_for_level();
    set_color(bc);

    gotoxy(ARENA_LEFT, TOP);
    printf("%s", BOX_TL);
    for (int x = 0; x < WIDTH * 2; x++) printf("%s", BOX_H);
    printf("%s", BOX_TR);

    for (int y = TOP + 1; y < HEIGHT; y++) {
        gotoxy(ARENA_LEFT, y);
        printf("%s", BLOCK);
        gotoxy(ARENA_RIGHT, y);
        printf("%s", BLOCK);
    }

    gotoxy(ARENA_LEFT, HEIGHT);
    printf("%s", BOX_BL);
    for (int x = 0; x < WIDTH * 2; x++) printf("%s", BOX_H);
    printf("%s", BOX_BR);

    set_color(C_RESET);
    fflush(stdout);
}

void draw_board_background(void) {
    for (int y = TOP + 1; y < HEIGHT; y++) {
        gotoxy(ARENA_LEFT + 1, y);
        set_color(C_BG);
        for (int x = 0; x < WIDTH; x++) {
            if ((x + y) % 4 == 0)
                printf("%s ", DOT_BG);
            else
                printf("  ");
        }
    }
    set_color(C_RESET);
    fflush(stdout);
}

/* ========================= BFS VISUALIZATION ========================= */

/* Desenha as celulas visitadas pelo BFS (verde escuro) */
void draw_bfs_visited_overlay(void) {
    if (!showBfsVisit) return;
    for (int i = 0; i < bfs_visited_count; i++) {
        int x = bfs_visited_cells[i].x;
        int y = bfs_visited_cells[i].y;
        /* Nao desenhar sobre cobra, fruta ou obstaculo */
        if (is_on_snake(x, y)) continue;
        if (fruit_at(x, y) >= 0) continue;
        if (is_obstacle(x, y)) continue;
        if (is_on_bfs_path(x, y)) continue; /* caminho tem cor propria */

        int col = arena_col(x);
        gotoxy(col, y);
        set_color(C_BFS_VISIT);
        printf(". ");
        set_color(C_RESET);
    }
    fflush(stdout);
}

/* Desenha o caminho BFS (azul claro) */
void draw_bfs_path_overlay(void) {
    if (!showBfsPath) return;
    for (int i = 0; i < bfs_path_len; i++) {
        int x = bfs_path[i].x;
        int y = bfs_path[i].y;
        if (is_on_snake(x, y)) continue;
        if (fruit_at(x, y) >= 0) continue;
        if (is_obstacle(x, y)) continue;

        int col = arena_col(x);
        gotoxy(col, y);
        set_color(C_BFS_PATH);
        printf("%s ", SYM_PATH);
        set_color(C_RESET);
    }
    fflush(stdout);
}

/* Limpa celulas do caminho BFS antigo antes de redesenhar */
void clear_bfs_overlay(void) {
    for (int i = 0; i < bfs_visited_count; i++) {
        int x = bfs_visited_cells[i].x;
        int y = bfs_visited_cells[i].y;
        if (is_on_snake(x, y)) continue;
        if (fruit_at(x, y) >= 0) continue;
        if (is_obstacle(x, y)) continue;

        int col = arena_col(x);
        gotoxy(col, y);
        set_color(C_BG);
        if ((x + y) % 4 == 0)
            printf("%s ", DOT_BG);
        else
            printf("  ");
    }
    set_color(C_RESET);
    fflush(stdout);
}

/* ========================= HUD LATERAL ========================= */

void draw_side_panel_frame(void) {
    int L = PANEL_X;
    int R = PANEL_X + PANEL_W;

    draw_box_double(L, TOP, R, HEIGHT, C_PANEL);

    char title[48];
    snprintf(title, sizeof(title), "%s SNAKE BFS", SYM_BRAIN);
    print_centered_range(L, R, TOP + 1, C_TITLE, title);
    draw_separator_double(L, TOP + 2, R, C_PANEL);

    /* Cabecalhos fixos */
    write_at(L + 2, TOP + 3,  C_PANEL_HDR, "SCORE");
    write_at(L + 2, TOP + 5,  C_PANEL_HDR, "NIVEL");
    write_at(L + 2, TOP + 7,  C_PANEL_HDR, "SPEED");
    write_at(L + 2, TOP + 9,  C_PANEL_HDR, "TEMPO");
    write_at(L + 2, TOP + 11, C_PANEL_HDR, "RECORDE");

    draw_separator_double(L, TOP + 13, R, C_PANEL);

    /* Modo */
    write_at(L + 2, TOP + 14, C_PANEL_HDR, "MODO");

    draw_separator_double(L, TOP + 16, R, C_PANEL);

    /* BFS Info */
    char bfsline[32];
    snprintf(bfsline, sizeof(bfsline), "%s BFS INFO", SYM_BRAIN);
    write_at(L + 2, TOP + 17, C_PANEL_HDR, bfsline);

    draw_separator_double(L, TOP + 21, R, C_PANEL);

    /* Controles */
    char ctrlline[32];
    snprintf(ctrlline, sizeof(ctrlline), "%s CONTROLES", SYM_CTRL);
    write_at(L + 2, TOP + 22, C_PANEL_HDR, ctrlline);
}

const char *speed_icon(void) {
    if (speedMs > 110) return SYM_SPEED_SLOW;
    if (speedMs >  85) return SYM_SPEED_MED;
    if (speedMs >  65) return SYM_SPEED_FAST;
    return SYM_SPEED_INS;
}

const char *speed_label(void) {
    if (speedMs > 110) return "SLOW  ";
    if (speedMs >  85) return "NORMAL";
    if (speedMs >  65) return "FAST  ";
    return "INSANE";
}

void update_side_panel(void) {
    int L = PANEL_X;
    char buf[64];

    /* Score */
    if (lastScore != score) {
        snprintf(buf, sizeof(buf), "%06d", score);
        write_at(L + 2, TOP + 4, C_SCORE, buf);
        lastScore = score;
    }

    /* Nivel + barra */
    if (lastLevel != level) {
        snprintf(buf, sizeof(buf), "%d  ", level);
        write_at(L + 2, TOP + 6, C_LEVEL, buf);
        int levelBase  = (level - 1) * LEVEL_STEP;
        int levelNext  = level       * LEVEL_STEP;
        int progress   = score - levelBase;
        int needed     = levelNext - levelBase;
        draw_progress_bar(L + 2, TOP + 7, PANEL_W - 3, progress, needed);
        lastLevel = level;
    } else {
        int levelBase = (level - 1) * LEVEL_STEP;
        int levelNext = level       * LEVEL_STEP;
        int progress  = score - levelBase;
        int needed    = levelNext - levelBase;
        if (needed > 0) draw_progress_bar(L + 2, TOP + 7, PANEL_W - 3, progress, needed);
    }

    /* Velocidade */
    snprintf(buf, sizeof(buf), "%s %s", speed_icon(), speed_label());
    write_at(L + 2, TOP + 8, C_SPEED, buf);

    /* Tempo */
    int elapsed = (int)(time(NULL) - startTime) - pausedSeconds;
    if (paused) elapsed = (int)(pauseStart - startTime) - pausedSeconds;
    if (elapsed < 0) elapsed = 0;
    if (elapsed != lastElapsed) {
        snprintf(buf, sizeof(buf), "%02d:%02d", elapsed / 60, elapsed % 60);
        write_at(L + 2, TOP + 10, C_TIME, buf);
        lastElapsed = elapsed;
    }

    /* Recorde */
    int rec = best_score();
    snprintf(buf, sizeof(buf), "%06d", rec);
    write_at(L + 2, TOP + 12,
             (score > 0 && score >= rec) ? C_BEST : C_PANEL_VAL, buf);
    if (score > 0 && score >= rec)
        write_at(L + 10, TOP + 12, C_BEST, SYM_CROWN);
    else
        write_at(L + 10, TOP + 12, C_RESET, "  ");

    /* Modo */
    if (autoMode) {
        snprintf(buf, sizeof(buf), "%s AUTOMATICO  ", SYM_ROBOT);
        write_at(L + 2, TOP + 15, C_MODE_AUTO, buf);
    } else {
        snprintf(buf, sizeof(buf), "%s MANUAL      ", SYM_CTRL);
        write_at(L + 2, TOP + 15, C_MODE_MAN, buf);
    }

    /* BFS Info */
    snprintf(buf, sizeof(buf), "Caminho: %3d    ", bfs_path_len);
    write_at(L + 2, TOP + 18, C_BFS_INFO, buf);

    snprintf(buf, sizeof(buf), "Visitados: %3d  ", bfs_visited_count);
    write_at(L + 2, TOP + 19, C_BFS_INFO, buf);

    snprintf(buf, sizeof(buf), "Frutas: %d  ", fruitsEaten);
    write_at(L + 2, TOP + 20, C_PANEL_VAL, buf);

    /* Controles - atualizados */
    if (autoMode) {
        write_at(L + 2, TOP + 23, C_NORMAL, "T  Modo Manual  ");
    } else {
        write_at(L + 2, TOP + 23, C_NORMAL, "WASD  Mover     ");
    }
    write_at(L + 2, TOP + 24, C_NORMAL, "V  Caminho BFS  ");
    write_at(L + 2, TOP + 25, C_NORMAL, "G  Visitados BFS");
    write_at(L + 2, TOP + 26, C_NORMAL, "T  Trocar Modo  ");
    write_at(L + 2, TOP + 27, C_NORMAL, "P  Pausar       ");

    /* Combo */
    if (lastCombo != comboCount) {
        if (comboCount >= 3) {
            snprintf(buf, sizeof(buf), "%s x%d COMBO!  ", SYM_FIRE, comboCount);
            write_at(L + 2, TOP + 28, C_COMBO, buf);
        } else {
            fill_spaces(L + 2, TOP + 28, PANEL_W - 3);
        }
        lastCombo = comboCount;
    }

    /* Status pausado */
    if (paused) {
        snprintf(buf, sizeof(buf), "%s PAUSADO ", SYM_PAUSE);
        write_at(L + 2, TOP + 29, C_PAUSED, buf);
    } else {
        fill_spaces(L + 2, TOP + 29, 12);
    }
}

/* ========================= COBRA ========================= */

void draw_head(int x, int y) {
    gotoxy(arena_col(x), y);
    set_color(C_RESET);
    printf("%s", autoMode ? SYM_HEAD_AUTO : SYM_HEAD);
    fflush(stdout);
}

void draw_body(int x, int y) {
    gotoxy(arena_col(x), y);
    set_color(C_RESET);
    printf("%s", speedMs <= 65 ? SYM_BODY_FAST : SYM_BODY);
    fflush(stdout);
}

void draw_tail_seg(int x, int y) {
    gotoxy(arena_col(x), y);
    set_color(C_RESET);
    printf("%s", SYM_TAIL);
    fflush(stdout);
}

void erase_cell(int x, int y) {
    int col = arena_col(x);
    gotoxy(col, y);
    set_color(C_BG);
    if ((x + y) % 4 == 0)
        printf("%s ", DOT_BG);
    else
        printf("  ");
    set_color(C_RESET);
    fflush(stdout);
}

/* ========================= FRUTAS ========================= */

void draw_fruit(int i) {
    int col = arena_col(fruits[i].x);
    gotoxy(col, fruits[i].y);
    set_color(fruits[i].color);
    printf("%s", fruits[i].symbol);
    set_color(C_RESET);
    fflush(stdout);
}

static const struct {
    int         value;
    const char *symbol;
    int         color;
    int         rarity;
} FRUIT_TABLE[] = {
    { 10, SYM_APPLE,  13, 0 },
    { 20, SYM_GRAPE,  13, 0 },
    { 30, SYM_CHERRY, 12, 0 },
    { 50, SYM_STAR,   14, 1 },
    { 80, SYM_GEM,    11, 2 },
};
#define FRUIT_TABLE_LEN 5

void spawn_fruit_anim(int i) {
    int col = arena_col(fruits[i].x);
    int y   = fruits[i].y;

    for (int f = 0; f < 2; f++) {
        gotoxy(col, y);
        set_color(C_FLASH);
        printf("* ");
        fflush(stdout);
        Sleep(40);
        erase_cell(fruits[i].x, y);
        Sleep(30);
    }
    draw_fruit(i);
}

void spawn_fruit(int i) {
    int x, y, ok;

    do {
        ok = 1;
        x = rand() % WIDTH + 1;
        y = rand() % (HEIGHT - TOP - 1) + TOP + 1;

        if (is_on_snake(x, y)) { ok = 0; continue; }
        if (is_obstacle(x, y)) { ok = 0; continue; }

        for (int j = 0; j < FRUIT_COUNT; j++) {
            if (j != i && fruits[j].x == x && fruits[j].y == y) {
                ok = 0; break;
            }
        }
    } while (!ok);

    int roll = rand() % 100;
    int fi;
    if      (roll < 10) fi = 4;
    else if (roll < 40) fi = 3;
    else                fi = rand() % 3;

    fruits[i].x      = x;
    fruits[i].y      = y;
    fruits[i].value  = FRUIT_TABLE[fi].value;
    fruits[i].symbol = FRUIT_TABLE[fi].symbol;
    fruits[i].color  = FRUIT_TABLE[fi].color;
    fruits[i].rarity = FRUIT_TABLE[fi].rarity;

    spawn_fruit_anim(i);
}

/* ========================= EFEITOS ========================= */

void flash_eat(int rarity) {
    int flashes    = rarity + 1;
    int flashColor = (rarity == 2) ? 11 : (rarity == 1 ? 14 : 10);
    int bc         = border_color_for_level();

    for (int f = 0; f < flashes; f++) {
        draw_box_unicode(ARENA_LEFT, TOP, ARENA_RIGHT, HEIGHT, flashColor);
        Sleep(25);
        draw_box_unicode(ARENA_LEFT, TOP, ARENA_RIGHT, HEIGHT, bc);
        Sleep(25);
    }
    draw_board_border();
}

void anim_death(void) {
    int cx  = ARENA_LEFT + WIDTH;
    int cy  = (TOP + HEIGHT) / 2;

    char msg[32];
    snprintf(msg, sizeof(msg), " %s GAME OVER %s ", SYM_SKULL, SYM_SKULL);

    for (int i = 0; i < 4; i++) {
        draw_box_unicode(ARENA_LEFT, TOP, ARENA_RIGHT, HEIGHT, C_GAMEOVER);
        if (i % 2 == 0)
            write_at(cx - (int)strlen(msg)/2, cy, C_GAMEOVER, msg);
        else
            fill_spaces(cx - (int)strlen(msg)/2, cy, (int)strlen(msg));
        Sleep(150);
    }
    draw_board_border();
}

void anim_level_up(void) {
    char msg[32];
    int cx = ARENA_LEFT + WIDTH;
    int cy = (TOP + HEIGHT) / 2;

    snprintf(msg, sizeof(msg), "  LEVEL %d!  ", level);

    for (int i = 0; i < 3; i++) {
        write_at(cx - (int)strlen(msg)/2, cy, C_FLASH, msg);
        Sleep(120);
        fill_spaces(cx - (int)strlen(msg)/2, cy, (int)strlen(msg));
        Sleep(80);
    }

    for (int x = 1; x <= WIDTH; x++) erase_cell(x, cy);
    for (int i = 0; i < FRUIT_COUNT; i++)
        if (fruits[i].y == cy) draw_fruit(i);
    for (int i = 0; i < obstacleCount; i++)
        if (obstacles[i].y == cy) draw_obstacle(obstacles[i].x, obstacles[i].y);
    for (int i = snakeLen - 1; i >= 0; i--) {
        if (snake[i].y != cy) continue;
        if (i == 0) draw_head(snake[i].x, snake[i].y);
        else        draw_body(snake[i].x, snake[i].y);
    }

    draw_board_border();
}

/* ========================= OVERLAY DE PAUSA ========================= */

void draw_pause_overlay(void) {
    int arenaW = WIDTH * 2;
    int boxW   = 20;
    int L = ARENA_LEFT + 1 + (arenaW - boxW) / 2;
    int R = L + boxW;
    int T = (TOP + HEIGHT) / 2 - 2;
    int B = T + 4;

    draw_box_double(L, T, R, B, C_PAUSED);
    clear_inside_box(L, T, R, B);

    char pauseTitle[32];
    snprintf(pauseTitle, sizeof(pauseTitle), "%s PAUSADO", SYM_PAUSE);
    print_centered_range(L, R, T + 1, C_PAUSED, pauseTitle);
    print_centered_range(L, R, T + 2, C_NORMAL, "P para retomar");
}

void clear_pause_overlay(void) {
    int arenaW = WIDTH * 2;
    int boxW   = 20;
    int L = ARENA_LEFT + 1 + (arenaW - boxW) / 2;
    int R = L + boxW;
    int T = (TOP + HEIGHT) / 2 - 2;
    int B = T + 4;

    for (int y = T; y <= B; y++) {
        gotoxy(L, y);
        for (int x = L; x <= R + 1; x++) putchar(' ');
    }
    fflush(stdout);

    draw_board_border();

    for (int y = T; y <= B; y++) {
        for (int lx = 1; lx <= WIDTH; lx++) {
            int col = arena_col(lx);
            if (col >= L && col <= R + 1) erase_cell(lx, y);
        }
    }
    draw_all_obstacles();
    for (int i = 0; i < FRUIT_COUNT; i++) draw_fruit(i);
    for (int i = snakeLen - 1; i >= 0; i--) {
        if (i == 0) draw_head(snake[i].x, snake[i].y);
        else        draw_body(snake[i].x, snake[i].y);
    }
    /* Redesenha BFS overlay */
    draw_bfs_visited_overlay();
    draw_bfs_path_overlay();
}

/* ========================= ESTADO DO JOGO ========================= */

void init_game(void) {
    snakeLen     = 5;
    score        = 0;
    level        = 1;
    speedMs      = INITIAL_SPEED;
    paused       = 0;
    gameOver     = 0;
    pausedSeconds= 0;
    lastElapsed  = -1;
    lastLevel    = -1;
    lastScore    = -1;
    lastFruits   = -1;
    lastCombo    = -1;
    fruitsEaten  = 0;
    comboCount   = 0;
    bfs_path_len = 0;
    bfs_visited_count = 0;

    int mx = WIDTH / 2;
    int my = (TOP + HEIGHT) / 2;

    for (int i = 0; i < snakeLen; i++) {
        snake[i].x = mx - i;
        snake[i].y = my;
    }

    dir     = DIR_RIGHT;
    nextDir = DIR_RIGHT;

    clear_screen();

    /* Inicializa obstaculos conforme o modo */
    obstacleCount = 0;
    if (obstacleType == 2) {
        spawn_obstacles();
    }

    draw_board_border();
    draw_board_background();
    draw_all_obstacles();
    draw_side_panel_frame();

    for (int i = snakeLen - 1; i >= 0; i--) {
        if (i == 0)              draw_head(snake[i].x, snake[i].y);
        else if (i==snakeLen-1)  draw_tail_seg(snake[i].x, snake[i].y);
        else                     draw_body(snake[i].x, snake[i].y);
    }

    for (int i = 0; i < FRUIT_COUNT; i++) spawn_fruit(i);

    /* Calcula BFS inicial */
    find_nearest_fruit_bfs();
    draw_bfs_visited_overlay();
    draw_bfs_path_overlay();

    startTime = time(NULL);
    update_side_panel();
}

/* ========================= DIRECAO A PARTIR DO BFS ========================= */

Direction bfs_next_direction(void) {
    if (bfs_path_len == 0) return dir; /* sem caminho, mantem direcao */

    int nx = bfs_path[0].x;
    int ny = bfs_path[0].y;
    int hx = snake[0].x;
    int hy = snake[0].y;

    int ddx = nx - hx;
    int ddy = ny - hy;

    if (ddx ==  1 && ddy ==  0) return DIR_DOWN;
    if (ddx == -1 && ddy ==  0) return DIR_UP;
    if (ddx ==  0 && ddy ==  1) return DIR_RIGHT;
    if (ddx ==  0 && ddy == -1) return DIR_LEFT;

    /* Fallback: verifica como direcao no eixo do tabuleiro */
    /* No nosso sistema: x = coluna logica (1..WIDTH), y = linha (TOP+1..HEIGHT-1) */
    ddx = nx - hx;
    ddy = ny - hy;

    if (ddx ==  0 && ddy == -1) return DIR_UP;
    if (ddx ==  1 && ddy ==  0) return DIR_RIGHT;
    if (ddx ==  0 && ddy ==  1) return DIR_DOWN;
    if (ddx == -1 && ddy ==  0) return DIR_LEFT;

    return dir;
}

/* ========================= UPDATE ========================= */

void update_game(void) {
    if (paused || gameOver) return;

    /* Limpa overlay BFS antigo */
    clear_bfs_overlay();

    /* No modo automatico, a BFS decide a direcao */
    if (autoMode) {
        find_nearest_fruit_bfs();

        if (bfs_path_len > 0) {
            /* Proximo passo do caminho */
            int nx = bfs_path[0].x;
            int ny = bfs_path[0].y;
            int hx = snake[0].x;
            int hy = snake[0].y;

            int ddx = nx - hx;
            int ddy = ny - hy;

            if      (ddy == -1) nextDir = DIR_UP;
            else if (ddx ==  1) nextDir = DIR_RIGHT;
            else if (ddy ==  1) nextDir = DIR_DOWN;
            else if (ddx == -1) nextDir = DIR_LEFT;
        }
    }

    dir = nextDir;

    int nx = snake[0].x;
    int ny = snake[0].y;

    if      (dir == DIR_UP)    ny--;
    else if (dir == DIR_RIGHT) nx++;
    else if (dir == DIR_DOWN)  ny++;
    else if (dir == DIR_LEFT)  nx--;

    /* Colisao com paredes */
    if (nx < 1 || nx > WIDTH || ny <= TOP || ny >= HEIGHT) {
        gameOver = 1;
        return;
    }

    /* Colisao com obstaculos */
    if (is_obstacle(nx, ny)) {
        gameOver = 1;
        return;
    }

    int fruitIndex = fruit_at(nx, ny);

    if (fruitIndex == -1) {
        for (int i = 0; i < snakeLen - 1; i++) {
            if (snake[i].x == nx && snake[i].y == ny) { gameOver = 1; return; }
        }
    } else {
        for (int i = 0; i < snakeLen; i++) {
            if (snake[i].x == nx && snake[i].y == ny) { gameOver = 1; return; }
        }
    }

    draw_body(snake[0].x, snake[0].y);

    Segment oldTail = snake[snakeLen - 1];

    for (int i = snakeLen; i > 0; i--) snake[i] = snake[i-1];

    snake[0].x = nx;
    snake[0].y = ny;

    if (fruitIndex != -1) {
        int rarity = fruits[fruitIndex].rarity;

        score      += fruits[fruitIndex].value;
        snakeLen++;
        if (snakeLen >= MAX_SNAKE) snakeLen = MAX_SNAKE - 1;
        fruitsEaten++;
        comboCount++;

        int newLevel = score / LEVEL_STEP + 1;
        int didLevel = (newLevel > level);
        if (didLevel) {
            level   = newLevel;
            speedMs -= 8;
            if (speedMs < MIN_SPEED) speedMs = MIN_SPEED;
        }

        spawn_fruit(fruitIndex);

        /* Lógica de Inversão */
        if (reverseMode) {
            /* Inverte o array da cobra */
            for (int i = 0; i < snakeLen / 2; i++) {
                Segment temp = snake[i];
                snake[i] = snake[snakeLen - 1 - i];
                snake[snakeLen - 1 - i] = temp;
            }
            /* Recalcula a direção baseada na nova cabeça e no segmento seguinte */
            if (snakeLen > 1) {
                int dx_rev = snake[0].x - snake[1].x;
                int dy_rev = snake[0].y - snake[1].y;
                if      (dy_rev == -1) nextDir = DIR_UP;
                else if (dx_rev ==  1) nextDir = DIR_RIGHT;
                else if (dy_rev ==  1) nextDir = DIR_DOWN;
                else if (dx_rev == -1) nextDir = DIR_LEFT;
            }
            /* Limpa e redesenha a cobra inteira para atualizar os ícones */
            for (int i = 0; i < snakeLen; i++) erase_cell(snake[i].x, snake[i].y);
            for (int i = snakeLen - 1; i >= 0; i--) {
                if (i == 0)              draw_head(snake[i].x, snake[i].y);
                else if (i==snakeLen-1)  draw_tail_seg(snake[i].x, snake[i].y);
                else                     draw_body(snake[i].x, snake[i].y);
            }
        }

        /* Cria obstaculo no modo dinamico */
        if (obstacleType == 1 && obstacleCount < MAX_OBSTACLES) {
            obstacles[obstacleCount].x = oldTail.x;
            obstacles[obstacleCount].y = oldTail.y;
            obstacleCount++;
            draw_obstacle(oldTail.x, oldTail.y);
        }

        draw_tail_seg(snake[snakeLen-1].x, snake[snakeLen-1].y);
        draw_head(nx, ny);

        /* Recalcula BFS */
        find_nearest_fruit_bfs();
        draw_bfs_visited_overlay();
        draw_bfs_path_overlay();

        update_side_panel();

        flash_eat(rarity);
        if (didLevel) {
            anim_level_up();
            draw_board_border();
        }

    } else {
        erase_cell(oldTail.x, oldTail.y);
        draw_tail_seg(snake[snakeLen-1].x, snake[snakeLen-1].y);
        draw_head(nx, ny);

        /* Recalcula BFS a cada passo */
        find_nearest_fruit_bfs();
        draw_bfs_visited_overlay();
        draw_bfs_path_overlay();

        int elapsed = (int)(time(NULL) - startTime) - pausedSeconds;
        if (elapsed != lastElapsed || lastScore != score) {
            update_side_panel();
        }
    }
}

/* ========================= TELAS ========================= */

static const char *TITLE_ART[6] = {
    "  _____ _   _          _  _______",
    " / ____| \\ | |   /\\   | |/ /  ___|",
    "| (___ |  \\| |  /  \\  | ' /| |__  ",
    " \\___ \\| . ` | / /\\ \\ |  < |  __| ",
    " ____) | |\\  |/ ____ \\| . \\| |___ ",
    "|_____/|_| \\_/_/    \\_\\_|\\_\\_____|",
};

static void draw_title(int cx, int ty) {
    int cols[6] = { 10, 10, 11, 11, 10, 10 };
    for (int r = 0; r < 6; r++) {
        int len = (int)strlen(TITLE_ART[r]);
        gotoxy(cx - len / 2, ty + r);
        set_color(cols[r]);
        printf("%s", TITLE_ART[r]);
    }
    set_color(C_RESET);
    fflush(stdout);
}

static void screen_box(int L, int T, int R, int B,
                       int frame_color, int accent_color) {
    int my = (T + B) / 2;

    set_color(frame_color);
    gotoxy(L, T);
    printf("%s", BOX_TLH);
    for (int x = L+1; x < R; x++) printf("%s", BOX_DH);
    printf("%s", BOX_TRH);

    gotoxy(L, B);
    printf("%s", BOX_BLH);
    for (int x = L+1; x < R; x++) printf("%s", BOX_DH);
    printf("%s", BOX_BRH);

    for (int y = T+1; y < B; y++) {
        if (y == my) {
            set_color(accent_color);
            gotoxy(L, y); printf("%s", BOX_LM);
            for (int x = L+1; x < R; x++) printf("%s", BOX_DH);
            gotoxy(R, y); printf("%s", BOX_RM);
        } else {
            set_color(frame_color);
            gotoxy(L, y); printf("%s", BOX_DV);
            gotoxy(R, y); printf("%s", BOX_DV);
        }
    }
    set_color(C_RESET);
    fflush(stdout);
}

static void screen_sep(int L, int y, int R, int color) {
    set_color(color);
    gotoxy(L+1, y);
    printf("%s", BOX_LM);
    for (int x = L+2; x < R; x++) printf("%s", BOX_DH);
    printf("%s", BOX_RM);
    set_color(C_RESET);
    fflush(stdout);
}

static void screen_clear(int L, int T, int R, int B) {
    for (int y = T+1; y < B; y++) {
        gotoxy(L+1, y);
        for (int x = L+1; x < R; x++) putchar(' ');
    }
    fflush(stdout);
}

static void screen_center(int L, int R, int y, int color, const char *text) {
    int w = R - L - 1;
    int len = (int)strlen(text);
    int x = L + 1 + (w - len) / 2;
    if (x < L+1) x = L+1;
    gotoxy(x, y);
    set_color(color);
    printf("%s", text);
    set_color(C_RESET);
    fflush(stdout);
}

static void screen_cursor(int x, int y, int selected) {
    static DWORD last = 0;
    static int   on   = 1;
    DWORD now = GetTickCount();
    if (now - last > 450) { on = !on; last = now; }
    gotoxy(x, y);
    if (selected && on) { set_color(14); printf(">"); }
    else                { set_color(0);  printf(" "); }
    set_color(C_RESET);
    fflush(stdout);
}

static void screen_dots(int L, int y, int R, int color) {
    set_color(color);
    gotoxy(L+2, y);
    for (int x = L+2; x < R-1; x++) printf((x % 2 == 0) ? "." : " ");
    set_color(C_RESET);
    fflush(stdout);
}

/* ---------------------------------------------------------------
 * MENU PRINCIPAL
 * --------------------------------------------------------------- */

void render_menu_screen(void) {
    if (menuNeedsRedraw) {
        clear_screen();

        int L = 12, R = 67, T = 1, B = 30;
        int cx = (L + R) / 2;

        screen_box(L, T, R, B, 10, 14);
        screen_clear(L, T, R, B);

        draw_title(cx, T + 2);

        screen_dots(L, T + 9, R, 2);

        screen_center(L, R, T + 10, 8, "B F S   E D I T I O N");

        /* Subtitulo com brain emoji */
        char sub[64];
        snprintf(sub, sizeof(sub), "%s Cobra Inteligente com BFS %s", SYM_BRAIN, SYM_BRAIN);
        screen_center(L, R, T + 11, 3, sub);

        screen_sep(L, T + 12, R, 10);

        /* Instrucoes rodape */
        screen_sep(L, B - 2, R, 2);
        screen_center(L, R, B - 1, 8, "W / S   MOVER        ENTER   CONFIRMAR");

        menuNeedsRedraw = 0;
        menuLastIndex   = -1;
    }

    /* Cursor piscante */
    {
        int L = 12, R = 67, T = 1;
        int cx = (L + R) / 2;
        for (int i = 0; i < 6; i++)
            screen_cursor(cx - 15, T + 12 + i * 3, i == menuIndex);
    }

    /* Itens do menu */
    if (menuLastIndex != menuIndex) {
        int L = 12, R = 67, T = 1;
        int cx = (L + R) / 2;

        const char *obsLabels[] = { "SEM OBSTACULOS", "DINAMICO (POR FRUTA)", "FIXO (INICIO)" };
        char obsLine[64];
        snprintf(obsLine, sizeof(obsLine), "OBSTACULOS: %s", obsLabels[obstacleType]);

        char revLine[64];
        snprintf(revLine, sizeof(revLine), "INVERTER AO COMER: %s", reverseMode ? "SIM" : "NAO");

        const char *labels[] = { "JOGAR MANUAL", "JOGAR AUTO (BFS)", obsLine, revLine, "RANKING", "SAIR" };
        int hiCol[] = { 10, 9, 11, 13, 14, 12 };

        for (int i = 0; i < 6; i++) {
            int oy = T + 12 + i * 3;

            gotoxy(L + 2, oy);
            for (int x = L + 2; x < R - 1; x++) putchar(' ');

            if (i == menuIndex) {
                char line[64];
                snprintf(line, sizeof(line), "[ %s ]", labels[i]);
                gotoxy(L + 3, oy); set_color(hiCol[i]); printf("<<");
                gotoxy(R - 4, oy); set_color(hiCol[i]); printf(">>");
                screen_center(L, R, oy, hiCol[i], line);
            } else {
                screen_center(L, R, oy, 8, labels[i]);
            }
        }

        /* High score */
        {
            int rs = best_score();
            char rec[40];
            snprintf(rec, sizeof(rec), "HIGH  SCORE     %06d", rs);
            gotoxy(L + 2, T + 27);
            for (int x = L + 2; x < R - 1; x++) putchar(' ');
            screen_center(L, R, T + 27, rs > 0 ? 14 : 8, rec);
        }

        menuLastIndex = menuIndex;
    }
}

/* ---------------------------------------------------------------
 * RANKING
 * --------------------------------------------------------------- */

void render_ranking_screen(void) {
    if (!rankingNeedsRedraw) return;

    clear_screen();

    int L = 12, R = 67, T = 2, B = 28;

    screen_box(L, T, R, B, 14, 11);
    screen_clear(L, T, R, B);

    screen_center(L, R, T + 1, 14, "H A L L   O F   F A M E");
    screen_sep(L, T + 2, R, 14);
    screen_center(L, R, T + 3, 8, "#      NOME                     SCORE");
    screen_sep(L, T + 4, R, 8);

    int count = 0;
    load_ranking(&count);

    if (count == 0) {
        screen_center(L, R, T + 13, 8, "- - -  SEM SCORES  - - -");
    } else {
        const char *pos[]  = { "1ST", "2ND", "3RD", "4TH", "5TH" };
        int posCol[]       = {   14,    11,    13,     7,     7  };

        for (int i = 0; i < count; i++) {
            char line[64];
            int y = T + 6 + i * 3;

            if (i == 0) {
                set_color(14);
                gotoxy(L + 2, y - 1);
                for (int x = L + 2; x < R - 1; x++) printf("%s", BOX_DH);
                gotoxy(L + 2, y + 1);
                for (int x = L + 2; x < R - 1; x++) printf("%s", BOX_DH);
                set_color(C_RESET);
                fflush(stdout);
            }

            snprintf(line, sizeof(line), "[%s]   %-18s   %06d",
                     pos[i], ranking[i].name, ranking[i].score);
            screen_center(L, R, y, posCol[i], line);
        }
    }

    screen_sep(L, B - 2, R, 14);
    screen_center(L, R, B - 1, 8, "ENTER / ESC     VOLTAR AO MENU");

    rankingNeedsRedraw = 0;
}

/* ---------------------------------------------------------------
 * GAME OVER
 * --------------------------------------------------------------- */

void render_gameover_screen(void) {
    if (gameoverNeedsRedraw) {
        clear_screen();

        int L = 12, R = 67, T = 2, B = 28;

        screen_box(L, T, R, B, 12, 14);
        screen_clear(L, T, R, B);

        screen_center(L, R, T + 1, 12, "G A M E    O V E R");

        screen_sep(L, T + 2, R, 12);

        int elapsed = (int)(pauseStart > startTime
                            ? pauseStart - startTime
                            : time(NULL) - startTime) - pausedSeconds;
        if (elapsed < 0) elapsed = 0;

        char buf[48];

        snprintf(buf, sizeof(buf), "SCORE     >>   %06d", score);
        screen_center(L, R, T + 4, 14, buf);

        snprintf(buf, sizeof(buf), "NIVEL     >>   %d", level);
        screen_center(L, R, T + 6, 10, buf);

        snprintf(buf, sizeof(buf), "TEMPO     >>   %02d : %02d", elapsed/60, elapsed%60);
        screen_center(L, R, T + 8, 13, buf);

        snprintf(buf, sizeof(buf), "FRUTAS    >>   %d", fruitsEaten);
        screen_center(L, R, T + 10, 11, buf);

        snprintf(buf, sizeof(buf), "COMBO     >>   %d x", comboCount);
        screen_center(L, R, T + 12, 12, buf);

        snprintf(buf, sizeof(buf), "MODO      >>   %s", autoMode ? "BFS AUTO" : "MANUAL");
        screen_center(L, R, T + 14, autoMode ? 9 : 10, buf);

        if (score > 0 && score >= best_score()) {
            screen_sep(L, T + 15, R, 14);
            screen_center(L, R, T + 16, 14, "* *  N O V O   R E C O R D E  * *");
        }

        screen_sep(L, B - 3, R, 12);
        screen_center(L, R, B - 2, 8, "W / S   MOVER        ENTER   CONFIRMAR");

        gameoverNeedsRedraw = 0;
        gameoverLastIndex   = -1;
    }

    if (gameoverLastIndex != gameOverIndex) {
        int L = 12, R = 67, T = 2, B = 28;

        const char *opts[] = { "JOGAR  NOVAMENTE", "VOLTAR  AO  MENU" };
        int hiCol[] = { 10, 8 };

        for (int i = 0; i < 2; i++) {
            int oy = B - 5 + i * 2;
            gotoxy(L + 2, oy);
            for (int x = L + 2; x < R - 1; x++) putchar(' ');

            if (i == gameOverIndex) {
                char line[40];
                snprintf(line, sizeof(line), "[ %s ]", opts[i]);
                gotoxy(L + 3, oy); set_color(hiCol[i]); printf("<<");
                gotoxy(R - 4, oy); set_color(hiCol[i]); printf(">>");
                screen_center(L, R, oy, hiCol[i], line);
            } else {
                screen_center(L, R, oy, 8, opts[i]);
            }
        }
        gameoverLastIndex = gameOverIndex;
    }
}

/* ========================= INPUT ========================= */

void input_menu(GameState *state) {
    if (!_kbhit()) return;
    int k = _getch();

    if (k == 0 || k == 224) {
        k = _getch();
        if      (k == 72) { menuIndex--; if (menuIndex < 0) menuIndex = 5; }
        else if (k == 80) { menuIndex++; if (menuIndex > 5) menuIndex = 0; }
        return;
    }

    if      (k=='w'||k=='W') { menuIndex--; if (menuIndex < 0) menuIndex = 5; }
    else if (k=='s'||k=='S') { menuIndex++; if (menuIndex > 5) menuIndex = 0; }
    else if (k == 13) {
        if (menuIndex == 0) {
            autoMode = 0; init_game(); menuNeedsRedraw = 1; *state = STATE_PLAYING;
        } else if (menuIndex == 1) {
            autoMode = 1; init_game(); menuNeedsRedraw = 1; *state = STATE_PLAYING;
        } else if (menuIndex == 2) {
            obstacleType = (obstacleType + 1) % 3;
            menuLastIndex = -1;
        } else if (menuIndex == 3) {
            reverseMode = !reverseMode;
            menuLastIndex = -1;
        } else if (menuIndex == 4) {
            rankingNeedsRedraw = 1; *state = STATE_RANKING;
        } else {
            *state = STATE_EXIT;
        }
    }
}

void input_ranking(GameState *state) {
    if (!_kbhit()) return;
    int k = _getch();
    if (k == 13 || k == 27 || k == 'm' || k == 'M') {
        menuNeedsRedraw = 1;
        *state = STATE_MENU;
    }
}

void input_game(GameState *state) {
    if (!_kbhit()) return;
    int k = _getch();

    if (k == 0 || k == 224) {
        k = _getch();
        if (!autoMode) {
            if      (k == 72 && dir != DIR_DOWN)  nextDir = DIR_UP;
            else if (k == 77 && dir != DIR_LEFT)  nextDir = DIR_RIGHT;
            else if (k == 80 && dir != DIR_UP)    nextDir = DIR_DOWN;
            else if (k == 75 && dir != DIR_RIGHT) nextDir = DIR_LEFT;
        }
        return;
    }

    if (!autoMode) {
        if      ((k=='w'||k=='W') && dir != DIR_DOWN)  nextDir = DIR_UP;
        else if ((k=='d'||k=='D') && dir != DIR_LEFT)  nextDir = DIR_RIGHT;
        else if ((k=='s'||k=='S') && dir != DIR_UP)    nextDir = DIR_DOWN;
        else if ((k=='a'||k=='A') && dir != DIR_RIGHT) nextDir = DIR_LEFT;
    }

    if (k == 'p' || k == 'P') {
        if (!paused) {
            paused = 1;
            pauseStart = time(NULL);
            update_side_panel();
            draw_pause_overlay();
        } else {
            paused = 0;
            pausedSeconds += (int)(time(NULL) - pauseStart);
            clear_pause_overlay();
            update_side_panel();
        }
    } else if (k == 'q' || k == 'Q') {
        menuNeedsRedraw = 1;
        *state = STATE_MENU;
    } else if (k == 'v' || k == 'V') {
        /* Toggle visualizacao do caminho BFS */
        showBfsPath = !showBfsPath;
        if (!showBfsPath) {
            /* Limpa o overlay do caminho */
            for (int i = 0; i < bfs_path_len; i++) {
                int px = bfs_path[i].x;
                int py = bfs_path[i].y;
                if (!is_on_snake(px, py) && fruit_at(px, py) < 0 && !is_obstacle(px, py))
                    erase_cell(px, py);
            }
        } else {
            draw_bfs_path_overlay();
        }
    } else if (k == 'g' || k == 'G') {
        /* Toggle visualizacao dos visitados BFS */
        showBfsVisit = !showBfsVisit;
        if (!showBfsVisit) {
            clear_bfs_overlay();
            draw_all_obstacles();
            for (int i = 0; i < FRUIT_COUNT; i++) draw_fruit(i);
            for (int i = snakeLen - 1; i >= 0; i--) {
                if (i == 0) draw_head(snake[i].x, snake[i].y);
                else        draw_body(snake[i].x, snake[i].y);
            }
            if (showBfsPath) draw_bfs_path_overlay();
        } else {
            draw_bfs_visited_overlay();
        }
    } else if (k == 't' || k == 'T') {
        /* Alterna entre modo manual e automatico */
        autoMode = !autoMode;
        update_side_panel();
    }
}

void input_gameover(GameState *state) {
    if (!_kbhit()) return;
    int k = _getch();

    if (k == 0 || k == 224) {
        k = _getch();
        if      (k == 72) gameOverIndex = 0;
        else if (k == 80) gameOverIndex = 1;
        return;
    }

    if      (k=='w'||k=='W') gameOverIndex = 0;
    else if (k=='s'||k=='S') gameOverIndex = 1;
    else if (k == '1') { init_game(); gameoverNeedsRedraw=1; menuNeedsRedraw=1; *state=STATE_PLAYING; }
    else if (k == '2') { menuNeedsRedraw=1; gameoverNeedsRedraw=1; *state=STATE_MENU; }
    else if (k == 13) {
        if (gameOverIndex == 0) {
            init_game(); gameoverNeedsRedraw=1; menuNeedsRedraw=1; *state=STATE_PLAYING;
        } else {
            menuNeedsRedraw=1; gameoverNeedsRedraw=1; *state=STATE_MENU;
        }
    }
}

/* ========================= MAIN ========================= */

int main(void) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        printf("Erro ao obter o console.\n");
        return 1;
    }

    COORD bufSize = { (SHORT)CON_COLS, (SHORT)CON_ROWS };
    SetConsoleScreenBufferSize(hConsole, bufSize);

    SMALL_RECT winRect = { 0, 0, (SHORT)(CON_COLS - 1), (SHORT)(CON_ROWS - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &winRect);

    SetConsoleTitleA("SNAKE BFS - Cobra Inteligente");

    srand((unsigned int)time(NULL));
    hide_cursor();

    GameState state = STATE_MENU;

    while (state != STATE_EXIT) {

        if (state == STATE_MENU) {
            render_menu_screen();
            input_menu(&state);
            Sleep(30);
        }

        else if (state == STATE_RANKING) {
            render_ranking_screen();
            input_ranking(&state);
            Sleep(50);
        }

        else if (state == STATE_PLAYING) {
            input_game(&state);
            if (state != STATE_PLAYING) continue;

            if (!paused) {
                update_game();

                if (gameOver) {
                    anim_death();

                    char name[32];
                    show_cursor();

                    gotoxy(0, HEIGHT + 2);
                    set_color(C_TITLE);
                    printf("  Digite seu nome: ");
                    fflush(stdout);
                    set_color(C_PANEL_VAL);

                    if (fgets(name, sizeof(name), stdin) != NULL)
                        trim_newline(name);
                    else
                        strcpy(name, "Jogador");

                    if (strlen(name) == 0) strcpy(name, "Jogador");

                    insert_ranking(name, score);
                    hide_cursor();

                    gameOverIndex       = 0;
                    gameoverNeedsRedraw = 1;
                    state = STATE_GAMEOVER;

                } else {
                    /* Modo auto: velocidade fixa mais rapida */
                    int sp = autoMode ? (speedMs > 80 ? 80 : speedMs) : speedMs;
                    Sleep(sp);
                }
            } else {
                Sleep(50);
            }
        }

        else if (state == STATE_GAMEOVER) {
            render_gameover_screen();
            input_gameover(&state);
            Sleep(50);
        }
    }

    show_cursor();
    clear_screen();
    set_color(C_RESET);
    return 0;
}
