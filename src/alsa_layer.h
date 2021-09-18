#ifndef __ALSA_LAYER_H
#define __ALSA_LAYER_H
#include <alsa/asoundlib.h>

extern snd_seq_t* sequencer;
extern int port_out_id, queue_id;

#define TICKS_PER_QUARTER 128
extern double bpm;

void setup();
void on_quit(int signum);
void schedule_note(int pitch, int velocity, float duration, float start_time);
// void check_error(int code, int line);
void play_scheduled();
void set_channel(int chan);


#endif
