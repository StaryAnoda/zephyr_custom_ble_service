#include  <zephyr/logging/log.h>

#include "audio_service.h"

LOG_MODULE_REGISTER(sensor_module);

K_MEM_SLAB_DEFINE_STATIC(mem_slab, BLOCK_SIZE, BLOCK_COUNT, 4);

void *mem_block;


void audio_service_init() {
	const struct device *i2s_dev = DEVICE_DT_GET(DT_NODELABEL(i2s0));
	struct i2s_config config;
	size_t size;
	int ret;


	config.word_size = SAMPLE_BIT_WIDTH;
	config.channels = CHANNELS;
	config.format = I2S_FMT_DATA_FORMAT_I2S;
	config.options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER;
	config.frame_clk_freq = SAMPLE_FREQUENCY;
	config.mem_slab = &mem_slab;
	config.block_size = BLOCK_SIZE;
	config.timeout = TIMEOUT_MS;

	ret = i2s_configure(i2s_dev, I2S_DIR_RX, &config);
	if (ret < 0) {
		printf("Failed to configure I2S RX: %d\n", ret);
		return 0;
	}

	if(!device_is_ready(i2s_dev)) {
		printf("I2S device not ready\n");
		return 0;
	}


}
void audio_sense_thread(void *arg1 , void *arg2, void *arg3) {
	LOG_WRN("Sense not implemented \n");
}
