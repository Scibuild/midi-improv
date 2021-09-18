#include <stdlib.h>
#include "int_stack.h"

int int_stack_push(int_stack_t* stack, int data) {
	if(stack->head < stack->size)	{
		stack->data[stack->head++] = data;
		return 1;
	}
	return 0;
}

int int_stack_pop(int_stack_t* stack) {
	if(stack->head > 0) return stack->data[--stack->head];
	else return stack->data[0];
}

void int_stack_clear(int_stack_t* stack) {
	stack->head = 0;
}

void int_stack_init(int_stack_t* stack, int size) {
	// *stack = malloc(sizeof(int_stack_t));

	stack->head = 0;
	stack->size = size;
	stack->data = malloc(sizeof(int) * size);
}
void int_stack_deinit(int_stack_t* stack) {
	free(stack->data);
}

/*
void print_array(int *array, int array_length) {
	for(int i = 0; i < array_length; i++){
		printf("%d ", array[i]);
	}
	putchar('\n');
}
*/
