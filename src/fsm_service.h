#ifndef FSM_SERVICE_H
#define FSM_SERVICE_H
#include <zephyr/sys/util.h>

/*List of App Events & Gates*/
#define EVENT_CENTRAL_CONNECTED    0x0001  // BIT(0)
#define EVENT_TEMP_READ            0x0002  // BIT(1)
#define EVENT_AUDIO_DETECTED       0x0004  // BIT(2)
#define EVENT_ERROR_OCCURED        0x0008  // BIT(3)
#define EVENT_CENTRAL_DISCONNECTED 0x0010  // BIT(4)
#define EVENT_AUD_SENSE_STARTED    0x0020  // BIT(5)
#define EVENT_AUD_SENSE_END        0x0040  // BIT(6)
#define EVENT_INFERENCE_START      0x0080  // BIT(7)
#define EVENT_INFERENCE_END        0x0100  // BIT(8)

extern struct k_sem temp_sense_gate;
extern struct k_sem mic_sense_gate;
extern struct k_sem inference_gate;

/*End List Events & gates*/
void fsm_init(void);
int fsm_post_event(uint32_t events);
void fsm_run_thread(void *arg1, void *arg2, void *arg3);
#endif
