#include <zephyr/logging/log.h>
#include <zephyr/smf.h>

#include "fsm_service.h"
#include "zephyr/kernel.h"
 
LOG_MODULE_REGISTER(fsm_module); 


/*App Gates*/
K_SEM_DEFINE(temp_sense_gate, 0, 1); 
K_SEM_DEFINE(mic_sense_gade, 0, 1); 

/*End Gates*/

static const struct smf_state ble_app_states[];

enum ble_app_state  { IDLE, CONNECTED, TEMP_READ, MIC_CAPTURE, ERROR}; 

struct state_object {
	/*Must be first for  casting*/
	struct smf_ctx state_context;

	/*Rest of the State Stuff*/
	struct k_event smf_event; 
	int32_t events;

} state_object; 

// idle
static void idle_entry(void *o)
{
        LOG_WRN("Entered Idle");
//	k_sem_reset(&audio_sense_gate); 
//	k_sem_reset(&temp_sense_gate);

}
static enum smf_state_result idle_run(void *o)
{
        smf_set_state(SMF_CTX(&state_object), &ble_app_states[IDLE]);
        return SMF_EVENT_HANDLED;
}
static void idle_exit(void *o)
{
        LOG_WRN("Left Idle");
}

//connected
static void connected_entry(void *o) 
{
	/*Once connected open the sense gates*/
}
static enum smf_state_result connected_run(void *o)
{
	LOG_WRN("Connected state is being run");
        smf_set_state(SMF_CTX(&state_object), &ble_app_states[CONNECTED]);
        return SMF_EVENT_HANDLED;
}
static void connected_exit(void *o)
{
        LOG_WRN("Left Connnected");
}

// temp  read 

static void temp_read_entry(void *o)
{
        LOG_WRN("Entered Temp_Read");
}
static enum smf_state_result temp_read_run(void *o)
{
        smf_set_state(SMF_CTX(&state_object), &ble_app_states[TEMP_READ]);
        return SMF_EVENT_HANDLED;
}

// mic capture
static void mic_capture_entry(void *o)
{
        LOG_WRN("Entered MIC_CAPTURE");
}
static enum smf_state_result mic_capture_run(void *o)
{
        smf_set_state(SMF_CTX(&state_object), &ble_app_states[MIC_CAPTURE]);
        return SMF_EVENT_HANDLED;
}

// error state

static void error_entry(void *o)
{
        LOG_WRN("Entered Error");
}
static enum smf_state_result error_run(void *o)
{
	// Do computation here to determine if you want to be here or not
	// or cause some side effect
        smf_set_state(SMF_CTX(&state_object), &ble_app_states[ERROR]);
        return SMF_EVENT_HANDLED;
}

// make a state table
static const struct smf_state ble_app_states[] = {
        [IDLE] = SMF_CREATE_STATE(idle_entry, idle_run, idle_exit, NULL, NULL),
	[CONNECTED] = SMF_CREATE_STATE(connected_entry, connected_run, connected_exit, NULL, NULL),
        [TEMP_READ] = SMF_CREATE_STATE(temp_read_entry, temp_read_run, NULL, NULL, NULL),
	[MIC_CAPTURE] = SMF_CREATE_STATE(mic_capture_entry, mic_capture_run, NULL, NULL, NULL),
	[ERROR] = SMF_CREATE_STATE(error_entry, error_run, NULL, NULL, NULL),
};
/* END STATE MACHINE DEFS*/

void fsm_run_thread(void *arg1, void *arg2, void *arg3) {

	/*Initialize events*/
	k_event_init(&state_object.smf_event);

	smf_set_initial(SMF_CTX(&state_object),  &ble_app_states[IDLE]);
	
	while(1) {
		state_object.events = k_event_wait(&state_object.smf_event, 
				EVENT_CENTRAL_CONNECTED, 
				false, K_SECONDS(10));

		int ret = smf_run_state(SMF_CTX(&state_object));

		if ( ret != 0) {
			LOG_ERR("State machine exploded (%d)", ret);
			break;
		}

	}

	
}


