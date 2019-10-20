#if defined(AzureSphere_CA7)

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "applibs_versions.h"

#include <applibs/log.h>
#include <applibs/i2c.h>
#include <applibs/spi.h>
#include <applibs/gpio.h>
#include <hw/mt3620_rdb.h>

#elif defined(AzureSphere_CM4)
#endif

#include "ll.h"

#if defined(AzureSphere_CA7)
static int GpioFd;
static int i2cFd;
static int spiFd;
#endif

#define OV2640_I2C_ADDR			0x30
#define MAX_SPI_TRANSFER_BYTES	4096

int ll_gpio_init(void)
{
	GpioFd = GPIO_OpenAsOutput(MT3620_RDB_HEADER4_PIN14_GPIO, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (GpioFd < 0) {
		Log_Debug("ERROR: GPIO_OpenAsOutput: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	return 0;
}

void ll_gpio_cs_go_low(void)
{
	GPIO_SetValue(GpioFd, GPIO_Value_Low);
}

void ll_gpio_cs_go_high(void)
{
	GPIO_SetValue(GpioFd, GPIO_Value_High);
}

int ll_i2c_init(void)
{
#if defined(AzureSphere_CA7)

    i2cFd = I2CMaster_Open(MT3620_RDB_HEADER2_ISU0_I2C);
	if (i2cFd < 0) {
		Log_Debug("ERROR: I2CMaster_Open: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	int ret = I2CMaster_SetBusSpeed(i2cFd, I2C_BUS_SPEED_STANDARD);
	if (ret < 0) {
		Log_Debug("ERROR: I2CMaster_SetBusSpeed: errno=%d (%s)\n", errno, strerror(errno));
		close(i2cFd);
		return -1;
	}

	ret = I2CMaster_SetTimeout(i2cFd, 100);
	if (ret < 0) {
		Log_Debug("ERROR: I2CMaster_SetTimeout: errno=%d (%s)\n", errno, strerror(errno));
		close(i2cFd);
		return -1;
	}

	return 0;

#elif defined(AzureSphere_CM4)
#endif
}

int ll_i2c_tx(uint8_t* tx_data, uint32_t tx_len)
{
#if defined(AzureSphere_CA7)

	int ret = I2CMaster_Write(i2cFd, OV2640_I2C_ADDR, tx_data, tx_len);
	if (ret < 0) {
		Log_Debug("ERROR: I2CMaster_Write: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}
	else if (ret != tx_len) {
		Log_Debug("ERROR: I2CMaster_Write transfer %d bytes, expect %d bytes\n", ret, tx_len);
		return -1;
	}

	return 0;

#elif defined(AzureSphere_CM4)

#endif
}

int ll_i2c_tx_then_rx(uint8_t* tx_data, uint32_t tx_len, uint8_t* rx_data, uint32_t rx_len)
{
#if defined(AzureSphere_CA7)

	int ret = I2CMaster_WriteThenRead(i2cFd, OV2640_I2C_ADDR, tx_data, tx_len, rx_data, rx_len);
	if (ret < 0) {
		Log_Debug("ERROR: I2CMaster_WriteThenRead: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	} else if (ret != (tx_len + rx_len)) {
		Log_Debug("ERROR: I2CMaster_WriteThenRead transfer %d bytes, expect %d bytes\n", ret, tx_len + rx_len);
		return -1;
	}

	return 0;

#elif defined(AzureSphere_CM4)

#endif
}

int ll_spi_init(void)
{
#if defined(AzureSphere_CA7)

    SPIMaster_Config config;
    int ret = SPIMaster_InitConfig(&config);
    if (ret < 0) {
        Log_Debug("ERROR: SPIMaster_InitConfig: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    config.csPolarity = SPI_ChipSelectPolarity_ActiveLow;
    spiFd = SPIMaster_Open(MT3620_RDB_HEADER4_ISU1_SPI, MT3620_SPI_CS_A, &config);
    if (spiFd < 0) {
        Log_Debug("ERROR: SPIMaster_Open: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    int result = SPIMaster_SetBusSpeed(spiFd, 4000000);
    if (result < 0) {
        Log_Debug("ERROR: SPIMaster_SetBusSpeed: errno=%d (%s)\n", errno, strerror(errno));
        close(spiFd);
        return -1;
    }

    result = SPIMaster_SetMode(spiFd, SPI_Mode_0);
    if (result < 0) {
        Log_Debug("ERROR: SPIMaster_SetMode: errno=%d (%s)\n", errno, strerror(errno));
        close(spiFd);
        return -1;
    }
	
	return 0;

#elif defined(AzureSphere_CM4)

#endif
}

int ll_spi_tx(uint8_t *tx_data, uint32_t tx_len)
{
#if defined(AzureSphere_CA7)

	if (tx_len > MAX_SPI_TRANSFER_BYTES) {
		Log_Debug("ll_spi_tx does not support split transfer when data len > 4096\r\n");
		return -1;
	}

	SPIMaster_Transfer transfers;

	int ret = SPIMaster_InitTransfers(&transfers, 1);
	if (ret < 0) {
		Log_Debug("ERROR: SPIMaster_InitTransfers: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	transfers.flags     = SPI_TransferFlags_Write;
	transfers.writeData = tx_data;
	transfers.length    = tx_len;

	ret = SPIMaster_TransferSequential(spiFd, &transfers, 1);
	if (ret < 0) {
		Log_Debug("ERROR: SPIMaster_TransferSequential: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	} else if (ret != tx_len) {
		Log_Debug("ERROR: SPIMaster_TransferSequential transfer %d bytes, expect %d bytes\n", ret, tx_len);
		return -1;
	}

	return 0;

#elif defined(AzureSphere_CM4)

#endif
}

int ll_spi_rx(uint8_t *rx_data, uint32_t rx_len)
{
#if defined(AzureSphere_CA7)

	size_t numOfXfer = (rx_len % MAX_SPI_TRANSFER_BYTES == 0) ? (rx_len / MAX_SPI_TRANSFER_BYTES) : (rx_len / MAX_SPI_TRANSFER_BYTES + 1);
	
	SPIMaster_Transfer transfer;

	uint32_t offset = 0;
	int32_t sizeleft = (int32_t)rx_len;
	
	while (numOfXfer > 0) {
		int ret = SPIMaster_InitTransfers(&transfer, 1);
		if (ret < 0) {
			Log_Debug("ERROR: SPIMaster_InitTransfers: errno=%d (%s)\n", errno, strerror(errno));
			return -1;
		}

		transfer.flags = SPI_TransferFlags_Read;
		transfer.readData = rx_data + offset;
		transfer.length = (sizeleft > MAX_SPI_TRANSFER_BYTES) ? MAX_SPI_TRANSFER_BYTES : sizeleft;

		ret = SPIMaster_TransferSequential(spiFd, &transfer, 1);
		if (ret < 0) {
			Log_Debug("ERROR: SPIMaster_TransferSequential: errno=%d (%s)\n", errno, strerror(errno));
			return -1;
		} else if (ret != transfer.length) {
			Log_Debug("ERROR: SPIMaster_TransferSequential transfer %d bytes, expect %d bytes\n", ret, rx_len);
			return -1;
		}

		sizeleft -= MAX_SPI_TRANSFER_BYTES;
		offset   += MAX_SPI_TRANSFER_BYTES;
		numOfXfer--;
	};
	
    return 0;

#elif defined(AzureSphere_CM4)

#endif
}

int ll_spi_tx_then_rx(uint8_t *tx_data, uint32_t tx_len, uint8_t *rx_data, uint32_t rx_len)
{
#if defined(AzureSphere_CA7)
#if 0
	if ((tx_len > MAX_SPI_TRANSFER_BYTES) || (rx_len > MAX_SPI_TRANSFER_BYTES)) {
		Log_Debug("ll_spi_tx_then_rx does not support split transfer when data len > 4096\r\n");
		return -1;
	}
#endif
    int ret;
    ret = SPIMaster_WriteThenRead(spiFd, (const uint8_t *)tx_data, tx_len, rx_data, rx_len);
    if (ret < 0) {
        Log_Debug("ERROR: SPIMaster_WriteThenRead: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
	} else if (ret != (tx_len + rx_len)) {
		Log_Debug("ERROR: SPIMaster_TransferSequential transfer %d bytes, expect %d bytes\n", ret, tx_len);
		return -1;
	}
    return 0;

#elif defined(AzureSphere_CM4)

#endif
}
