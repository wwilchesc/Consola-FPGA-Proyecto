#include "internal.h"

int key_direction(uint16_t keys, int player, uint8_t *dir) {
    if(player==0) {
        if(keys&K_W) { *dir=DIR_UP; return 1; }
        if(keys&K_D) { *dir=DIR_RIGHT; return 1; }
        if(keys&K_S) { *dir=DIR_DOWN; return 1; }
        if(keys&K_A) { *dir=DIR_LEFT; return 1; }
    } else {
        if(keys&K_UP) { *dir=DIR_UP; return 1; }
        if(keys&K_RIGHT) { *dir=DIR_RIGHT; return 1; }
        if(keys&K_DOWN) { *dir=DIR_DOWN; return 1; }
        if(keys&K_LEFT) { *dir=DIR_LEFT; return 1; }
    }
    return 0;
}
