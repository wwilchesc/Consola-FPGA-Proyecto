#include "../firmware/console.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../firmware/platform.h"

static void test_menu_and_games(void) {
    Console c;
    console_init(&c);
    assert(c.mode==MODE_MENU);
    console_step(&c,K_GAME1);
    assert(c.mode==MODE_PACMAN && c.lives==3 && c.pellets_left>0);
    /* Escape requiere un flanco de tecla y regresa al menú. */
    console_step(&c,K_ESCAPE);
    assert(c.mode==MODE_MENU);
    console_step(&c,0);
    console_step(&c,K_GAME2);
    assert(c.mode==MODE_SNAKE && c.snake[0].length==3 && c.snake[1].length==3);
    console_step(&c,0);
    console_step(&c,K_PAUSE);
    assert(c.paused);
    uint32_t ticks=c.ticks;
    console_step(&c,K_PAUSE);
    assert(c.ticks==ticks);
    console_step(&c,0);
    console_step(&c,K_PAUSE);
    assert(!c.paused);
    console_step(&c,K_ESCAPE);
    assert(c.mode==MODE_MENU);
}
static void test_render_palette(void) {
    Console c; uint8_t f[SCREEN_W*SCREEN_H];
    console_init(&c);
    console_render(&c,f);
    for(unsigned i=0;i<sizeof(f);++i) assert(f[i]<=7);
    console_step(&c,K_GAME1);
    console_render(&c,f);
    for(unsigned i=0;i<sizeof(f);++i) assert(f[i]<=7);
    console_step(&c,K_ESCAPE);
    console_step(&c,0);
    console_step(&c,K_GAME2);
    console_render(&c,f);
    for(unsigned i=0;i<sizeof(f);++i) assert(f[i]<=7);
}
static void test_visual_layout(void) {
    Console c;
    uint8_t f[SCREEN_W*SCREEN_H];
    console_init(&c);
    console_render(&c,f);
    assert(f[3u*64u+24u]==COL_YELLOW); /* título RETRO FPGA */
    assert(f[11u*64u+4u]==COL_CYAN); /* línea bajo título */
    assert(f[24u*64u+3u]==COL_YELLOW); /* resaltado del menú */
    console_step(&c,K_GAME1);
    console_render(&c,f);
    assert(f[0]==COL_BLUE); /* laberinto desde fila 0 */
    assert(f[56u*64u]==COL_BLACK); /* HUD inferior */
    assert(f[4u*64u+4u]==COL_YELLOW); /* casco Nexo */
    console_step(&c,K_ESCAPE);
    console_step(&c,0);
    console_step(&c,K_GAME2);
    console_render(&c,f);
    assert(f[16u*64u+16u]==COL_YELLOW); /* cabeza snake J1: 8,8 */
    assert(f[39u*64u+47u]==COL_GREEN); /* cabeza snake J2: 23,19 */
    assert(f[56u*64u]==COL_BLACK); /* barra HUD en la parte baja */
}
static void test_snake_resolution(void) {
    Console c;
    console_init(&c);
    console_step(&c,K_GAME2);
    console_step(&c,0);
    /* Sin cambios de dirección, chocarán eventualmente contra el borde. */
    for(unsigned i=0;i<25 && c.mode==MODE_SNAKE;++i) console_step(&c,0);
    assert(c.mode==MODE_RESULT);
    assert(c.result==RESULT_DRAW || c.result==RESULT_P1_WIN || c.result==RESULT_P2_WIN);
    console_step(&c,K_ENTER);
    assert(c.mode==MODE_SNAKE);
}
static void start(Console *c, unsigned players, unsigned game) {
    console_init(c);
    console_step(c,players==1?K_LEFT:K_RIGHT);
    console_step(c,game==1?K_GAME1:K_GAME2);
}
static void test_players_results_restart(void) {
    Console c;
    start(&c,1,1);
    Cell other=c.pac[1];
    c.ghosts[0]=other; c.ghosts[1]=other;
    console_step(&c,K_UP);
    assert(c.lives==3 && c.pac[1].x==other.x && c.pac[1].y==other.y);
    c.pellets_left=1;
    memset(c.pellets,0,sizeof(c.pellets));
    c.pellets[c.pac[0].y*GRID_W+c.pac[0].x]=1;
    console_step(&c,0);
    assert(c.result==RESULT_PAC_WIN && c.records[0]==1);
    console_step(&c,K_ENTER);
    assert(c.mode==MODE_PACMAN && c.players==1 && c.score[0]==0 && c.records[0]==1);
    c.lives=1; c.ghosts[0]=c.pac[0];
    console_step(&c,0);
    assert(c.result==RESULT_PAC_LOSE);
    start(&c,1,2);
    c.apple=(Cell){9,8};
    console_step(&c,0);
    assert(c.snake[0].length==4 && c.score[0]==1);
    console_step(&c,K_PAUSE);
    Console paused=c;
    console_step(&c,K_D);
    assert(c.ticks==paused.ticks && memcmp(c.snake,paused.snake,sizeof(c.snake))==0);
    console_step(&c,K_RESTART);
    assert(!c.paused && c.snake[0].length==3 && c.players==1 && c.ticks==0);
    c.snake[0].body[0]=(Cell){31,8};
    console_step(&c,0);
    assert(c.result==RESULT_SOLO_LOSE);
    for(unsigned winner=0;winner<3;++winner) {
        start(&c,2,2);
        if(winner!=1) c.snake[0].body[0]=(Cell){31,8};
        if(winner!=0) c.snake[1].body[0]=(Cell){0,19};
        console_step(&c,0);
        assert(c.result==(winner==0?RESULT_P2_WIN:winner==1?RESULT_P1_WIN:RESULT_DRAW));
    }
    start(&c,2,2);
    c.snake[0].body[0]=(Cell){10,10}; c.snake[1].body[0]=(Cell){12,10};
    console_step(&c,0); assert(c.result==RESULT_DRAW);
}
static void test_snake_limits(void) {
    Console c; start(&c,1,2);
    /* Rechazar inversión sobre el propio cuello. */
    console_step(&c,K_A); assert(c.snake[0].direction==DIR_RIGHT);
    /* Cola que se libera al crecer estando ya en el límite de capacidad. */
    c.snake[0].length=SNAKE_MAX;
    c.snake[0].body[0]=(Cell){1,1};
    for(unsigned i=1;i<SNAKE_MAX-1;++i)
        c.snake[0].body[i]=(Cell){(uint8_t)(i%32),(uint8_t)(4+i/32)};
    c.snake[0].body[SNAKE_MAX-1]=(Cell){2,1};
    c.apple=(Cell){2,1};
    console_step(&c,0);
    assert(c.mode==MODE_SNAKE && c.snake[0].length==SNAKE_MAX && c.score[0]==1);
    assert(c.apple.x!=2 || c.apple.y!=1);
    /* Colisión contra cuerpo propio que no se libera. */
    c.snake[0].body[1]=(Cell){3,1};
    console_step(&c,0); assert(c.result==RESULT_SOLO_LOSE);
}
static void test_html_regressions(void) {
    Console c; uint8_t a[4096],b[4096];
    start(&c,1,1);
    assert(c.pellets[12*GRID_W+14]==1); /* J2 ausente deja energía. */
    unsigned dots=c.pellets_left;
    console_step(&c,K_RIGHT); assert(c.pac[0].x==2 && c.score[0]==1);
    start(&c,2,1); assert(c.pellets_left+1u==dots);
    console_render(&c,a); c.ticks=1; console_render(&c,b);
    assert(memcmp(a,b,sizeof(a))!=0); /* Dos fases de animación Nexo. */
    c.pac[1]=c.pac[0]; console_render(&c,a);
    assert(a[c.pac[0].y*4*64+c.pac[0].x*4]==COL_CYAN);
    start(&c,1,1); c.ghosts[0]=(Cell){2,1};
    console_step(&c,K_D); assert(c.lives==2 && c.score[0]==0);
    start(&c,1,2); console_step(&c,K_UP);
    assert(c.snake[0].body[0].y==7);
    start(&c,2,2); c.snake[0].body[0]=(Cell){31,8};
    c.apple=(Cell){22,19}; console_step(&c,0);
    assert(c.result==RESULT_P2_WIN && c.score[1]==1);
    assert(!c.snake[0].alive && c.snake[1].alive && c.records[1]==1);
}
static void test_render_bounds(void) {
    struct { uint8_t before[32], f[4096], after[32]; } guard;
    Console c;
    for(unsigned players=1;players<=2;++players) for(unsigned game=1;game<=2;++game) {
        start(&c,players,game);
        for(unsigned t=0;t<300;++t) {
            memset(&guard,0xA5,sizeof(guard));
            console_render(&c,guard.f);
            for(unsigned i=0;i<4096;++i) assert(guard.f[i]<8);
            for(unsigned i=0;i<32;++i) assert(guard.before[i]==0xA5 && guard.after[i]==0xA5);
            console_step(&c,t%17==0?K_PAUSE:t%29==0?K_RESTART:t%7==0?K_ENTER:0);
        }
    }
}
static unsigned presented, sounds;
static uint16_t fake_keys(void *ctx) { (void)ctx; return K_GAME2; }
static void fake_present(void *ctx,const uint8_t *f) { (void)ctx; assert(f[0]<8); ++presented; }
static void fake_sound(void *ctx,Sound s) { (void)ctx; assert(s!=SOUND_NONE); ++sounds; }
static void test_platform(void) {
    Console c; uint8_t f[4096]; console_init(&c);
    ConsolePlatform p={0,fake_keys,fake_present,fake_sound};
    console_frame(&c,&p,f); assert(c.mode==MODE_SNAKE && presented==1);
    c.apple=(Cell){9,8}; console_frame(&c,&p,f);
    assert(sounds==1 && c.sound==SOUND_NONE);
}
int main(void) {
    test_players_results_restart();
    test_snake_limits();
    test_html_regressions();
    test_render_bounds();
    test_platform();
    test_menu_and_games();
    test_render_palette();
    test_snake_resolution();
    test_visual_layout();
    puts("OK: menú, estados, pausa, juegos, colisiones y formato del framebuffer.");
    return 0;
}
