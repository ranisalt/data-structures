#include <assert.h>
#include <tests.h>
#include <abstract.h>

void test_stack(structures::Stack<int>* stack) {
	for (int i = 0; i < SIZE; i++) {
		stack->push(i);
	}

	assert(stack->size() == SIZE);
	assert(stack->top() == SIZE - 1);

	stack->clear();

	assert(stack->size() == 0);

	for (int i = 0; i < SIZE; i++) {
		stack->push(i);
	}

	for (int i = SIZE - 1; i >= 0; i--) {
		assert(stack->pop() == i);
	}

	// test for memory leaks
	for (int i = 0; i < SIZE; i++) {
		stack->push(i);
	}
}