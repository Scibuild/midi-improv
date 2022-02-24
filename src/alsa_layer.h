#ifndef __ALSA_LAYER_H
#define __ALSA_LAYER_H
#include <alsa/asoundlib.h>

extern snd_seq_t* sequencer;
extern int port_out_id, queue_id;

#define TICKS_PER_QUARTER 128
extern double bpm;

void alsa_layer_setup(const char *name);
void alsa_layer_on_quit(int signum);


/// velocity: 0-127
/// duration: 1.0 = crochet  0.5 = quaver  2.0 = minum
/// start_time: 0.0 = start  4.0 = 4 beats in   0.5 = half a beat in
/// if start_time is less than zero, plays instantly
/// if duration = 0: note on
/// if duration < 0: note off
void alsa_layer_schedule_note(int pitch, int velocity, float duration, float start_time);

// void check_error(int code, int line);
void alsa_layer_play_scheduled();
void alsa_layer_set_channel(int chan);


#endif
