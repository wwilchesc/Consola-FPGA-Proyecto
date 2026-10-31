/* Prototipo ejecutable en PC para validar el menú, los juegos y la imagen.
   NO sustituye el firmware de arranque RISC-V ni los controladores MMIO. */
#include "console.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

static uint8_t pixels[SCREEN_W*SCREEN_H];
static const uint8_t rgb[8][3] = {
    {0,0,0},{0,0,255},{0,255,0},{0,255,255},
    {255,0,0},{255,0,255},{255,255,0},{255,255,255}
};
static int snapshot(const char *filename,const Console *c) {
    FILE *out=fopen(filename,"wb");
    if(!out) { perror(filename); return 0; }
    console_render(c,pixels);
    for(unsigned i=0;i<sizeof(pixels);++i) assert(pixels[i]<8);
    fprintf(out,"P6\n384 384\n255\n");
    for(unsigned y=0;y<SCREEN_H;++y) for(unsigned sy=0;sy<6;++sy)
        for(unsigned x=0;x<SCREEN_W;++x) for(unsigned sx=0;sx<6;++sx) {
            const uint8_t *p=rgb[pixels[y*64+x]];
            if(fwrite(p,1,3,out)!=3) { fclose(out);return 0; }
        }
    return fclose(out)==0;
}
int main(void) {
    Console c;
    console_init(&c);
    assert(c.mode==MODE_MENU);
    if(!snapshot("build/menu.ppm",&c)) return 1;
    console_step(&c,K_GAME1);
    /* Captura inicial: comparable con el HTML justo al iniciar la partida. */
    assert(c.mode==MODE_PACMAN && c.players==2);
    if(!snapshot("build/nexo.ppm",&c)) return 1;
    console_step(&c,K_ESCAPE); console_step(&c,0);
    console_step(&c,K_GAME2);
    assert(c.mode==MODE_SNAKE && c.players==2);
    if(!snapshot("build/snake.ppm",&c)) return 1;
    console_step(&c,K_PAUSE);
    if(!snapshot("build/pausa.ppm",&c)) return 1;
    console_step(&c,K_ESCAPE); console_step(&c,K_LEFT); console_step(&c,K_GAME2);
    assert(c.mode==MODE_SNAKE && c.players==1);
    if(!snapshot("build/snake-solo.ppm",&c)) return 1;
    for(unsigned i=0;i<40 && c.mode!=MODE_RESULT;++i) console_step(&c,0);
    if(c.mode!=MODE_RESULT) return 1;
    if(!snapshot("build/resultado.ppm",&c)) return 1;
    printf("sizeof(Console)=%zu; framebuffer=%zu bytes\n",sizeof(c),sizeof(pixels));
    printf("Capturas: build/menu.ppm, build/nexo.ppm y build/snake.ppm\n");
    printf("Demostración C ejecutada en PC, no en la FPGA.\n");
    return 0;
}
