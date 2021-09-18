#include "permutations.h"
#include "int_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*
int factorial(int n) {
	if(n == 0) return 1;
	if(n == 1) return 1;
	int x = 1;
	for (int i = 2; i <= n; i++) 
		x *= i;
	return x;
}
*/

// https://stackoverflow.com/questions/32443379/an-algorithm-for-enumerating-all-permutations-of-the-numbers-1-2-n-using-a
/*
void permute(int n) {
	int fill_from_index = 0;
	stack_t stack;
	init_stack(&stack, n);
	int* permutation = malloc(sizeof(int) * n);
	
	// initialise with 0, 1, 2, ...
	for (int i = 0; i < n; i++) {
		push(&stack, i);
	}

	// how many times are we permuting? n! (factorial lol)
	int fact = factorial(n);
	for(int k = 0; k < fact; k++) {
		// pop the stack into the permutation array
		for (int i = 0; i < n; i++) {
			permutation[i] = pop(&stack);
		}
		// eventually change this to some kind of callback maybe?
		print_array(permutation, n);

		int i = fill_from_index;
		// push pack onto the stack, but starting from fill index and looping round
		do  {
			push(&stack, permutation[i]);
			i++;
			i %= n;
		} while (i != fill_from_index);
		// increment fill index
		fill_from_index++;
		fill_from_index %= n;
	}

	free(permutation);
	deinit_stack(&stack);
}
*/

void print_int_array(int* array, int length) {
	for(int i = 0; i < length; i++) {
		printf("%d ", array[i]);
	}
}

// https://en.wikipedia.org/wiki/Factorial_number_system#Permutations
void permute(int n, void (*callback)(int *perm, void *params), void *params) {
	int* factorials = malloc(sizeof(int) * n);
	int* tempOrderedArray = malloc(sizeof(int) * n);
	int_stack_t onePermutation;
	int_stack_init(&onePermutation, n);
	factorials[0] = 1;
	// fill the factorials array with factorials
	for (int i = 1; i <= n; i++) {
		factorials[i] = factorials[i - 1] * i;
	}
	
	for (int i = 0; i < factorials[n]; i++) {
		int_stack_clear(&onePermutation);
		// fill this temp ordered array from 0..n-1 and by taking elements out of it, we ensure no duplicates
		for(int j = 0; j < n; j++)
			tempOrderedArray[j] = j;
		// positionCode is the number we turn into factoradic (factorial base)
		int positionCode = i;

#ifdef DEBUG_PERMUTE
		printf("--------------------------\n");
		printf("i = %d\t\t", i);
		printf("positionCode = %d\n", positionCode);
#endif

		for(int position = n; position > 0; position--) {
			// get the value in the (position-1) place of positionCode in factoradic
			int selected = positionCode / factorials[position-1];

#ifdef DEBUG_PERMUTE
			printf("  position = %d  factorials[position-1] = %d\n", position, factorials[position-1]);
			printf("  selected = %d  tempOrderedArray[selected] = %d\n", selected, tempOrderedArray[selected]);
#endif

			// push that value from the ordered array into the permutation stack
			int_stack_push(&onePermutation, tempOrderedArray[selected]);

#ifdef DEBUG_PERMUTE
			printf("  onePermutation = [ ");
			print_int_array(onePermutation.data,onePermutation.head);
			printf("]\n");
#endif

			// the modulo is so we can find the next factoradic digit, by taking 
			// the remainder of what was left over
			// its just like, to find the hundreds digit of 325, divide by 100, get 3
			// to get the rest that needs to be converted, modulo 100, get 25
			positionCode %= factorials[position - 1];
			// delete the item at selected from the ordered array so it can't be chosen again
			memmove(
					&tempOrderedArray[selected], 
					&tempOrderedArray[selected+1], 
					(position-selected-1)*sizeof(int));

#ifdef DEBUG_PERMUTE
			printf("  positionCode = %d  tempOrderedArray = [ ", positionCode);
			print_int_array(tempOrderedArray, position - 1);
			printf("]\n\n");
#endif
		}
		// print_array(onePermutation.data, n);
		// run the callback given with the data, this could do almost anything
		// but any state must be closed over in the params
		callback(onePermutation.data, params);
		
	}
#ifdef DEBUG_PERMUTE
	printf("--------------------------\n");
#endif

	int_stack_deinit(&onePermutation);
	free(factorials);
}
