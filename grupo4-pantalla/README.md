# Grupo 4 · Pantalla

Controlador del panel LED 64×64 y memoria de imagen (framebuffer).

## Integrantes

-
-
-

## Qué hace este grupo

_(Describir el periférico y su función dentro de la consola.)_

- Dispositivo(s): Panel LED RGB 64×64 HUB75
- Protocolo: HUB75
- Dirección base en el mapa de memoria: `0x48_0000` framebuffer · `0x4C_0000` control

## Tareas

- [ ] Estudiar el panel HUB75 (barrido, BCM)
- [ ] Definir el formato de los píxeles y el framebuffer
- [ ] Diseñar el barrido del panel
- [ ] Definir los registros de control
- [ ] Escribir el testbench
- [ ] Probar en la placa (patrón de prueba)

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
