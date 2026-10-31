#include "internal.h"

static uint32_t rand_next(Console *c) {
    /* Mismo LFSR de 16 bits de la demostración HTML. */
    c->random_state=(c->random_state>>1) ^
                    ((c->random_state&1u)?0xB400u:0u);
    return c->random_state;
}
static int snake_contains(const Snake *s, Cell p, unsigned limit) {
    for(unsigned i=0;i<limit;++i) if(equal(s->body[i],p)) return 1;
    return 0;
}
static int apple_occupied(const Console *c, Cell p) {
    return !inside_snake(p) ||
        snake_contains(&c->snake[0],p,c->snake[0].length) ||
        (c->players==2 && snake_contains(&c->snake[1],p,c->snake[1].length));
}
static void place_apple(Console *c) {
    /* Seleccionar entre celdas libres en orden fila/columna, igual al HTML.
       No se reserva un arreglo temporal de 896 celdas. */
    unsigned available=0;
    for(unsigned y=0;y<SNAKE_H;++y) for(unsigned x=0;x<SNAKE_W;++x) {
        Cell p={(uint8_t)x,(uint8_t)y};
        if(!apple_occupied(c,p)) ++available;
    }
    if(!available) { set_result(c,RESULT_DRAW); return; }
    unsigned chosen=(unsigned)(rand_next(c)%available);
    for(unsigned y=0;y<SNAKE_H;++y) for(unsigned x=0;x<SNAKE_W;++x) {
        Cell p={(uint8_t)x,(uint8_t)y};
        if(apple_occupied(c,p)) continue;
        if(chosen--==0) { c->apple=p; return; }
    }
}
void start_snake(Console *c) {
    c->mode=MODE_SNAKE; c->last_game=MODE_SNAKE; c->result=RESULT_NONE;
    c->paused=0; c->ticks=0; c->score[0]=c->score[1]=0;
    c->random_state=0xACE1u;
    for(unsigned i=0;i<2;++i) {
        c->snake[i].length=3; c->snake[i].alive=1;
    }
    c->snake[0].direction=DIR_RIGHT;
    c->snake[0].body[0]=(Cell){8,8};
    c->snake[0].body[1]=(Cell){7,8};
    c->snake[0].body[2]=(Cell){6,8};
    c->snake[1].direction=DIR_LEFT;
    c->snake[1].body[0]=(Cell){23,19};
    c->snake[1].body[1]=(Cell){24,19};
    c->snake[1].body[2]=(Cell){25,19};
    place_apple(c);
}
void snake_tick(Console *c, uint16_t keys) {
    Cell next[2]; uint8_t grows[2]={0,0},dies[2]={0,0};
    for(unsigned p=0;p<c->players;++p) {
        uint8_t proposed;
        if(key_direction(keys,(int)p,&proposed) &&
           ((proposed+2u)%4u)!=c->snake[p].direction)
            c->snake[p].direction=proposed;
        next[p]=moved(c->snake[p].body[0],c->snake[p].direction);
        grows[p]=(uint8_t)equal(next[p],c->apple);
        if(!inside_snake(next[p])) dies[p]=1;
    }
    /* Se evalúan ambas serpientes antes de moverlas para evitar ventaja por turno. */
    for(unsigned p=0;p<c->players;++p) for(unsigned q=0;q<c->players;++q) {
        unsigned limit=c->snake[q].length;
        /* La última celda queda libre si la serpiente se mueve sin crecer. */
        if((!grows[q] || limit==SNAKE_MAX) && limit>0) --limit;
        if(snake_contains(&c->snake[q],next[p],limit)) dies[p]=1;
    }
    if(c->players==2 && (equal(next[0],next[1]) ||
       (equal(next[0],c->snake[1].body[0]) && equal(next[1],c->snake[0].body[0]))))
        dies[0]=dies[1]=1;
    for(unsigned p=0;p<c->players;++p) {
        Snake *s=&c->snake[p];
        s->alive=(uint8_t)!dies[p];
        if(dies[p]) continue;
        if(grows[p] && s->length<SNAKE_MAX) ++s->length;
        for(unsigned i=s->length-1;i>0;--i) s->body[i]=s->body[i-1];
        s->body[0]=next[p];
        if(grows[p]) { if(c->score[p]<65535u) c->score[p]++; c->sound=SOUND_PICKUP; }
    }
    if(dies[0] || dies[1]) {
        if(c->players==1) set_result(c,RESULT_SOLO_LOSE);
        else if(dies[0] && dies[1]) set_result(c,RESULT_DRAW);
        else set_result(c,dies[0]?RESULT_P2_WIN:RESULT_P1_WIN);
        return;
    }
    if(grows[0] || grows[1]) place_apple(c);
}
