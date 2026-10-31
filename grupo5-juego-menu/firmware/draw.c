#include "internal.h"

static void dot(uint8_t *f,int x,int y,uint8_t color) {
    if(x>=0 && x<64 && y>=0 && y<64) f[(unsigned)y*64u+(unsigned)x]=color;
}
static void rect(uint8_t *f,int x,int y,int w,int h,uint8_t color) {
    for(int yy=y;yy<y+h;++yy) for(int xx=x;xx<x+w;++xx) dot(f,xx,yy,color);
}
/* Fuente compacta 3x5 para interfaz legible en 64x64. */
static void glyph(char c,uint8_t r[5]) {
    const char *src="000000000000000";
    switch(c) {
    case 'A':src="010101111101101";break;
    case 'B':src="110101110101110";break;
    case 'C':src="011100100100011";break;
    case 'D':src="110101101101110";break;
    case 'E':src="111100110100111";break;
    case 'F':src="111100110100100";break;
    case 'G':src="011100101101011";break;
    case 'H':src="101101111101101";break;
    case 'I':src="111010010010111";break;
    case 'J':src="001001001101010";break;
    case 'K':src="101101110101101";break;
    case 'L':src="100100100100111";break;
    case 'M':src="101111111101101";break;
    case 'N':src="101111111111101";break;
    case 'O':src="010101101101010";break;
    case 'P':src="110101110100100";break;
    case 'Q':src="010101101111011";break;
    case 'R':src="110101110101101";break;
    case 'S':src="011100010001110";break;
    case 'T':src="111010010010010";break;
    case 'U':src="101101101101111";break;
    case 'V':src="101101101101010";break;
    case 'W':src="101101111111101";break;
    case 'X':src="101101010101101";break;
    case 'Y':src="101101010010010";break;
    case 'Z':src="111001010100111";break;
    case '0':src="111101101101111";break;
    case '1':src="010110010010111";break;
    case '2':src="110001111100111";break;
    case '3':src="110001011001110";break;
    case '4':src="101101111001001";break;
    case '5':src="111100110001110";break;
    case '6':src="011100111101111";break;
    case '7':src="111001001001001";break;
    case '8':src="111101111101111";break;
    case '9':src="111101111001110";break;
    case ':':src="000010000010000";break;
    case '-':src="000000111000000";break;
    default:break;
    }
    for(unsigned y=0;y<5;++y) {
        r[y]=0;
        for(unsigned x=0;x<3;++x) if(src[y*3+x]=='1') r[y]|=(uint8_t)(1u<<(2u-x));
    }
}
static void text(uint8_t *f,int x,int y,const char *s,uint8_t color) {
    while(*s && x<64) {
        uint8_t r[5]; glyph(*s++,r);
        for(unsigned yy=0;yy<5;++yy) for(unsigned xx=0;xx<3;++xx)
            if(r[yy]&(1u<<(2u-xx))) dot(f,x+(int)xx,y+(int)yy,color);
        x+=4;
    }
}
static void number_fixed(uint8_t *f,int x,int y,unsigned val,unsigned digits,uint8_t col) {
    char b[6];
    unsigned max=1;
    for(unsigned i=0;i<digits && i<5;++i) max*=10;
    if(val>=max) val=max-1;
    if(digits>sizeof(b)-1u) digits=sizeof(b)-1u;
    for(unsigned i=0;i<digits;++i) { b[digits-i-1u]=(char)('0'+val%10u); val/=10u; }
    b[digits]='\0';
    text(f,x,y,b,col);
}
/* Gráficos internos 64x64 derivados del prototipo del grupo.
   No incluyen el marco ni los controles CSS externos del navegador. */
static void centered(uint8_t *f,const char *s,int y,uint8_t color) {
    unsigned len=0;
    while(s[len]) ++len;
    int x=(64-(int)len*4+1)/2;
    text(f,x,y,s,color);
}
static void pac_sprite(uint8_t *f,Cell p,unsigned player,unsigned direction,unsigned ticks) {
    int x=(int)p.x*4, y=(int)p.y*4;
    uint8_t col=player==0?COL_YELLOW:COL_GREEN;
    /* Explorador: casco rectangular, visor orientado y dos botas. */
    rect(f,x,y,4,3,col);
    rect(f,x,y+3,1,1,col); rect(f,x+3,y+3,1,1,col);
    rect(f,x+1,y+1,2,1,COL_BLACK);
    dot(f,x+(direction==DIR_LEFT?1:2),y+1,COL_WHITE);
    if(ticks%2u) dot(f,x+3,y,COL_CYAN);
}
static void draw_pac(const Console *c,uint8_t *f) {
    for(unsigned y=0;y<GRID_H;++y) for(unsigned x=0;x<GRID_W;++x) {
        unsigned i=cell_id(x,y);
        if(c->walls[i]) {
            rect(f,(int)x*4,(int)y*4,4,4,COL_BLUE);
            if(y>0&&!c->walls[cell_id(x,y-1)])
                rect(f,(int)x*4,(int)y*4,4,1,COL_CYAN);
        } else if(c->pellets[i]) dot(f,(int)x*4+2,(int)y*4+2,COL_WHITE);
    }
    for(unsigned i=0;i<2;++i) {
        int x=c->ghosts[i].x*4, y=c->ghosts[i].y*4;
        uint8_t color=i==0?COL_RED:COL_MAGENTA;
        /* Dron en X, núcleo luminoso; sin silueta de fantasma. */
        dot(f,x,y,color); dot(f,x+3,y,color);
        dot(f,x,y+3,color); dot(f,x+3,y+3,color);
        rect(f,x+1,y+1,2,2,color);
        dot(f,x+1+(int)(c->ticks%2u),y+1,COL_WHITE);
    }
    for(unsigned i=0;i<c->players;++i)
        pac_sprite(f,c->pac[i],i,c->pac_direction[i],c->ticks);
    if(c->players==2 && equal(c->pac[0],c->pac[1]))
        dot(f,c->pac[0].x*4,c->pac[0].y*4,COL_CYAN);
    rect(f,0,56,64,8,COL_BLACK);
    text(f,0,58,"P:",COL_WHITE);
    number_fixed(f,8,58,c->score[0]%1000u,3,COL_WHITE);
    text(f,36,58,"V:",COL_YELLOW);
    number_fixed(f,44,58,c->lives,1,COL_YELLOW);
}
static void draw_snake(const Console *c,uint8_t *f) {
    for(int x=0;x<32;++x) for(int y=0;y<28;++y)
        if((x+y)%8==0) dot(f,x*2,y*2,COL_BLUE);
    rect(f,(int)c->apple.x*2,(int)c->apple.y*2,2,2,COL_RED);
    for(unsigned p=0;p<c->players;++p) {
        const Snake *sn=&c->snake[p];
        uint8_t color=p==0?COL_YELLOW:COL_GREEN;
        for(unsigned i=0;i<sn->length;++i) {
            Cell s=sn->body[i];
            int x=(int)s.x*2, y=(int)s.y*2;
            rect(f,x,y,2,2,color);
            if(i>0 && i%2==0) dot(f,x,y,COL_BLACK);
        }
        Cell h=sn->body[0];
        int hx=h.x*2+(sn->direction==DIR_RIGHT?1:0);
        int hy=h.y*2+(sn->direction==DIR_DOWN?1:0);
        dot(f,hx,hy,COL_WHITE);
    }
    rect(f,0,56,64,8,COL_BLACK);
    text(f,0,58,"1:",COL_YELLOW);
    number_fixed(f,8,58,c->score[0],3,COL_YELLOW);
    if(c->players==2) {
        text(f,36,58,"2:",COL_GREEN);
        number_fixed(f,44,58,c->score[1],3,COL_GREEN);
    } else text(f,36,58,"SOLO",COL_CYAN);
}
static void draw_result(const Console *c,uint8_t *f) {
    centered(f,c->last_game==MODE_SNAKE?"SNAKE":"NEXO",3,COL_CYAN);
    if(c->result==RESULT_PAC_WIN) centered(f,"VICTORIA",13,COL_GREEN);
    else if(c->result==RESULT_PAC_LOSE || c->result==RESULT_SOLO_LOSE)
        centered(f,"DERROTA",13,COL_RED);
    else if(c->result==RESULT_P1_WIN) centered(f,"GANA J1",13,COL_YELLOW);
    else if(c->result==RESULT_P2_WIN) centered(f,"GANA J2",13,COL_GREEN);
    else centered(f,"EMPATE",13,COL_WHITE);
    text(f,2,23,"1",COL_YELLOW); number_fixed(f,9,23,c->score[0],4,COL_YELLOW);
    if(c->players==2 && c->last_game==MODE_SNAKE) {
        text(f,35,23,"2",COL_GREEN); number_fixed(f,43,23,c->score[1],4,COL_GREEN);
    }
    centered(f,"RECORD SESION",33,COL_WHITE);
    number_fixed(f,24,41,c->records[c->last_game==MODE_SNAKE?1:0],4,COL_YELLOW);
    centered(f,"ENTER REPETIR",51,COL_GREEN);
    centered(f,"ESC MENU",58,COL_CYAN);
}
void console_render(const Console *c,uint8_t f[SCREEN_W*SCREEN_H]) {
    for(unsigned i=0;i<SCREEN_W*SCREEN_H;++i) f[i]=COL_BLACK;
    if(c->mode==MODE_MENU) {
        centered(f,"RETRO FPGA",3,COL_YELLOW);
        rect(f,4,11,56,1,COL_CYAN);
        centered(f,"ELIGE JUEGO",15,COL_WHITE);
        rect(f,3,c->selection==0?24:36,58,9,COL_YELLOW);
        centered(f,"1 NEXO",26,c->selection==0?COL_BLACK:COL_WHITE);
        centered(f,"2 SNAKE",38,c->selection==1?COL_BLACK:COL_WHITE);
        centered(f,c->players==1?"JUGADORES: 1":"JUGADORES: 2",47,COL_GREEN);
        centered(f,"ENTER INICIAR",57,COL_CYAN);
    } else if(c->mode==MODE_RESULT) {
        draw_result(c,f);
    } else {
        if(c->mode==MODE_PACMAN) draw_pac(c,f);
        else draw_snake(c,f);
        if(c->paused) {
            rect(f,4,18,56,34,COL_BLUE);
            centered(f,"PAUSA",21,COL_YELLOW);
            centered(f,"P CONTINUAR",30,COL_WHITE);
            centered(f,"R REINICIAR",38,COL_GREEN);
            centered(f,"ESC MENU",46,COL_CYAN);
        }
    }
}
