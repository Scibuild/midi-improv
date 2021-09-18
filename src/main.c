#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// void check_error(int code, int line) {
// 	if(code < 0) {
// 		fprintf(stderr, "Error %d detected on line %d\n", code, line);
// 		exit(1);
// 	}
// }

// really need to fix this up lmao
// 
// exposed functions:
// void setup()
// void on_quit(int signum)
// void schedule_note(int pitch, int velocity, float duration, float start_time)
#include "alsa_layer.h"

#include "permutations.h"

void print_array(int *array, int array_length) {
	for(int i = 0; i < array_length; i++){
		printf("%d ", array[i]);
	}
	putchar('\n');
}


void schedule_note_relative(int pitch, int velocity, float duration, float start_time) {
	static float offset_time = 0.0;
	offset_time += start_time;
	schedule_note(pitch, velocity, duration, offset_time);
}

int note_letter_to_number(char c) {
	c &= ~(1<<5); // capitalise
	assert(c >= 'A' && c <= 'G');

	// c=0 c#=1 d=2 ...
	int number = (int)((c <= 'B') ? 
			(c - 'A') * 2 + 9: 
			(c - 'C') * 2);
	if(c >= 'F')
		number--;

	return number;
}

// string to note
int s2n(char* str) {
	char note_letter = str[0];
	int note_number_in_octave = note_letter_to_number(note_letter);

	int octave_position = 1;
	if(str[1] == '#'){
		note_number_in_octave++;
		octave_position = 2;
	}
	if(str[1] == 'b'){
		note_number_in_octave--;
		octave_position = 2;
	}

	int octave_number = str[octave_position] - '0' + 1;


	return octave_number * 12 + note_number_in_octave;
}


enum chort_type {
	MAJOR, MINOR, DOM_SEVEN, MAJ_SEVEN, MIN_SEVEN, 
	SUS4, SUS2, SUS2_4, SEVEN_SUS_4, DIM_SEVEN, HALF_DIM_SEVEN,
	DIMINISHED
};

enum interval {
	UNISON = 0,
	MINOR_SECOND = 1,
	MAJOR_SECOND = 2,
	MINOR_THIRD = 3,
	MAJOR_THIRD = 4,
	PERFECT_FOURTH = 5,
	TRITONE = 6,
	PERFECT_FIFTH = 7,
	MINOR_SIXTH = 8,
	MAJOR_SIXTH = 9,
	MINOR_SEVENTH = 10,
	MAJOR_SEVENTH = 11,
	OCTAVE = 12
};

enum note_in_octave {
	NOTE_A = 9,
	NOTE_A_SHARP = 10,
	NOTE_B_FLAT = 10,
	NOTE_B = 11,
	NOTE_C = 0,
	NOTE_C_SHARP = 1,
	NOTE_D_FLAT = 1,
	NOTE_D = 2,
	NOTE_D_SHARP = 3,
	NOTE_E_FLAT = 3,
	NOTE_E = 4,
	NOTE_F = 5,
	NOTE_F_SHARP = 6,
	NOTE_G_FLAT = 6,
	NOTE_G = 7,
	NOTE_G_SHARP = 8,
	NOTE_A_FLAT = 8
};

int midi_to_in_octave(int midi) {
 	return midi % 12;
}

void schedule_triad(int root, int second, int third, int inversion, int velocity, float duration, float start_time) {
	schedule_note(root, velocity, duration, start_time);
	schedule_note(root+second - (inversion == 1 ? OCTAVE : 0), velocity, duration, start_time);
	schedule_note(root+third - (inversion ? OCTAVE : 0), velocity, duration, start_time);
}

void schedule_tetrachord(int root, int second, int third, int fourth, int inversion, int velocity, float duration, float start_time) {
	schedule_note(root, velocity, duration, start_time);
	schedule_note(root+second - (inversion == 1 ? OCTAVE : 0), velocity, duration, start_time);
	schedule_note(root+third - (inversion >= 1 && inversion <= 2 ? OCTAVE : 0), velocity, duration, start_time);
	schedule_note(root+fourth - (inversion >= 1 && inversion <= 3 ? OCTAVE : 0), velocity, duration, start_time);
}

void schedule_chord(int root, enum chort_type chord, int inversion, int velocity, float duration, float start_time) {
	switch (chord) {
		case MAJOR:
			schedule_triad(root, MAJOR_THIRD, PERFECT_FIFTH, inversion, velocity, duration, start_time);
			break;
		case MINOR: 
			schedule_triad(root, MINOR_THIRD, PERFECT_FIFTH, inversion, velocity, duration, start_time);
			break; 
		case DOM_SEVEN: 
			schedule_tetrachord(root, MAJOR_THIRD, PERFECT_FIFTH, MINOR_SEVENTH, inversion, velocity, duration, start_time);
			break; 
		case MAJ_SEVEN: 
			schedule_tetrachord(root, MAJOR_THIRD, PERFECT_FIFTH, MAJOR_SEVENTH, inversion, velocity, duration, start_time);
			break; 
		case MIN_SEVEN: 
			schedule_tetrachord(root, MINOR_THIRD, PERFECT_FIFTH, MINOR_SEVENTH, inversion, velocity, duration, start_time);
			break;
		case SUS4: 
			schedule_triad(root, PERFECT_FOURTH, PERFECT_FIFTH, inversion, velocity, duration, start_time);
			break; 
		case SUS2: break; 
			schedule_triad(root, MAJOR_SECOND, PERFECT_FIFTH, inversion, velocity, duration, start_time);
			break; 
		case SUS2_4: break; 
			schedule_tetrachord(root, MAJOR_SECOND, PERFECT_FOURTH, PERFECT_FIFTH, inversion, velocity, duration, start_time);
		case SEVEN_SUS_4: break; 
		case DIM_SEVEN: 
			schedule_tetrachord(root, MINOR_THIRD, TRITONE, MAJOR_SIXTH, inversion, velocity, duration, start_time);
			break; 
		case HALF_DIM_SEVEN: 
			schedule_tetrachord(root, MINOR_THIRD, TRITONE, MINOR_SEVENTH, inversion, velocity, duration, start_time);
			break;
		case DIMINISHED:
			schedule_triad(root, MINOR_THIRD, TRITONE, inversion, velocity, duration, start_time);
			break;

	}
}

void schedule_chord_relative(int root, enum chort_type chord, int inversion, int velocity, float duration, float start_time) {
	static float offset_time = 0.0;
	offset_time += start_time;
	schedule_chord(root, chord, inversion, velocity, duration, offset_time);
}

void test_s2n() {
	printf("%s\t\t%d\n", "C4", s2n("C4")); // 60
	printf("%s\t\t%d\n", "Ab2", s2n("Ab2")); // 44
	printf("%s\t\t%d\n", "E5", s2n("E5")); //76
	printf("%s\t\t%d\n", "e#4", s2n("e#4")); //65
}

int closest_note(int starting, int target) {
	//  60 (C4)  going to an E (4), correct answer is 64
	//  60 % 12 = 0
	//  4 - 0 = 4
	//  12 * (60/12) + 4 = 64
	//
	//  60 (C4)  going to an A (9), correct answer is 57
	//  60 % 12 = 0
	//  9 - 0 = 9
	//  because 9 > 6  (? how to choose which tritone to go to, 
	//  hopefully with multiple voices, no single voice jumps a tritone)

	int octave_distance = target - starting % 12;
	if (octave_distance > 6) octave_distance -= 12;
	if (octave_distance < -6) octave_distance += 12;
	return starting + octave_distance;
}

int note_distance(int starting, int target) {
	int octave_distance = target - starting % 12;
	if (octave_distance > 6) octave_distance -= 12;
	if (octave_distance < -6) octave_distance += 12;
	return abs(octave_distance);
}

struct voice_lead_callback_params {
	int length;             // number of notes aka length of one side of cost matrix
	int* cost_matrix;       // precalculated cost matrix to determine cost of permutation
	int minimum;            // the value of the current smallest cost
	int* current_minimum;   // where to write the new minimum if found
};

static void voice_lead_callback(int* permutation, void * params) {
	struct voice_lead_callback_params * vlcp = params;
	int total_cost = 0;
	for(int i = 0; i < vlcp->length; i++) {
		total_cost += vlcp->cost_matrix[i * vlcp->length + permutation[i]];
	}
//printf("%d   \t%d   \t\t%d %d %d\n", total_cost, vlcp->minimum, permutation[0], permutation[1], permutation[2]);
	if(total_cost < vlcp->minimum) {
		memcpy(vlcp->current_minimum, permutation, vlcp->length * sizeof(int));
		vlcp->minimum = total_cost;
//		print_array(vlcp->current_minimum, vlcp->length);
	}
}

void voice_lead(int* starting_notes, int* target_notes, int* output_notes, int length) {
	if (length == 1) {
		*output_notes = closest_note(*starting_notes, *target_notes);
		return;
	}

	if(length >= 8) {
		fprintf(stderr, "Voice leading algorithm does not work well for many notes, still trying to find more optimised solution");
		return;
	}

	// we need a 2d array now to store the distances to the closest notes for each
	
	int* cost_matrix = malloc(length * length * sizeof(int));

	for(int i = 0; i < length * length; i++) {
		// outer product style
		// cost_matrix[m * length + n] = starting_notes[m], target_notes[n]
		cost_matrix[i] = note_distance(starting_notes[i/length], target_notes[i%length]);
	}

	// this definitely feels like an O(n!) algorithm oops
	// yeaaa bc then this becomes a problem of checking all the orderings,	
	
	int current_minimum[8];
	// print_array(&current_minimum[0], length);
	struct voice_lead_callback_params vlcp;
	vlcp.length = length;
	vlcp.cost_matrix = cost_matrix;
	vlcp.minimum = 0x7FFFFFFF;
	vlcp.current_minimum = &current_minimum[0];
	permute(length, voice_lead_callback, (void *)&vlcp);
	//	print_array(&current_minimum[0], length);

	for(int i = 0; i < length; i++) {
		output_notes[i] = closest_note(starting_notes[i], target_notes[current_minimum[i]]);
	//		printf("%d\t\t%d\t\t%d\n", starting_notes[i], target_notes[current_minimum[i]], output_notes[i]);
	}

	free(cost_matrix);
	
}

void chord_name_to_triad(const char* name, int* triad_out) {
	int index = 1;
	char note_letter = name[0];
	int root_number = note_letter_to_number(note_letter);
	if(name[index] == 'b') {
		root_number--;
		if(root_number == -1) root_number = 11;
		index++;
	} else if(name[index] == '#') {
		root_number++;
		if(root_number == 12) root_number = 0;
		index++;
	}
	triad_out[0] = root_number;

	int is_major = 1;
	int is_seven = 0;
	int is_sixth = 0;
	int is_diminished = 0;
	int is_augmented = 0;
	int is_suspended = 0;

	if(name[index] == 'm') {
		index++;
		if(name[index] == 'a' && name[index+1] == 'j') {
			index += 2;
			is_major = 2; // special case if seven chord
		} else {
			is_major = 0;
		}
	} else if(name[index] == 'd' &&name[index+1] == 'i' && name[index+2] == 'm') {
		is_diminished = 1;
		is_major = 0;
		index+=3;
	} else if(name[index] == 'a' &&name[index+1] == 'u' && name[index+2] == 'g') {
		is_augmented = 1;
		index+=3;
	} else if(name[index] == 's' &&name[index+1] == 'u' && name[index+2] == 's') {
		is_suspended = 4;
		index+=3;
		if(name[index] == '4') index++;
		else if(name[index] == '2') {
			index++;
			is_suspended = 2;
		}
	} else if(name[index] == 'h' &&name[index+1] == 'd' && name[index+2] == 'i'&& name[index+3] == 'm') {
		is_diminished = 2;
		is_major = 0;
		index+=4;
	}

	if(name[index] == '7') {
		is_seven = 1;
	} else if (name[index] == '6') {
		is_sixth = 1;
	} 

	if (name[index] == 'b' && name[index+1] == '5') {
		is_diminished = 1;
	}

	if(is_suspended) {
		if(is_suspended == 2)
			triad_out[1] = triad_out[0] + MAJOR_SECOND;
		else
			triad_out[1] = triad_out[0] + PERFECT_FOURTH;
	} else if(is_major) {
		triad_out[1] = triad_out[0] + MAJOR_THIRD;
	} else {
		triad_out[1] = triad_out[0] + MINOR_THIRD;
	}
	
	if(is_seven) {
		if(is_major < 2) {
			triad_out[2] = triad_out[0] + MINOR_SEVENTH;
		} else {
			triad_out[2] = triad_out[0] + MAJOR_SEVENTH;
		}

		if(is_diminished) {
			triad_out[1] = triad_out[0] + TRITONE;
		}
		if(is_diminished == 1) {
			triad_out[2] = triad_out[0] + MAJOR_SIXTH;
		}
	} else if(is_sixth) {
			triad_out[2] = triad_out[0] + MAJOR_SIXTH;
	} else {
		triad_out[2] = triad_out[0] + PERFECT_FIFTH;
		if(is_diminished) {
			triad_out[2] = triad_out[0] + TRITONE;
		} else if(is_augmented) {
			triad_out[2] = triad_out[0] + MINOR_SIXTH;
		}
	}
	triad_out[1] %= 12;
	triad_out[2] %= 12;
}

#define CHORD_SIZE 3
int playing_chord[CHORD_SIZE] = {60, 64, 67};

void clear_playing_chord() {
	schedule_note(playing_chord[0], 80, -1.0, -1.0);
	schedule_note(playing_chord[1], 80, -1.0, -1.0);
	schedule_note(playing_chord[2], 80, -1.0, -1.0);
}

void my_on_quit(int signum) {
	clear_playing_chord();
	play_scheduled();
	usleep(100000);
	on_quit(signum);
}

int main() {
	signal(SIGTERM, my_on_quit);
	signal(SIGINT, my_on_quit);

	setup();


	int target_chord[CHORD_SIZE]; // = {NOTE_G, NOTE_B, NOTE_D};
	int output[CHORD_SIZE];

	schedule_note(playing_chord[0], 80, 1, -1.0);
	schedule_note(playing_chord[1], 80, 1, -1.0);
	schedule_note(playing_chord[2], 80, 1, -1.0);
	play_scheduled();

	char buffer[20];
	while(1) {
		fgets(buffer, 20, stdin);
		if(buffer[0] == '\n') continue;

		/*
		note_str[0] = c;
		target_chord[0] = s2n(note_str);
		if(c=='c' || c == 'f' || c == 'g')
			target_chord[1] = target_chord[0] + MAJOR_THIRD;
		else
			target_chord[1] = target_chord[0] + MINOR_THIRD;

		if(c == 'b') 
			target_chord[2] = target_chord[0] + TRITONE;
		else 
			target_chord[2] = target_chord[0] + PERFECT_FIFTH;

		target_chord[0] %= 12;
		target_chord[1] %= 12;
		target_chord[2] %= 12;
		*/

		if(!strcmp(buffer, "stop\n")) {
			clear_playing_chord();
			play_scheduled();
		} else if(!strcmp(buffer, "reset\n")) {
			playing_chord[0] = s2n("C4");
			playing_chord[1] = s2n("E4");
			playing_chord[2] = s2n("G4");
		} else if(!strcmp(buffer, "iup\n")) {
			clear_playing_chord();
			if(playing_chord[0] < playing_chord[1] && playing_chord[0] < playing_chord[2])
				playing_chord[0] += 12;
			else if(playing_chord[1] < playing_chord[0] && playing_chord[1] < playing_chord[2])
				playing_chord[1] += 12;
			else
				playing_chord[2] += 12;
		} else if(!strcmp(buffer, "idown\n")) {
			clear_playing_chord();
			if(playing_chord[0] > playing_chord[1] && playing_chord[0] > playing_chord[2])
				playing_chord[0] -= 12;
			else if(playing_chord[1] > playing_chord[0] && playing_chord[1] > playing_chord[2])
				playing_chord[1] -= 12;
			else
				playing_chord[2] -= 12;
		} else if(!strcmp(buffer, "up\n")) {
			clear_playing_chord();
			playing_chord[0] += 12;
			playing_chord[1] += 12;
			playing_chord[2] += 12;
		} else if(!strcmp(buffer, "down\n")) {
			clear_playing_chord();
			playing_chord[0] -= 12;
			playing_chord[1] -= 12;
			playing_chord[2] -= 12;
	  } else if(!strncmp(buffer, "chan ", 5)){
			int new_chan = atoi(&buffer[5]);
			clear_playing_chord();
			set_channel(new_chan);
		} else {
			chord_name_to_triad(buffer, target_chord);

			clear_playing_chord();

			voice_lead(playing_chord, target_chord, playing_chord, CHORD_SIZE);
			// print_array(target_chord, CHORD_SIZE);
			// print_array(starting_chord, CHORD_SIZE);
			schedule_note(playing_chord[0], 80, 0, -1.0);
			schedule_note(playing_chord[1], 80, 0, -1.0);
			schedule_note(playing_chord[2], 80, 0, -1.0);
			play_scheduled();
		}
	}

	// voice_lead(&starting_chord[0], &target_chord[0], &output[0], 4);
	//print_array(&output[0], 4);

	/*


	// schedule_note_relative(s2n("C4"), 127, 1.0, 0.0);
	// schedule_note_relative(s2n("E4"), 127, 1.0, 1.0);
	// schedule_note_relative(s2n("G4"), 127, 1.0, 1.0);
	// schedule_note_relative(s2n("C5"), 127, 3.0, 1.0);
	// schedule_note_relative(s2n("Bb4"), 127, 0.5, 3.0);
	// schedule_note_relative(s2n("G4"), 127, 0.5, 0.5);
	// schedule_note_relative(s2n("E4"), 127, 0.5, 0.5);
	// schedule_note_relative(s2n("C4"), 127, 0.5, 0.5);
	
	schedule_chord_relative(s2n("C5"), MAJOR, 1, 80, 2.0, 0.0);
	schedule_chord_relative(s2n("F4"), MAJOR, 2, 80, 2.0, 2.0);
	schedule_chord_relative(s2n("G4"), SUS4, 1, 80, 2.0, 1.0);
	for(int i = 0; i < 40; i++) {
		schedule_chord_relative(s2n("G4"), DOM_SEVEN, 0, 80, 2.0, 2.0);
		schedule_chord_relative(s2n("C5"), MAJOR, 2, 80, 2.0, 2.0);
	}
	//schedule_chord_relative(s2n("D5"), HALF_DIM_SEVEN, 1, 80, 2.0, 2.0);
	// for(int i = 0; i < 100; i++) {
	// 	schedule_chord_relative(s2n("C1") + i, MAJOR, 0, 80, 0.5, 0.5);
	// }

	play_scheduled();
	printf("Waiting... %d\n", __LINE__);

	getchar();
// 	while(0) {
// 		sleep(1);
// 		int result = snd_seq_event_output(sequencer, &midi_event);
// 		printf("%d\n", result);
// 		result = snd_seq_drain_output(sequencer);
// 		printf("%d\n", result);
// 	}
	
	*/
	snd_seq_close(sequencer);

	return 0;
}
