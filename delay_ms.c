#include <errno.h>
#include <time.h>

#include "delay_ms.h"

void delay_ms(uint32_t period)
{
	struct timespec ts = {
		(time_t)(period / 1000),
		(long)((period % 1000) * 1000000)
	};

	while ((-1 == nanosleep(&ts, &ts)) && (EINTR == errno));
}