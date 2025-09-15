#include <zephyr/logging/log.h> 

#include "inference_service.h"

LOG_MODULE_REGISTER(inference_module);


void inference_init() {
	LOG_WRN("Inference initialized"); 
}

void inference_thread_run(void *arg1, void *arg2,  void *arg3) {
	while(1) {
		LOG_WRN("Inference has been run");
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


		k_sleep(K_SECONDS(3)); 
	}
}
