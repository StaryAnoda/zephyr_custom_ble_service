#include <zephyr/logging/log.h>

#include "audio_service.h"
#include "fsm_service.h"

LOG_MODULE_REGISTER(audio_module);

K_MEM_SLAB_DEFINE_STATIC(mem_slab, BLOCK_SIZE, BLOCK_COUNT, 4);
K_MEM_SLAB_DEFINE(inference_slab, BLOCK_SIZE, BLOCK_COUNT, 4);
K_FIFO_DEFINE(infer_fifo);


static const struct device *i2s_dev;

void audio_service_init()
{
	i2s_dev = DEVICE_DT_GET(DT_NODELABEL(i2s0));
	struct i2s_config config;
	int ret;

	config.word_size = SAMPLE_BIT_WIDTH;
	config.channels = NUMBER_OF_CHANNELS;
	config.format = I2S_FMT_DATA_FORMAT_I2S;
	config.options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER;
	config.frame_clk_freq = SAMPLE_FREQUENCY;
	config.mem_slab = &mem_slab;
	config.block_size = BLOCK_SIZE;
	config.timeout = TIMEOUT_MS;

	ret = i2s_configure(i2s_dev, I2S_DIR_RX, &config);
	if (ret < 0) {
		LOG_ERR("Failed to configure I2S RX: %d\n", ret);
		return;
	}

	if (!device_is_ready(i2s_dev)) {
		LOG_ERR("I2S device not ready\n");
		return;
	}
}

void audio_sense_thread(void *arg1, void *arg2, void *arg3)
{
	int ret;

	while (1) {
		/*Wait on a semaphore*/
		k_sem_take(&mic_sense_gate, K_FOREVER);

		/* Post an Event to FSM to show MIC capturing*/
		fsm_post_event(EVENT_AUD_SENSE_STARTED);



		/*Check the audio sense gate before working*/
		void *mem_block;
		void *infer_block;
		uint32_t size;

		ret = i2s_trigger(i2s_dev, I2S_DIR_RX, I2S_TRIGGER_START);
		if (ret < 0) {
			LOG_ERR("Failed to start I2S RX: %d \n", ret);
			return;
		}

		LOG_WRN("SENSING AUDIO");
		ret = i2s_read(i2s_dev, &mem_block, &size);
		if (ret == 0) {
			for (int b = 1; b <= 3; b++) {
				ret = k_mem_slab_alloc(&inference_slab, (void **)&infer_block,
						       K_NO_WAIT);
				if (ret == 0) {
					memset(infer_block, 0, size); 
					memcpy(infer_block, mem_block, size);
					k_fifo_put(&infer_fifo, infer_block);
					k_mem_slab_free(&mem_slab, (void *)mem_block);
				} else {
					LOG_ERR("Could not allocate Infer block (%d)", ret);
				}
			}

		} else {
			LOG_ERR("I2S Failed to read %d", ret);
		}

		// 3. Stop capture, then drop any packes
		i2s_trigger(i2s_dev, I2S_DIR_RX, I2S_TRIGGER_STOP);
		i2s_trigger(i2s_dev, I2S_DIR_RX, I2S_TRIGGER_DROP);

		// 4. Sleep before next cycle
		k_sem_give(&mic_sense_gate);
		//SEND A STATE EVENT TO TRIGGER FILTERING
		fsm_post_event(EVENT_AUD_SENSE_END);
		k_sleep(K_SECONDS(2));
	}
}
