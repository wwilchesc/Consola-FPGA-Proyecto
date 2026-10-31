#ifndef G5_PLATFORM_H
#define G5_PLATFORM_H
#include "console.h"
/* El integrador proporciona callbacks que implementan el contrato MMIO real.
   Sin direcciones, tamaños de registro ni accesos volátiles supuestos aquí.
   No retener el framebuffer: pertenece al llamador. Sin malloc. */
typedef struct {
    void *context;
    uint16_t (*read_keys)(void *context);
    void (*present)(void *context, const uint8_t *pixels);
    void (*play_sound)(void *context, Sound event);
} ConsolePlatform;
/* Llamar una vez por paso del timer acordado (propuesta 150 ms).
   Los callbacks son opcionales; los efectos se consumen en este paso. */
void console_frame(Console *c, const ConsolePlatform *platform,
                   uint8_t pixels[SCREEN_W*SCREEN_H]);
#endif
