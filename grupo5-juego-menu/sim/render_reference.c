/* Fixtures para comparar exclusivamente los 4096 píxeles, sin CSS exterior. */
#include "../firmware/console.h"
#include <stdio.h>
static int emit(Console *c) {
    uint8_t f[4096]; console_render(c,f);
    return fwrite(f,1,sizeof(f),stdout)==sizeof(f);
}
int main(void) {
    Console c;
    console_init(&c); if(!emit(&c)) return 1;
    for(unsigned players=1;players<=2;++players) {
        console_init(&c); c.players=(uint8_t)players;
        console_step(&c,K_GAME1);
        for(unsigned frame=0;frame<2;++frame) {
            c.ticks=frame; c.score[0]=67;
            if(!emit(&c)) return 1;
        }
        console_init(&c); c.players=(uint8_t)players;
        console_step(&c,K_GAME2);
        if(!emit(&c)) return 1;
        console_step(&c,0); if(!emit(&c)) return 1;
    }
    return 0;
}
