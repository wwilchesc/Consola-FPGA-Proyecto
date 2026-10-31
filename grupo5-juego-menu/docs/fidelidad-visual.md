# Comparación con el HTML original — revisión final

Referencia: `docs/referencia-html.html`. Alcance exclusivo: las llamadas de dibujo
sobre el canvas de **64×64**, sin carcasa, paneles, CSS ni controles exteriores.
El HTML se conserva sin modificaciones.

## Método y resultado reproducible

`make compare-html` compila `sim/render_reference.c` y ejecuta
`sim/compare_html.cjs` con Node.js (sin paquetes adicionales). Ejecuta el script
original en un contexto con DOM mínimo y rasteriza sus llamadas enteras fillRect.
Compara índices de color con el render C; no es una prueba del navegador real.

| Fixture | Píxeles comparados | Excluidos intencionalmente | Diferencias |
|---|---:|---:|---:|
| Menú inicial | 2688 | 1408: franja del nombre y pie compacto | 0 |
| Nexo individual, cada fase 0/1 | 4048 | 48: tres sprites de 4×4 | 0 |
| Nexo doble, cada fase 0/1 | 4032 | 64: cuatro sprites de 4×4 | 0 |
| Snake individual, inicial y primer paso | 3584 | 512: HUD inferior ampliado | 0 |
| Snake doble, inicial y primer paso | 3584 | 512: HUD inferior ampliado | 0 |

Nexo usa puntuación 67 en las fixtures para comprobar los dígitos corregidos.
Sus dos fases se renderizan con posiciones fijas: no se afirma equivalencia de
trayectorias/IA. Las exclusiones son explícitas en el script; no se afirma igualdad
de todos los 4096 píxeles ni de todos los estados posibles.

## Correcciones de esta revisión

- Fuente 3×5: los dígitos 6 y 7 ahora coinciden con el original.
- Nexo individual conserva la energía en la casilla donde aparecería J2.
- El píxel cian indica que ambos exploradores comparten casilla.
- En individual, flechas y WASD controlan J1 en ambos juegos. Ante teclas
  simultáneas, la prioridad fija es arriba, derecha, abajo, izquierda.
- Nexo verifica el choque antes de recoger energía, como el HTML.
- Snake actualiza al superviviente y su puntuación antes de resolver un choque
  del rival en el mismo paso; actualiza también el indicador interno alive.
- El generador verifica explícitamente modo y cantidad de jugadores de las
  capturas principales, y comprueba la paleta sin ocultar errores con una máscara.

## Diferencias intencionales conservadas

| Aspecto | HTML | Firmware C |
|---|---|---|
| Identidad laberinto | Personajes clásicos | Nexo: exploradores y drones originales, dos fases animadas |
| Menú | Dos pantallas: juego, luego jugadores; flechas alternan | Una pantalla; arriba/abajo elige juego, izquierda/derecha jugadores; 1/2 inicia directamente |
| Inicio por Enter | Confirma cada etapa | Inicia con selección visible |
| Laberinto | Movimiento continuo, giro pendiente | Movimiento solo con tecla mantenida, sin giro en cola |
| Enemigos | Buscan al más cercano, con decisiones pseudoaleatorias | Cada dron persigue al jugador asignado; ambos a J1 en individual; desempate fijo |
| Controles | Eventos de pulsación | Máscara de teclas mantenidas; acciones de menú por flanco |
| Pausa/salida | Escape pausa primero; Enter o P reanuda; R vuelve al menú | P pausa/reanuda; Escape va al menú; R reinicia; instrucciones visibles |
| Resultados | Mensaje y récord | Juego, resultado, puntuaciones, récord de sesión e instrucciones |
| Récords | localStorage, separados por juego y número de jugadores | RAM de sesión, un máximo por juego compartido entre individual/doble |
| Snake HUD | Dos cifras mínimas; sin etiqueta individual | Tres cifras y etiqueta SOLO; saturación visual para evitar desbordar |
| Snake longitud | Puede llenar la arena y ganar | Máximo 128 segmentos por serpiente para limitar RAM; continúa puntuando, sin victoria por llenar arena |
| Tiempo | 130 ms laberinto / 155 ms Snake | Paso abstracto; 150 ms propuestos, temporizador aún pendiente |

Se conservan las paredes azules y bordes cian, ocho colores exactos, posiciones
iniciales, retícula discreta Snake, ojos blancos, cuerpos tramados, alimento rojo,
crecimiento, tres vidas compartidas, cooperación y colisiones competitivas.

## Capturas actuales

`make demo` produce PPM ampliados 6× sin interpolación. PNG revisados en
`docs/capturas/`: `menu.png`, `nexo.png`, `snake.png`, `pausa.png`,
`snake-solo.png`, `resultado.png` y mosaico `resumen.png`.
`pacman.png` es una referencia histórica previa, no una captura actual.
Las aserciones del generador evitan etiquetar Nexo como Snake o viceversa.

## Verificación y límites

Pasaron `make test`, `make demo`, `make compare-html`, `make sanitize` y
`make size-rv32`. Compilación final sin avisos (warnings tratados como errores).
Las regresiones incluyen energía individual, control con flechas, dos fases de
animación, casilla compartida y puntuación del superviviente en el turno final.
Se mantienen las comprobaciones de paleta y centinelas en 1200 renders.
ASan/UBSan pasan; LeakSanitizer sigue desactivado por el entorno ptrace.
El primer intento del comparador encontró EPERM al lanzar un subproceso Node;
se verificó después con ejecución autorizada fuera del sandbox.

No se probó en FPGA. Pendientes: contratos MMIO, temporizador, arranque y linker,
main embebido, libgcc, integración y validación del SoC, síntesis y prueba en placa.
