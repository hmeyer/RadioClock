#include <string.h>
#include "scroller.h"
#include "display.h"

#define MXRES 64

void scrollString(volatile uint8_t *buffer, const char *string, int16_t counter) {
	size_t l = strlen(string)*6; 
	int16_t scrollrange = l + MXRES;
	if (scrollrange<1) {
		drawString(buffer, 0, string);
		return;
	}
	int16_t scrollpos = counter % scrollrange;
	scrollpos = MXRES - scrollpos;
	drawString(buffer, scrollpos, string);
}
