# Contratos MMIO y pendientes de integración

## Direcciones publicadas actualmente en README raíz

| Base | Dispositivo | Responsable | Estado |
|---|---|---|---|
| `0x000000` | BRAM / programa, datos y pila | 6 | Tamaño propuesto 32 KiB; **recalcular para Tang Nano 9K** |
| `0x400000` | UART | 6 | Falta tabla de registros |
| `0x410000` | SPI-RAM | 1 | Falta tabla de registros y protocolo de espera |
| `0x420000` | SPI-Flash | 1 | Falta tabla de registros |
| `0x430000` | PS/2 | 2 | Falta mapa de teclas, estado y confirmación de lectura |
| `0x460000` | I2C EEPROM | 1 | Falta contrato de récords |
| `0x470000` | Audio I2S | 3 | Falta contrato de sonidos |
| `0x480000` | Framebuffer | 4 | Falta formato / granularidad / arbitraje |
| `0x4C0000` | Control de pantalla | 4 | Falta tabla de registros |

**Estas direcciones son propuestas extraídas del README principal, NO son definitivas ni garantizan que el hardware esté implementado.** El grupo 6 mantiene el mapa y el decodificador. La elección de Tang Nano 9K no obliga por sí misma a cambiar las direcciones, pero obliga a comprobar límites del CPU, memoria y temporización.

## Cinco acuerdos críticos antes de compilar para FPGA

1. **Grupo 4 / framebuffer:** ¿un píxel equivale a 1 byte?, ¿el bus admite escrituras de 8 bits o solo palabras de 32?, ¿se requiere doble buffer?, ¿cómo evita rasgado?, ¿quién arbitra las lecturas simultáneas del panel?
2. **Grupo 2 / teclado:** ¿se lee un registro con máscara de teclas mantenidas, o una cola de scan codes?, ¿cómo se informan key releases (`F0`) y teclas extendidas (`E0`)?, ¿pueden funcionar WASD + flechas simultáneamente?
3. **Grupo 3 / sonido:** ¿cómo solicita el software el efecto de menú, punto, choque y victoria? Definir frecuencia, duración, control o ID de efecto.
4. **Grupo 1 / récords:** ¿cuál es el formato de cinco récords con iniciales por juego?, ¿cómo se manejan tiempos de escritura EEPROM?, ¿qué API se usará desde C?
5. **Grupo 6 / sistema:** ¿cuáles son los archivos `crt0.S`, `linker.ld`, compilador `riscv32-unknown-elf-gcc`, MMIO y timer?, ¿dónde comienza el firmware?, ¿cómo se carga en BRAM y cómo se manejan lecturas con latencia?

## Interfaz propuesta del firmware, independiente del hardware

El archivo `firmware/console.h` recibe una máscara abstracta de teclas (`K_W`, `K_A`, etc.) y produce una imagen lógica de 4096 bytes de índice de color. Esto permite avanzar sin asumir nombres ni bits de registros MMIO que los otros grupos todavía no han definido.

**No escribir directamente a `0x480000` con un puntero `uint8_t*` hasta que el grupo 4 confirme que el framebuffer permite escrituras de byte en ese rango.** En un bus RV32 de 32 bits, esa elección depende del hardware y del soporte de `byte enable` o del empaquetamiento elegido.

## Verificación del mapa y arquitectura Von Neumann

FemtoRV utiliza una interfaz principal de CPU para instrucciones y datos. Una memoria de programa en BRAM, framebuffer y periféricos MMIO pueden tener lógica interna independiente sin convertir el CPU en Harvard: el software sigue accediendo a un espacio de direcciones unificado.

Para la Tang Nano 9K se debe medir la utilización real en Yosys y confirmar el presupuesto de memoria (firmware, datos, pila, framebuffer y FIFO). Los 32 KiB del README eran **una propuesta previa**, no una reserva garantizada.

## Adaptador implementado en esta revisión

`platform.h` define callbacks de lectura de teclas mantenidas, presentación del
framebuffer lógico y envío de eventos Sound. El contexto del adaptador permite
pasar una configuración de direcciones/registros acordada sin recompilar la lógica.
No se realizan accesos físicos MMIO todavía: no hay offsets ni anchos confirmados.
Los callbacks opcionales permiten arrancar una simulación sin audio. El efecto del
paso se consume una vez; si hay varios eventos, el resultado/choque tiene prioridad.
Los récords implementados son máximos de sesión, no almacenamiento persistente.

Para producir firmware ejecutable faltan crt0.S, linker.ld, main con estado y
framebuffer estáticos, timer, implementaciones de callbacks MMIO, traducción PS/2
(incluido R), empaquetado de pantalla y contrato de audio/EEPROM. Los objetos RV32I
requieren las rutinas de división/módulo de libgcc (`__udivsi3`, `__umodsi3`,
`__modsi3`). El grupo 6 debe confirmar ISA, ABI, memoria y conversión ELF a imagen
BRAM. Después hacen falta simulación del SoC, síntesis/timing y prueba física.
