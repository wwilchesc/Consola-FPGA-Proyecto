# Grupo 1 · Memoria externa

Controladores de las memorias externas de la consola y la persistencia de los récords.

## Integrantes

-
-
-

## Qué hace este grupo

_(Describir el periférico y su función dentro de la consola.)_

- Dispositivo(s): SPI-RAM (memoria de trabajo), Flash SPI de la tarjeta (gráficos), EEPROM I2C (récords que no se borran al apagar)
- Protocolo: SPI e I2C
- Dirección base en el mapa de memoria: `0x41_0000` SPI-RAM · `0x42_0000` Flash · `0x46_0000` I2C

## Tareas

- [ ] Estudiar los protocolos SPI e I2C y las hojas de datos de las memorias
- [ ] Definir los registros de cada controlador
- [ ] Diseñar el controlador SPI
- [ ] Diseñar el controlador I2C
- [ ] Definir el formato de la tabla de récords
- [ ] Escribir los testbenches
- [ ] Probar en la placa

## Tareas y actividades

Las respuestas a las actividades (clasificación del protocolo, diagrama de funcionamiento, pines, registros y lista de precios) están en su propia carpeta:

**[Abrir la carpeta de tareas y actividades](tareas-y-actividades/)**

## Diseño

Diagramas de flujo (clic para abrirlos en draw.io):

- [Diagrama de flujo desde el punto de vista del usuario](https://app.diagrams.net/#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fwwilchesc%2FConsola-FPGA-Proyecto%2Fmain%2Fgrupo1-memoria-externa%2Fdocs%2FGrupo1_Flujo_Usuario.drawio)
- [Diagrama de flujo de lo que hace la máquina](https://app.diagrams.net/#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fwwilchesc%2FConsola-FPGA-Proyecto%2Fmain%2Fgrupo1-memoria-externa%2Fdocs%2FGrupo1_Flujo_Maquina.drawio)

_(Agregar aquí diagrama de bloques y cálculos. Guardar los archivos en `docs/`.)_

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
| 22/10/2026 | Diagramas de flujo del grupo: [vista del usuario](https://app.diagrams.net/#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fwwilchesc%2FConsola-FPGA-Proyecto%2Fmain%2Fgrupo1-memoria-externa%2Fdocs%2FGrupo1_Flujo_Usuario.drawio) y [lo que hace la máquina](https://app.diagrams.net/#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fwwilchesc%2FConsola-FPGA-Proyecto%2Fmain%2Fgrupo1-memoria-externa%2Fdocs%2FGrupo1_Flujo_Maquina.drawio) |
| 22/10/2026 | Tarea de actividades para todos los grupos: clasificación de SPI e I2C, diagrama de funcionamiento, mapa de pines, registros MMIO y lista de compras con precios ([ver carpeta](tareas-y-actividades/)) |

## Pendientes y problemas

-
