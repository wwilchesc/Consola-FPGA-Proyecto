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

Respuestas a las **actividades para todos los grupos** (arquitectura digital del periférico).

### 1. Clasificación del protocolo

| Protocolo | Se usa en | ¿Síncrono o asíncrono? | ¿Half o full-duplex? |
| --- | --- | --- | --- |
| **SPI** (modo 0) | SPI-RAM 23LC1024 y Flash de la tarjeta | **Síncrono**: la FPGA (maestro) genera una línea de reloj dedicada, **SCK**. No hay baud rate. | **Full-duplex**: hay una línea para cada sentido, **MOSI** (FPGA → memoria) y **MISO** (memoria → FPGA), y ambas pueden moverse en el mismo pulso de reloj. |
| **I2C** | EEPROM 24LC256 (récords) | **Síncrono**: la FPGA genera el reloj **SCL**. | **Half-duplex**: una sola línea de datos, **SDA**, que se turnan maestro y memoria (por ejemplo, la memoria responde con ACK por la misma línea). |

### 2. Diagrama de funcionamiento (según las hojas de datos)

- [Diagrama de bloques, señales y tiempos](https://app.diagrams.net/#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fwwilchesc%2FConsola-FPGA-Proyecto%2Fmain%2Fgrupo1-memoria-externa%2Fdocs%2FGrupo1_Diagrama_Funcionamiento.drawio) (3 páginas: bloques, tabla de señales con los tiempos de las hojas de datos y diagramas de tiempos SPI e I2C)
- [Diagrama de flujo de lo que hace la máquina](https://app.diagrams.net/#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fwwilchesc%2FConsola-FPGA-Proyecto%2Fmain%2Fgrupo1-memoria-externa%2Fdocs%2FGrupo1_Flujo_Maquina.drawio)

Resumen de las señales:

| Señal | Sentido | Qué lleva |
| --- | --- | --- |
| SCK | FPGA → SPI-RAM / Flash | Reloj SPI (12,5 MHz; máx. 20 MHz en la 23LC1024) |
| MOSI | FPGA → SPI-RAM / Flash | Comando (0x03 leer, 0x02 escribir…), dirección de 24 bits y datos |
| MISO | SPI-RAM / Flash → FPGA | Datos leídos |
| CS | FPGA → SPI-RAM / Flash | Selección, activo en bajo; una transacción por cada CS bajo |
| SCL | FPGA → EEPROM | Reloj I2C (100 kHz; máx. 400 kHz) |
| SDA | Bidireccional | Dirección 0x50 + R/W, dirección interna, datos y ACK (open-drain con pull-up de 4,7 kΩ) |

### 3. Mapa de hardware y conteo de pines

**Total: 12 pines de la FPGA** (4 SPI-RAM + 6 Flash + 2 I2C). Solo 6 llevan cables hacia afuera; la Flash ya está soldada en la tarjeta.

| # | Señal (nombre en el .cst) | Pin FPGA | Va a | Pin del chip |
| --- | --- | --- | --- | --- |
| 1 | `ram_sck` | K2 | 23LC1024 SCK | 6 |
| 2 | `ram_mosi` | K1 | 23LC1024 SI | 5 |
| 3 | `ram_miso` | L1 | 23LC1024 SO | 2 |
| 4 | `ram_cs_n` | L2 | 23LC1024 CS | 1 |
| 5 | `i2c_scl` | K4 | 24LC256 SCL | 6 |
| 6 | `i2c_sda` | J4 | 24LC256 SDA | 5 |
| 7 | `flash_cs_n` | E6 | Flash de la tarjeta | — |
| 8 | `flash_sck` | E7 | Flash de la tarjeta | — |
| 9 | `flash_mosi` | D6 | Flash de la tarjeta | — |
| 10 | `flash_miso` | E5 | Flash de la tarjeta | — |
| 11 | `flash_wp_n` | D5 | Flash de la tarjeta | — |
| 12 | `flash_hold_n` | E4 | Flash de la tarjeta | — |

Alimentación de los chips externos: 3,3 V y GND de la tarjeta. 23LC1024: HOLD (7) y SIO2 (3) a 3,3 V. 24LC256: A0, A1, A2 y WP (1, 2, 3, 7) a GND, y una resistencia de 4,7 kΩ de SDA y de SCL a 3,3 V.

> Los pines de la FPGA son la asignación propuesta del proyecto; se confirman con el grupo 6 contra el esquemático del Dock antes de cablear. Los de la Flash, además, requieren usar sus pines de configuración (MSPI) como E/S de usuario.

### 4. Registros de control (MMIO)

**SPI** (mismo módulo para SPI-RAM en `0x41_0000` y Flash en `0x42_0000`):

| Desp. | Registro | Acceso | Contenido |
| --- | --- | --- | --- |
| 0x00 | CONTROL | L/E | bit0 START · bit1 RW (1 = escribir) · bits 15:8 comando · bit16 SIN_DIR (comando sin dirección) · bits 18:17 bytes a leer (1 o 3) |
| 0x04 | ADDR | L/E | Dirección de 24 bits |
| 0x08 | WDATA | L/E | Byte a escribir |
| 0x0C | RDATA | L | Dato leído (8 bits, o 24 para el ID de la Flash) |
| 0x10 | STATUS | L | bit0 BUSY · bit1 DONE (se limpia al leer) |
| 0x14 | CLK_DIV | L/E | SCK = 50 MHz / (2·(CLK_DIV+1)); reset 1 → 12,5 MHz |

**I2C** (`0x46_0000`):

| Desp. | Registro | Acceso | Contenido |
| --- | --- | --- | --- |
| 0x00 | CONTROL | E | bit0 START · bit1 STOP · bit2 WRITE · bit3 READ · bit4 ACK_OUT (0 = ACK, 1 = NACK al leer) |
| 0x04 | TX_DATA | L/E | Byte a enviar |
| 0x08 | RX_DATA | L | Byte recibido |
| 0x0C | STATUS | L | bit0 BUSY · bit1 ACK_IN (0 = la EEPROM respondió) · bit2 DONE |
| 0x10 | CLK_DIV | L/E | SCL = 50 MHz / (4·(CLK_DIV+1)); reset 124 → 100 kHz |

Uso: la CPU escribe los datos y el comando, sondea `STATUS.BUSY` hasta que valga 0 y lee el resultado. No se usan interrupciones.

### 5. Componentes a comprar: lista de precios

Precios consultados el 29 de octubre de 2026; los marcados como *aprox.* son referencias de mercado y conviene confirmarlos en la tienda.

| Componente | Cant. | Precio unitario (COP) | Subtotal (COP) | Dónde |
| --- | --- | --- | --- | --- |
| EEPROM 24LC256-I/P (DIP-8) | 1 | 11.900 (IVA incluido) | 11.900 | [Sigma Electrónica](https://www.sigmaelectronica.net/producto/24lc256/) |
| SRAM serial 23LC1024-I/P (DIP-8) | 1 | ≈ 11.000 (2,44 € + envío) | 11.000 | [Farnell](https://es.farnell.com/microchip/23lc1024-i-p/sram-serial-1mbit-2-5v-8pdip/dp/2212152) o Mercado Libre |
| Resistencia 4,7 kΩ 1/4 W | 2 | ≈ 100 (aprox.) | 200 | Cualquier tienda de electrónica |
| Condensador cerámico 100 nF | 2 | ≈ 200 (aprox.) | 400 | Cualquier tienda de electrónica |
| Zócalo DIP-8 | 2 | ≈ 600 (aprox.) | 1.200 | Cualquier tienda de electrónica |
| Cables dupont macho-hembra (paquete) | 1 | ≈ 6.000 (aprox.) | 6.000 | Cualquier tienda de electrónica |
| Protoboard pequeña (si no la tienen) | 1 | ≈ 8.000 (aprox.) | 8.000 | Cualquier tienda de electrónica |
| **Total aproximado** | | | **≈ 38.700** | |

La Flash no se compra: es la de 64 Mbit que trae la Tang Primer 25K. Se recomienda comprar un chip de repuesto de cada memoria.

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
| | |

## Pendientes y problemas

-
