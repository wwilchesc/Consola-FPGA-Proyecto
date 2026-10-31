#include "internal.h"

void set_result(Console *c, Result result) {
    unsigned game=c->mode==MODE_SNAKE?1u:0u;
    unsigned count=c->mode==MODE_SNAKE?c->players:1u;
    for(unsigned p=0;p<count;++p)
        if(c->score[p]>c->records[game]) c->records[game]=c->score[p];
    c->sound=(result==RESULT_PAC_LOSE || result==RESULT_SOLO_LOSE)?SOUND_HIT:SOUND_FINISH;
    c->result=result;
    c->last_game=c->mode;
    c->mode=MODE_RESULT;
    c->paused=0;
}
