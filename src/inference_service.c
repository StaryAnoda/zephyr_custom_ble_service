#include <zephyr/logging/log.h> 

#include "inference_service.h"
#include "fsm_service.h"
#include "audio_service.h"

LOG_MODULE_REGISTER(inference_module);


void inference_init() {
	LOG_WRN("Inference initialized"); 
}

void inference_thread_run(void *arg1, void *arg2,  void *arg3) {
	while(1) {
		//TODO prevent race with audio service using FSM
		k_sem_take(&inference_gate, K_FOREVER);
		LOG_WRN("Inference has been run");


		for (int b = 0; b < 3; b++) {
			void *rx_block = k_fifo_get(&infer_fifo, K_NO_WAIT);
			//1.normalize samples 
			//2.fiter samples 
			//3.do a power measurement 
			// 4. Free each block
			k_mem_slab_free(&inference_slab, (void *)rx_block);

		}
		
		// 5 Use hysteresis  to check for clap


		/*	        
		int32_t *samples = mem_block;
				//Normalize sample
				
				for(int f = 0; f < SAMPLES_PER_BLOCK; f++)
				{
					// convert to float32
					int32_t  sam = samples[f] >> 8; 
					samples_f[f] = (float32_t)sam / 8388608.0f;
				}

				// End Normalization

				// Apply Filter
				
			
				arm_fir_f32(&FIR, samples_f, filtered_samples, SAMPLES_PER_BLOCK);


				for(int f = 0; f < 100; f++)
				{
					LOG_ERR("Filtered Sample %d: %f", f, 
							(double)filtered_samples[f]);
				}

				End Filter apply*/
		k_sem_give(&inference_gate);
		fsm_post_event(EVENT_INFERENCE_END);
		k_sleep(K_SECONDS(4)); 
	}
}
