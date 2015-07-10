#ifndef WS2812_H__
#define WS2812_H__

#include <stdint.h>

void WS2812_Init(void);
void WS2812_Set(uint32_t offset, uint32_t size, char* data);

#endif

