# Grupo 6 · Inicio (integración)

Une todo: procesador, memoria interna, bus, mapa de memoria, UART, pines y bitstream; secuencia de arranque de la consola.

## Integrantes

- Andres Felipe Sanchez Rodriguez
- Daniel Galeano Herrera
- Nirvana 
- Pablo

## Qué hace este grupo

_(Describir el periférico y su función dentro de la consola.)_

- Dispositivo(s): Tang Primer 25K, Conversor USB-serie de la tarjeta (UART)
- Protocolo: Bus del FemtoRV32, MMIO y UART
- Dirección base en el mapa de memoria: `0x00_0000` BRAM · `0x40_0000` UART

## Tareas

- [ ] Montar el SoC base (FemtoRV32 + BRAM + UART)
- [ ] Mantener el mapa de memoria y el decodificador
- [ ] Asignar los pines (.cst)
- [ ] Definir la secuencia de arranque
- [ ] Integrar los módulos de cada grupo
- [ ] Generar el bitstream y probar en la placa

## Diseño

_(Diagrama de bloques, diagrama de flujo, cálculos. Guardar los archivos en `docs/`.)_

## Registros

| Desplazamiento | Nombre | Acceso | Descripción |
| --- | --- | --- | --- |
| 0x00 | | | |

## Pines

| Señal | Pin FPGA | Conector | Nota |
| --- | --- | --- | --- |
| | | | |

## Simulación

_(Cómo correr el testbench y qué se verificó. Archivos en `sim/`.)_

## Avances

| Fecha | Avance |
| --- | --- |
| | |

## Pendientes y problemas

-
