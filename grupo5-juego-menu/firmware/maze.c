#include "internal.h"

static void base_walls(Console *c) {
    for(unsigned y=0;y<GRID_H;++y) for(unsigned x=0;x<GRID_W;++x)
        c->walls[cell_id(x,y)]=(uint8_t)(x==0 || y==0 || x==GRID_W-1 || y==GRID_H-1);
}
void start_pacman(Console *c) {
    c->mode=MODE_PACMAN; c->last_game=MODE_PACMAN; c->result=RESULT_NONE;
    c->paused=0; c->ticks=0; c->lives=3; c->score[0]=c->score[1]=0;
    base_walls(c);
    /* Geometría conservada del prototipo del grupo; contrastada con docs/referencia-html.html. */
    for(unsigned x=2;x<=5;++x) { c->walls[cell_id(x,4)]=1; c->walls[cell_id(x,9)]=1; }
    for(unsigned x=10;x<=13;++x) { c->walls[cell_id(x,4)]=1; c->walls[cell_id(x,9)]=1; }
    for(unsigned y=2;y<=3;++y) { c->walls[cell_id(7,y)]=1; c->walls[cell_id(8,y)]=1; }
    for(unsigned y=10;y<=11;++y) { c->walls[cell_id(7,y)]=1; c->walls[cell_id(8,y)]=1; }
    c->pellets_left=0;
    for(unsigned y=0;y<GRID_H;++y) for(unsigned x=0;x<GRID_W;++x) {
        unsigned i=cell_id(x,y);
        c->pellets[i]=(uint8_t)!c->walls[i];
        if(c->pellets[i]) ++c->pellets_left;
    }
    c->pac[0]=(Cell){1,1}; c->pac[1]=(Cell){14,12};
    c->ghosts[0]=(Cell){7,6}; c->ghosts[1]=(Cell){8,7};
    c->pac_direction[0]=DIR_RIGHT; c->pac_direction[1]=DIR_LEFT;
    /* En la demo HTML no existen puntos bajo los jugadores y fantasmas al iniciar. */
    const Cell occupied[]={c->pac[0],c->pac[1],c->ghosts[0],c->ghosts[1]};
    for(unsigned k=0;k<4;++k) {
        if(k==1 && c->players==1) continue;
        unsigned i=cell_id(occupied[k].x,occupied[k].y);
        if(c->pellets[i]) {c->pellets[i]=0; --c->pellets_left;}
    }
}
static void take_pellet(Console *c, unsigned player) {
    unsigned i=cell_id(c->pac[player].x,c->pac[player].y);
    if(c->pellets[i]) {
        c->pellets[i]=0; --c->pellets_left; c->score[0]++; c->sound=SOUND_PICKUP;
        (void)player; /* PAC-MAN comparte puntuación de equipo. */
    }
}
static int ghost_collision(const Console *c) {
    for(unsigned i=0;i<c->players;++i) for(unsigned j=0;j<2;++j)
        if(equal(c->pac[i],c->ghosts[j])) return 1;
    return 0;
}
void pacman_tick(Console *c, uint16_t keys) {
    uint8_t d;
    for(unsigned p=0;p<c->players;++p) {
        /* Sin tecla no se desplaza: evita movimiento accidental en menú. */
        if(key_direction(keys,(int)p,&d)) {
            c->pac_direction[p]=d;
            Cell n=moved(c->pac[p],d);
            if(passable(c,n)) c->pac[p]=n;
        }
    }
    if(ghost_collision(c)) goto hit;
    for(unsigned p=0;p<c->players;++p) take_pellet(c,p);
    /* Drones: cada uno persigue al jugador asignado cada dos pasos. */
    if((c->ticks%2u)==0u) for(unsigned g=0;g<2;++g) {
        Cell pos=c->ghosts[g], target=c->pac[g%c->players];
        int best=1000; Cell next=pos;
        for(unsigned d=0;d<4;++d) {
            Cell n=moved(pos,d);
            if(!passable(c,n)) continue;
            int dx=(int)n.x-(int)target.x, dy=(int)n.y-(int)target.y;
            int dist=(dx<0?-dx:dx)+(dy<0?-dy:dy);
            if(dist<best) { best=dist; next=n; }
        }
        c->ghosts[g]=next;
    }
    if(ghost_collision(c)) goto hit;
    if(c->pellets_left==0) set_result(c,RESULT_PAC_WIN);
    return;
hit:
    c->sound=SOUND_HIT;
    if(--c->lives==0) { set_result(c,RESULT_PAC_LOSE); return; }
    c->pac[0]=(Cell){1,1}; c->pac[1]=(Cell){14,12};
    c->ghosts[0]=(Cell){7,6}; c->ghosts[1]=(Cell){8,7};
}
