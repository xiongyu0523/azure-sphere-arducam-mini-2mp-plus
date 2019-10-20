#ifndef __APP_DISPLAY_H
#define __APP_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "ArduCAM.h"

typedef enum {ArducamFormatJpeg, ArducamFormatBmp} arducamFormatT;



// Functions
void arducam_mini_2mp_init(void);

void arducam_mini_2mp_open(void);
void arducam_mini_2mp_setResolution(uint32_t res);
void arducam_mini_2mp_setFormat(arducamFormatT format);
void arducam_mini_2mp_startSingleCapture(void);
uint32_t arducam_mini_2mp_bytesAvailable(void);
uint32_t arducam_mini_2mp_fillBuffer(uint8_t *buffer, uint32_t bufferSize);

uint32_t arducam_mini_2mp_asyncFillBuffer(uint8_t *buffer, uint32_t bufferSize);
void     arducam_mini_2mp_onSpiInterrupt(uint32_t txBytes, uint32_t rxBytes);

bool display_init(void);


#endif
