#include "alsa_layer.h"

snd_seq_t* sequencer;
int port_out_id, queue_id;

#define CHECK(stmt) check_error(stmt, __LINE__)

void check_error(int code, int line) {
	if(code < 0) {
		fprintf(stderr, "Error %d detected on line %d in file %s\n", code, line, __FILE__);
		exit(1);
	}
}

double bpm = 80.0;

void alsa_layer_setup(const char *name) {
	CHECK(snd_seq_open(&sequencer, "default", SND_SEQ_OPEN_DUPLEX, 0));
	CHECK(snd_seq_set_client_name(sequencer, name));
	CHECK(port_out_id = snd_seq_create_simple_port(
			sequencer, 
			"improv", 
			SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
			SND_SEQ_PORT_TYPE_APPLICATION));

	queue_id = snd_seq_alloc_queue(sequencer);
	snd_seq_set_client_pool_output(sequencer, 200);

  snd_seq_queue_tempo_t *queue_tempo;
  snd_seq_queue_tempo_malloc(&queue_tempo);
  int tempo = (int)(6e7 / ((double)bpm * (double)TICKS_PER_QUARTER) * (double)TICKS_PER_QUARTER);
  snd_seq_queue_tempo_set_tempo(queue_tempo, tempo);
  snd_seq_queue_tempo_set_ppq(queue_tempo, TICKS_PER_QUARTER);
  snd_seq_set_queue_tempo(sequencer, queue_id, queue_tempo);
  snd_seq_queue_tempo_free(queue_tempo);

  CHECK(snd_seq_start_queue(sequencer, queue_id, NULL));

	snd_seq_connect_to(sequencer, 0, 128, 0);
	// snd_seq_connect_to(sequencer, 0, 130, 0); // zynaddsubfx
}

void alsa_layer_on_quit(int signum) {
	// puts("Quitting...\n");
	snd_seq_close(sequencer);
	exit(1);
}

int _chan = 0;

/// please make sure all notes are stopped before running thi
void alsa_layer_set_channel(int chan) {
	_chan = chan - 1;
}

/// velocity: 0-127
/// duration: 1.0 = crochet  0.5 = quaver  2.0 = minum
/// start_time: 0.0 = start  4.0 = 4 beats in   0.5 = half a beat in
/// if start_time is less than zero, plays instantly
/// if duration = 0: note on
/// if duration < 0: note off
void alsa_layer_schedule_note(int pitch, int velocity, float duration, float start_time) {
	snd_seq_event_t midi_event;
	snd_seq_ev_clear(&midi_event);
	snd_seq_ev_set_subs(&midi_event);
	snd_seq_ev_set_source(&midi_event, port_out_id);

	if(duration > 0) {
		snd_seq_ev_set_note(&midi_event, _chan, pitch, velocity, (int)(duration * (float)TICKS_PER_QUARTER));
	} else if(duration == 0) {
		snd_seq_ev_set_noteon(&midi_event, _chan, pitch, velocity);
	} else {
		snd_seq_ev_set_noteoff(&midi_event, _chan, pitch, velocity);
	}

	if(start_time < 0 && duration > 0) {
		snd_seq_queue_status_t *queue_status;
		snd_seq_queue_status_malloc(&queue_status);
		snd_seq_get_queue_status(sequencer, queue_id, queue_status);
		snd_seq_ev_schedule_tick(&midi_event, queue_id, 0, snd_seq_queue_status_get_tick_time(queue_status));
		snd_seq_queue_status_free(queue_status);
	} else if(start_time < 0) {
		snd_seq_ev_set_direct(&midi_event);
		snd_seq_event_output_direct(sequencer, &midi_event);
		return;
	} else {
		snd_seq_ev_schedule_tick(&midi_event, queue_id, 0, (int)(start_time * (float)TICKS_PER_QUARTER));
	}
	CHECK(snd_seq_event_output(sequencer, &midi_event));
}

void alsa_layer_play_scheduled() {
	CHECK(snd_seq_drain_output(sequencer));
}
