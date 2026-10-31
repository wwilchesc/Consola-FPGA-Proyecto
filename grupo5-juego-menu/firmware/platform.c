#include "platform.h"
void console_frame(Console *c, const ConsolePlatform *p, uint8_t pixels[SCREEN_W*SCREEN_H]) {
    uint16_t keys=p->read_keys?p->read_keys(p->context):0;
    console_step(c,keys);
    console_render(c,pixels);
    if(p->present) p->present(p->context,pixels);
    if(p->play_sound && c->sound!=SOUND_NONE) p->play_sound(p->context,c->sound);
    c->sound=SOUND_NONE;
}
