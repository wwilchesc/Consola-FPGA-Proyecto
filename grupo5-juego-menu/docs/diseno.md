# Diseño y verificación — 31-10-2026

## Revisión inicial

El prototipo tenía menú, dos juegos exclusivamente para dos jugadores, pausa,
resultados/repetición, HUD inferior, fuente 3×5 y render RGB de ocho colores.
No tenía selección de jugadores, separación modular, audio ni récord funcional.
La documentación antigua decía diez puntos por alimento y puntuaciones separadas
en el laberinto; el código realmente otorgaba un punto y puntuación de equipo.
Se conservan las reglas del código.

## Organización

| Archivo | Responsabilidad |
|---|---|
| console.c / console.h | Estado público, menú, pausa, reinicio y coordinación |
| maze.c | Nexo: paredes, energía, exploradores y drones |
| snake.c | Movimiento, crecimiento, alimento y colisiones simultáneas |
| controls.c | Máscara abstracta a dirección; sin scan codes supuestos |
| draw.c | Fuente, sprites y render de 4096 píxeles |
| services.c | Resultados, récords de sesión y evento de sonido |
| platform.c / platform.h | Adaptadores de periféricos y paso temporizado |
| internal.h | Ayudas geométricas e interfaz privada |
| demo_host.c | Capturas en Linux, único módulo con stdio |

No hay asignación dinámica ni recursión. Se conservan nombres PAC en la API para
compatibilidad. Nexo introduce cascos con visor y drones en X dibujados para este
proyecto, sin sprites oficiales. Conserva la geometría y el detalle cian de las
paredes del prototipo; no se añadió ningún mapa externo. Los drones persiguen al
jugador asignado cada dos pasos; en individual ambos persiguen al primero.

La partida permanece inmóvil en pausa. P reanuda sin avanzar ese mismo paso.
R reinicia durante partida/pausa; Enter repite desde resultado. Se conserva la
cantidad de jugadores. Volver al menú no borra récords; apagar/reinicializar sí.
Solo se registran partidas terminadas. No hay top 5, iniciales ni EEPROM todavía.

Snake conserva amarillo/verde, ojo blanco, cuerpo tramado y alimento rojo.
Añade HUD de tres cifras y etiqueta SOLO, resultados con puntuaciones y récord
real de sesión. Los valores que exceden la anchura de texto se saturan visualmente;
la puntuación interna de Snake se satura en 65535. La longitud máxima es 128.

## Verificación ejecutada

- `make test` y `make demo`: pasan con C11, warnings tratados como errores.
- Pruebas existentes conservadas, adaptando la expectativa del sprite nuevo.
- Nuevas pruebas: elección individual/doble, exclusión de J2 en individual,
  victoria/derrota Nexo, derrota Snake individual, ganadores/empate competitivo,
  choque de cabezas, crecimiento, pausa, reinicio, récord y adaptadores.
- 1200 renders de partidas de ambos juegos/modos verifican colores 0..7 y
  centinelas antes/después de los 4096 bytes. Revisión visual de seis capturas.
- `make sanitize`: AddressSanitizer y UndefinedBehaviorSanitizer pasan.
  LeakSanitizer se desactiva por incompatibilidad del entorno con ptrace;
  el motor no usa memoria dinámica.
- `make size-rv32`: los siete módulos compilan con `-march=rv32i -mabi=ilp32
  -Os -ffreestanding -fno-builtin`. No se ejecutaron en RISC-V ni en placa.

## Memoria: mediciones y estimaciones

Medido: `sizeof(Console)` = 1024 bytes en host; framebuffer lógico = 4096 bytes.
No se incluyen dos framebuffers ni la memoria interna del controlador HUB75.
Los objetos RV32I suman **8376 bytes** de código/constantes y cero datos globales:
console 400, controls 124, maze 1432, snake 1616, draw 4520, services 128,
platform 156. El estado y framebuffer los aporta el llamador; cero BSS en los
objetos no significa que el programa no necesite RAM.

Estimación base si todo reside en BRAM: 8376 + 1024 + 4096 = **13496 bytes**,
antes de arranque, libgcc, pila, drivers, alineación y buffers del hardware.
Los informes `.su` muestran marcos individuales de hasta 96 bytes; no son el
máximo acumulado de pila. Reservar provisionalmente 1 KiB de pila da 14520 bytes
antes de las otras dependencias, sin demostrar que ese presupuesto sea suficiente.
El render es síncrono y su tiempo aún no se midió sobre FemtoRV32.

Pendiente: linker/map final, medición de pila, utilización BRAM del SoC completo
en síntesis para Tang Nano 9K y arbitraje Von Neumann. No se afirma que la
propuesta de 32 KiB del mapa global esté disponible ni que el diseño ya quepa.

## Revisión con HTML original

La referencia ya está disponible. Véase `fidelidad-visual.md` para comparación
píxel a píxel, correcciones y diferencias deliberadas. Las cifras anteriores
se actualizaron con los objetos RV32I de esta revisión.
