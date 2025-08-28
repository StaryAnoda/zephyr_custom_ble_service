#ifndef AUDIO_SERVICE_H
#define AUDIO_SERVIC_H
#include <zephyr/device.h>

#include <zephyr/drivers/i2s.h>

#define SAMPLE_FREQUENCY 44100
#define SAMPLE_BIT_WIDTH 24
#define CHANNELS 1
#define BLOCK_SIZE 256
#define BLOCK_COUNT 2
#define TIMEOUT_MS 2000

void audio_service_init();
void audio_sense_thread(void *arg1, void *arg2, void *arg3);
#endif
