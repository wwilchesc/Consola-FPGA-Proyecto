/* Ejecuta el JS original sin navegador. Solo intercepta fillRect entero sobre
   64x64; no prueba CSS, eventos del navegador ni temporización real. */
const fs=require('node:fs'), vm=require('node:vm'), cp=require('node:child_process');
const assert=require('node:assert/strict');
const palette=['#000000','#0000ff','#00ff00','#00ffff','#ff0000','#ff00ff','#ffff00','#ffffff'];
const pixels=new Uint8Array(4096);
const cx={fillStyle:'#000000',fillRect(x,y,w,h){
  const color=palette.indexOf(this.fillStyle); assert(color>=0);
  for(let yy=y;yy<y+h;yy++)for(let xx=x;xx<x+w;xx++)
    if(xx>=0&&xx<64&&yy>=0&&yy<64)pixels[yy*64+xx]=color;
}};
const element=()=>({getContext:()=>cx,classList:{remove(){},add(){},toggle(){}},
  setAttribute(){},addEventListener(){},style:{}});
const sandbox={document:{getElementById:()=>element(),addEventListener(){}},localStorage:{getItem:()=>0,setItem(){}},
  addEventListener(){},requestAnimationFrame(){}};
vm.createContext(sandbox);
const html=fs.readFileSync('docs/referencia-html.html','utf8');
vm.runInContext(html.match(/<script>([\s\S]*?)<\/script>/)[1],sandbox);
const raw=cp.execFileSync('./build/render_reference');
let index=0;
function compare(name,code,exclude){
  vm.runInContext(code,sandbox);
  const actual=raw.subarray(index*4096,++index*4096);assert.equal(actual.length,4096);
  let checked=0, differences=0, excluded=0;
  for(let y=0;y<64;y++)for(let x=0;x<64;x++){
    if(exclude(x,y)){excluded++;continue;}
    checked++; if(actual[y*64+x]!==pixels[y*64+x])differences++;
  }
  console.log(`${name}: ${checked} comparados, ${excluded} excluidos, ${differences} diferencias`);
  assert.equal(differences,0,name);
}
// Nombre Nexo y pie compacto son diferencias deliberadas.
compare('menu',"menu()",(x,y)=>(y>=26&&y<31)||y>=47);
for(const n of [1,2]){
  vm.runInContext(`playerCount=${n};selected='pac';start();state.score=67`,sandbox);
  // Excluir SOLO rectángulos de sprites originales sustituidos por Nexo.
  const sprite=(x,y)=>[[1,1],...(n===2?[[14,12]]:[]),[7,6],[8,7]]
    .some(([sx,sy])=>x>=sx*4&&x<sx*4+4&&y>=sy*4&&y<sy*4+4);
  for(let frame=0;frame<2;frame++)compare(`nexo ${n}J fase ${frame}`,`ticks=${frame};draw()`,sprite);
  vm.runInContext(`playerCount=${n};selected='snake';start()`,sandbox);
  // HUD extendido C: tres cifras y SOLO; arena se compara completa.
  compare(`snake ${n}J inicial`,'draw()',(_x,y)=>y>=56);
  compare(`snake ${n}J paso 1`,'ticks++;snakeTick();draw()',(_x,y)=>y>=56);
}
assert.equal(index*4096,raw.length);
