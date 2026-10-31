#ifndef CONSOLA_GRUPO5_H
#define CONSOLA_GRUPO5_H

#include <stdint.h>

#define SCREEN_W 64u
#define SCREEN_H 64u
#define GRID_W 16u
#define GRID_H 14u
#define GRID_SIZE (GRID_W * GRID_H)
#define SNAKE_W 32u
#define SNAKE_H 28u
#define SNAKE_SIZE (SNAKE_W * SNAKE_H)
#define SNAKE_MAX 128u

/* Colores lógicos de 3 bits: bit2=R, bit1=G, bit0=B.
   El grupo 4 debe confirmar si este formato coincide con su framebuffer. */
enum { COL_BLACK=0, COL_BLUE=1, COL_GREEN=2, COL_CYAN=3,
       COL_RED=4, COL_MAGENTA=5, COL_YELLOW=6, COL_WHITE=7 };

/* Máscara de controles abstractos, NO códigos PS/2.
   El grupo 2 se ocupa de traducir los scan codes a estos controles. */
enum {
    K_W=1u<<0, K_A=1u<<1, K_S=1u<<2, K_D=1u<<3,
    K_UP=1u<<4, K_LEFT=1u<<5, K_DOWN=1u<<6, K_RIGHT=1u<<7,
    K_ENTER=1u<<8, K_ESCAPE=1u<<9,
    K_GAME1=1u<<10, K_GAME2=1u<<11, K_PAUSE=1u<<12, K_RESTART=1u<<13
};

/* Nombres PAC conservados en la API por compatibilidad; juego visible: Nexo. */
typedef enum { MODE_MENU=0, MODE_PACMAN=1, MODE_SNAKE=2, MODE_RESULT=3 } Mode;
typedef enum { RESULT_NONE=0, RESULT_PAC_WIN, RESULT_PAC_LOSE,
               RESULT_P1_WIN, RESULT_P2_WIN, RESULT_DRAW, RESULT_SOLO_LOSE } Result;
typedef enum { DIR_UP=0, DIR_RIGHT=1, DIR_DOWN=2, DIR_LEFT=3 } Direction;
typedef enum { SOUND_NONE=0, SOUND_MENU, SOUND_PICKUP, SOUND_HIT, SOUND_FINISH } Sound;
typedef struct { uint8_t x, y; } Cell;
typedef struct { Cell body[SNAKE_MAX]; uint8_t length, direction, alive; } Snake;

typedef struct {
    Mode mode;
    Mode last_game;
    Result result;
    uint16_t last_keys;
    uint8_t selection, paused, players;
    uint16_t records[2]; /* Récords de sesión; sin persistencia EEPROM. */
    Sound sound; /* Evento del último paso; consumir una sola vez. */
    uint8_t walls[GRID_SIZE], pellets[GRID_SIZE];
    uint16_t pellets_left;
    uint8_t lives;
    Cell pac[2], ghosts[2];
    uint8_t pac_direction[2];
    Snake snake[2];
    Cell apple;
    uint16_t score[2];
    uint32_t ticks, random_state;
} Console;

void console_init(Console *c);
/* Una llamada corresponde a un paso de lógica (propuesta: 150 ms).
   keys representa teclas mantenidas; las acciones de menú detectan flancos. */
void console_step(Console *c, uint16_t keys);
/* Genera exactamente 4096 bytes de colores 0..7. */
void console_render(const Console *c, uint8_t framebuffer[SCREEN_W*SCREEN_H]);

#endif
