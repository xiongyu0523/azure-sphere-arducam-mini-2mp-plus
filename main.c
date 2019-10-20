#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <applibs/log.h>

#include "ArduCAM.h"
#include "delay_ms.h"

int main(int argc, char* argv[])
{
	Log_Debug("Exmaple to capture a JPEG image from ArduCAM mini 2MP Plus and send to Azure Blob\r\n");
	
	// init hardware and probe camera
	arducam_ll_init();
	arducam_reset();
	if (arducam_test() == 0) {
		Log_Debug("ArduCAM mini 2MP Plus is found\r\n");
	} else {
		Log_Debug("ArduCAM mini 2MP Plus is not found\r\n");
		return -1;
	}

	// config Camera
	arducam_set_format(JPEG);
	arducam_InitCAM();

	for (uint32_t i = 0; i <= 8; i++) {

		// 0 = OV2640_160x120, 8 = OV2640_1600x1200
		arducam_OV2640_set_JPEG_size(i);
		delay_ms(1000);
		arducam_clear_fifo_flag();
		arducam_flush_fifo();

		// Trigger a capture and wait for data ready in DRAM
		arducam_start_capture();
		while (!arducam_check_fifo_done());

		uint32_t img_len = arducam_read_fifo_length();
		if (img_len > MAX_FIFO_SIZE) {
			Log_Debug("ERROR: FIFO overflow\r\n");
			return -1;
		}

		Log_Debug("Captured imgSize = %d\r\n", img_len);

		uint8_t* p_jpg_buffer = malloc(img_len);

		arducam_CS_LOW();
		arducam_set_fifo_burst();
		arducam_read_fifo_burst(p_jpg_buffer, img_len);
		arducam_CS_HIGH();

		arducam_clear_fifo_flag();

#if 0
		for (uint32_t i = 0; i < img_len; i++) {
			Log_Debug("0x%02X ", p_jpg_buffer[i]);
		}
		Log_Debug("\r\n\r\n\r\n\r\n");
#endif

		free(p_jpg_buffer);
	}
}