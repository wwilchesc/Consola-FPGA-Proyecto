# Consola de videojuegos retro en FPGA

Proyecto del curso **Electrónica Digital 1** — Universidad Nacional de Colombia.

Vamos a construir una consola de videojuegos retro completa dentro de una FPGA: un procesador RISC-V propio, una pantalla LED de 64×64, teclado, sonido y memoria para guardar récords. Todo el hardware se describe en Verilog y el software (menú y juegos) corre sobre el procesador.

## Qué va a hacer la consola

- Al encender muestra un logo, revisa sus memorias y abre un **menú de juegos**.
- El jugador elige un juego con el **teclado PS/2** y juega en el **panel LED 64×64**.
- Cada evento tiene su **sonido** (moneda, daño, ganar, perder, menú).
- Los **récords** (top 5 por juego, con iniciales) se guardan en una memoria que no se borra al apagar.
- Una sola pantalla: la escena del juego y una barra superior con puntos y vidas.

## Plataforma

| Elemento | Elección |
| --- | --- |
| Tarjeta | Sipeed Tang Primer 25K (FPGA Gowin GW5A-LV25) + Dock |
| Procesador | FemtoRV32 Quark (RISC-V RV32I), arquitectura Von Neumann |
| Periféricos | Mapeados en memoria (MMIO); el software los atiende por sondeo |
| Flujo de herramientas | Yosys → nextpnr-himbaechel → Apycula → openFPGALoader (todo abierto) |
| Simulación | Icarus Verilog + GTKWave |
| Reloj | 50 MHz |

## Arquitectura general

```
                 ┌──────────────────────── FPGA ────────────────────────┐
 Teclado PS/2 ──►│ Receptor PS/2 ─┐                                     │
                 │                │                                     │
                 │   FemtoRV32 ◄──┼── Bus + decodificador de direcciones│
                 │   + BRAM       │                                     │
                 │                ├── Framebuffer + barrido HUB75 ──────┼──► Panel LED 64×64
                 │                ├── Audio I2S ────────────────────────┼──► MAX98357A ► parlante
                 │                ├── Controladores SPI / I2C ──────────┼──► SPI-RAM, Flash, EEPROM
                 │                └── UART ─────────────────────────────┼──► PC (depuración)
                 └──────────────────────────────────────────────────────┘
```

## División del trabajo

| Grupo | Tarea | Carpeta |
| --- | --- | --- |
| 1 | Memoria externa (SPI-RAM, Flash, EEPROM y récords) | [`grupo1-memoria-externa`](grupo1-memoria-externa) |
| 2 | Teclado PS/2 | [`grupo2-teclado-ps2`](grupo2-teclado-ps2) |
| 3 | Audio (I2S hacia el amplificador) | [`grupo3-audio`](grupo3-audio) |
| 4 | Pantalla (panel LED 64×64 HUB75 y framebuffer) | [`grupo4-pantalla`](grupo4-pantalla) |
| 5 | Juego y menú (software) | [`grupo5-juego-menu`](grupo5-juego-menu) |
| 6 | Inicio: integración del SoC, UART y arranque | [`grupo6-inicio-integracion`](grupo6-inicio-integracion) |

Cada grupo tiene su carpeta con un `README.md` para describir lo que hizo, y las subcarpetas:

- `docs/` diagramas, cálculos, hojas de datos, fotos y capturas
- `rtl/` código Verilog del módulo (en el grupo 5, `firmware/` con el código C)
- `sim/` testbench y resultados de simulación

En `docs/` (raíz) van los documentos generales del proyecto: diagramas de contexto, arquitectura y mapa de memoria.

## Mapa de memoria propuesto

Ventanas de 64 KB; registros cada 4 bytes. El grupo 6 lo mantiene actualizado.

| Dirección base | Bloque | Grupo |
| --- | --- | --- |
| `0x00_0000` | BRAM: programa, datos y pila (32 KB) | 6 |
| `0x40_0000` | UART de depuración | 6 |
| `0x41_0000` | Controlador SPI-RAM | 1 |
| `0x42_0000` | Controlador SPI-Flash | 1 |
| `0x43_0000` | Receptor de teclado PS/2 | 2 |
| `0x46_0000` | Maestro I2C (EEPROM) | 1 |
| `0x47_0000` | Audio I2S | 3 |
| `0x48_0000` | Framebuffer 64×64 | 4 |
| `0x4C_0000` | Control del panel (CONTROL, FRAMES) | 4 |

## Contrato común de los periféricos

Todos los módulos se conectan al bus con los mismos puertos, para que el grupo 6 los pueda integrar sin cambios:

```verilog
module perip_xxx (
    input         clk, rst,
    input  [31:0] d_in,     // dato escrito por la CPU
    input         cs,       // selección (lo genera el decodificador)
    input  [4:0]  addr,     // desplazamiento del registro
    input         rd, wr,
    output [31:0] d_out     // dato leído (registrado, 1 ciclo)
    // + los pines propios del periférico
);
```

## Cómo trabajar en el repositorio

1. Cada grupo trabaja **solo dentro de su carpeta**.
2. Se recomienda una rama por grupo (`grupo1`, `grupo2`…) y abrir un *pull request* para unir a `main`.
3. Todo módulo llega con su testbench y una tabla de registros en su README.
4. Cambios al mapa de memoria o a los pines se acuerdan con el grupo 6.

## Estado

| Grupo | Diseño | Verilog / código | Simulación | Probado en la placa |
| --- | --- | --- | --- | --- |
| 1 Memoria externa | ⬜ | ⬜ | ⬜ | ⬜ |
| 2 Teclado PS/2 | ⬜ | ⬜ | ⬜ | ⬜ |
| 3 Audio | ⬜ | ⬜ | ⬜ | ⬜ |
| 4 Pantalla | ⬜ | ⬜ | ⬜ | ⬜ |
| 5 Juego y menú | ⬜ | ⬜ | ⬜ | ⬜ |
| 6 Inicio | ⬜ | ⬜ | ⬜ | ⬜ |

(Cambiar ⬜ por ✅ al completar cada etapa.)
