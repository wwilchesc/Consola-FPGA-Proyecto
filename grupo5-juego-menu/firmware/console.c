#include "internal.h"

void console_init(Console *c) {
    uint8_t *p=(uint8_t *)c;
    for(unsigned i=0;i<sizeof(*c);++i) p[i]=0;
    c->mode=MODE_MENU; c->last_game=MODE_MENU; c->players=2;
    c->random_state=0xACE1u;
}
void console_step(Console *c,uint16_t keys) {
    c->sound=SOUND_NONE;
    uint16_t edge=(uint16_t)(keys & (uint16_t)~c->last_keys);
    c->last_keys=keys;
    if(c->mode==MODE_MENU) {
        if(edge&(K_LEFT|K_A)) c->players=1;
        if(edge&(K_RIGHT|K_D)) c->players=2;
        if(edge&(K_UP|K_W)) c->selection=0;
        if(edge&(K_DOWN|K_S)) c->selection=1;
        if(edge&K_GAME1) start_pacman(c);
        else if(edge&K_GAME2) start_snake(c);
        else if(edge&K_ENTER) {
            if(c->selection==0) start_pacman(c); else start_snake(c);
        }
        return;
    }
    if(edge&K_ESCAPE) {
        c->mode=MODE_MENU; c->paused=0; return;
    }
    if(c->mode==MODE_RESULT) {
        if(edge&K_ENTER) {
            if(c->last_game==MODE_PACMAN) start_pacman(c);
            else start_snake(c);
        }
        return;
    }
    if(edge&K_RESTART) {
        if(c->mode==MODE_PACMAN) start_pacman(c); else start_snake(c);
        return;
    }
    if(edge&K_PAUSE) { c->paused=(uint8_t)!c->paused; c->sound=SOUND_MENU; return; }
    if(c->paused) return;
    /* En individual, ambas familias de teclas controlan J1, como el HTML.
       La prioridad fija de direcciones la determina controls.c. */
    if(c->players==1) {
        if(keys&K_UP) keys|=K_W;
        if(keys&K_RIGHT) keys|=K_D;
        if(keys&K_DOWN) keys|=K_S;
        if(keys&K_LEFT) keys|=K_A;
    }
    ++c->ticks;
    if(c->mode==MODE_PACMAN) pacman_tick(c,keys);
    else if(c->mode==MODE_SNAKE) snake_tick(c,keys);
}
