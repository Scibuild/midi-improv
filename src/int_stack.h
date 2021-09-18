#ifndef __INT_STACK_H
#define __INT_STACK_H

typedef struct {
  int* data;
	int head;
	int size;
} int_stack_t;

int int_stack_push(int_stack_t* stack, int data);
int int_stack_pop(int_stack_t* stack);
void int_stack_clear(int_stack_t* stack);
void int_stack_init(int_stack_t* stack, int size);
void int_stack_deinit(int_stack_t* stack);


#endif

