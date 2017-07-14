#ifndef TESTS_H
#define TESTS_H

#include <abstract.h>
#include <assert.h>
#include <initializer_list>
#include <type_traits>
#include <vector>

#define SIZE 100

namespace tests {

template <typename S>
void test_stack() {
	S stack;

	for (int i = 0; i < SIZE; i++) {
		stack.push(i);
	}

	assert(stack.size() == SIZE);
	assert(stack.top() == SIZE - 1);

	stack.clear();

	assert(stack.size() == 0);

	for (int i = 0; i < SIZE; i++) {
		stack.push(i);
	}

	for (int i = SIZE - 1; i >= 0; i--) {
		assert(stack.pop() == i);
	}

	// test for memory leaks
	for (int i = 0; i < SIZE; i++) {
		stack.push(i);
	}
}

template <typename Q>
void test_queue() {
	Q queue;

	for (int i = 0; i < SIZE; i++) {
		queue.enqueue(i);
	}

	assert(queue.size() == SIZE);
	assert(queue.front() == 0);
	assert(queue.back() == SIZE - 1);

	queue.clear();

	assert(queue.size() == 0);

	for (int i = 0; i < SIZE; i++) {
		queue.enqueue(i);
	}

	for (int i = 0; i < SIZE; i++) {
		assert(queue.dequeue() == i);
	}

	// test for memory leaks
	for (int i = 0; i < SIZE; i++) {
		queue.enqueue(i);
	}
}

template <typename L>
void test_list() {
	L list;

	for (int i = 0; i < SIZE - 1; i++) {
		list.push_front(i);
		assert(list.contains(i));
		assert(list.find(i) == 0);
	}

	list.insert(SIZE, SIZE / 2);
	assert(list.at(SIZE / 2) == SIZE);
	assert(list.size() == SIZE);
	assert(list.erase(SIZE / 2) == SIZE);

	for (int i = 0; i < SIZE - 1; i++) {
		assert(list.pop_back() == i);
		assert(!list.contains(i));
		assert(list.find(i) == list.size());
	}

	assert(list.size() == 0);

	for (int i = 0; i < SIZE; i++) {
		list.push_back(i);
		assert(list.contains(i));
		assert(list.find(i) == (unsigned) i);
	}

	assert(list.size() == SIZE);

	for (int i = 0; i < SIZE; i++) {
		assert(list.at(list.find(i)) == i);
	}

	for (int i = 0; i < SIZE; i++) {
		assert(list.pop_front() == i);
		assert(!list.contains(i));
		assert(list.find(i) == list.size());
	}

	assert(list.size() == 0);

	for (int i = 0; i < SIZE; i++) {
		list.push_back(i);
	}

	list.clear();
	assert(list.size() == 0);

	// test for memory leaks
	for (int i = 0; i < SIZE; i++) {
		list.push_back(i);
	}
}

template <typename S>
void test_set() {
	S set;

	for (double i = 0; i < SIZE; i++) {
		assert(set.insert(i));
		assert(!set.insert(i));
		assert(set.contains(i));
	}

	assert(set.size() == SIZE);

	for (double i = 0; i < SIZE; i++) {
		assert(set.remove(i));
		assert(!set.remove(i));
		assert(!set.contains(i));
	}

	for (double i = 0; i < SIZE; i++) {
		assert(set.insert(i));
	}

	auto items = set.items();
	set.clear();
	assert(set.size() == 0);

	for (double i = 0; i < SIZE; i++) {
		assert(items->contains(i));
		assert(!set.contains(i));
	}

	delete items;

	// test for memory leaks
	for (double i = 0; i < SIZE; i++) {
		assert(set.insert(i));
	}
}

template <typename Q>
void test_prio_queue() {
	Q pq;

	std::vector<int> v = {4, 5, 3, 2, 8, 9, 1, 7, 6};

	for (int i : v) {
		pq.push(i);
	}

	assert(pq.size() == 9);
	assert(pq.top() == 9);

	for (int i = 9; i >= 1; i--) {
		assert(pq.pop() == i);
	}

	// test for memory leaks
	for (int i : v) {
		pq.push(i);
	}
}

template <
	typename T,
	typename std::enable_if<
		std::is_base_of<structures::Stack<int>, T>::value, int>::type = 0>
void test_structure() {
	test_stack<T>();
}

template <
	typename T,
	typename std::enable_if<
		std::is_base_of<structures::Queue<int>, T>::value, int>::type = 0>
void test_structure() {
	test_queue<T>();
}

template <
	typename T,
	typename std::enable_if<
		std::is_base_of<structures::List<int>, T>::value &&
			!std::is_base_of<structures::Stack<int>, T>::value &&
			!std::is_base_of<structures::Queue<int>, T>::value,
		int>::type = 0>
void test_structure() {
	test_list<T>();
}

template <
	typename T,
	typename std::enable_if<
		std::is_base_of<structures::Set<double>, T>::value, int>::type = 0>
void test_structure() {
	test_set<T>();
}

template <
	typename T,
	typename std::enable_if<
		std::is_base_of<structures::PriorityQueue<int>, T>::value, int>::type =
		0>
void test_structure() {
	test_prio_queue<T>();
}

}  // namespace tests

#endif
