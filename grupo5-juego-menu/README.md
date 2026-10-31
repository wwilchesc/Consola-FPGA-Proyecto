# Grupo 5 · Juego y menú

Firmware C sin sistema operativo para integrar con FemtoRV32 en Tang Nano 9K.
Validado en Linux y compilado a objetos RV32I; **no probado en FPGA**.

- **Nexo**: explorar el laberinto, recoger energía y evitar drones. Un jugador o
  dos en cooperación, tres vidas compartidas y puntuación de equipo.
- **Snake**: individual o competitivo de dos jugadores, un punto por alimento,
  crecimiento hasta 128 segmentos, colisiones simultáneas y empate.

## Controles abstractos

| Contexto | Teclas |
|---|---|
| Menú: juego | Arriba/abajo o W/S |
| Menú: jugadores | Izquierda/A = uno; derecha/D = dos |
| Iniciar | Enter; 1/2 inician directamente el juego con la cantidad elegida |
| Jugador 1 / jugador 2 | WASD / flechas (en individual, ambas controlan J1) |
| Partida | P pausa/reanuda; R reinicia; Escape vuelve al menú |
| Resultado | Enter repite; Escape vuelve al menú |

La selección inicial conserva dos jugadores. Las acciones detectan flancos:
soltar la tecla antes de repetir. El adaptador PS/2 debe entregar teclas mantenidas.

## Verificación reproducible

```sh
make test
make demo
make compare-html  # requiere Node.js; compara el canvas original
make sanitize
make size-rv32
```

`demo` genera PPM ampliados 6× sin interpolación en `build/`: menú, Nexo,
Snake, pausa, Snake individual y resultado. PNG revisados en `docs/capturas/`.
`size-rv32` requiere `riscv64-unknown-elf-gcc` (configurable con RISCV_CC),
y produce objetos RV32I/ILP32; no una imagen arrancable.

Ver [diseño y memoria](docs/diseno.md), [integración](docs/contratos-MMIO.md)
y [estado de la referencia HTML](docs/fidelidad-visual.md).
