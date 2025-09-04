/*
 * Copyright (c) gaiaochos.com
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/smf.h>

#include "app.h"
#include "audio_service.h"
#include "ble_service.h"
#include "temperature.h"

// Enable logs
LOG_MODULE_REGISTER(custom_service_log);

#define THREAD_STACK_SIZE 1024

K_MSGQ_DEFINE(tempmsgq, TEMP_MSG_SIZE, TEMPQSIZE, 2);
K_THREAD_DEFINE(temp_thread_id, THREAD_STACK_SIZE, temp_sensor_thread, NULL, NULL, NULL, 5, 0, 0); 
K_THREAD_DEFINE(ble_temp_thread_id, THREAD_STACK_SIZE, ble_temp_read_thread, NULL, NULL, NULL, 6, 0,0);
K_THREAD_DEFINE(audio_thread_id, THREAD_STACK_SIZE, audio_sense_thread, NULL, NULL, NULL, 4, 0, 0);

/* STATE MACHINE WORK*/
static const struct smf_state ble_app_states[];

enum ble_app_state  { IDLE, CONNECTED, TEMP_READ, MIC_CAPTURE, ERROR}; 

struct state_object {
	struct smf_ctx state_context; 
} state_object; 

// idle
static void idle_entry(void *o)
{
        LOG_WRN("Entered Idle");
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
static enum smf_state_result connected_run(void *o)
{
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
	[CONNECTED] = SMF_CREATE_STATE(NULL, connected_run, connected_exit, NULL, NULL),
        [TEMP_READ] = SMF_CREATE_STATE(temp_read_entry, temp_read_run, NULL, NULL, NULL),
	[MIC_CAPTURE] = SMF_CREATE_STATE(mic_capture_entry, mic_capture_run, NULL, NULL, NULL),
	[ERROR] = SMF_CREATE_STATE(error_entry, error_run, NULL, NULL, NULL),
};
/* END STATE MACHINE DEFS*/
int main(void) {

   smf_set_initial(SMF_CTX(&state_object),  &ble_app_states[IDLE]);
  /*Blocking bluetooth init*/
  ble_service_init();

  /*Bluetooth setup was a success do sensor now*/
  sensor_temp_sensor_init();

  /*Audop Service init*/
  audio_service_init();


 
  /*This becomes our idle loop*/
  while (1) {
    k_sleep(K_SECONDS(2));
  }
}
