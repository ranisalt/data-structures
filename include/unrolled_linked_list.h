#ifndef STRUCTURES_UNROLLED_LINKED_LIST_H
#define STRUCTURES_UNROLLED_LINKED_LIST_H

#include <cstdint>
#include <memory>
#include <stdexcept>

namespace structures {
namespace detail {

constexpr int cache_line_size = 64;
constexpr size_t efficient_amount(int obj_size, int ptr_size, int lines = 1)
{
	return obj_size > cache_line_size * lines - ptr_size
		? efficient_amount(obj_size, ptr_size, lines + 1)
		: (cache_line_size * lines - ptr_size) / obj_size;
}

}

template <class T>
class UnrolledLinkedList final {
	struct Node final {
		static constexpr auto mask = 63u; // 0b111111

		constexpr Node() = default;
		Node(const Node& other): stor{other.stor}, next{other.next ? new Node{*other.next} : nullptr} {}

		T pop_back(size_t index) {
			if (index < capacity) {
				return std::move(stor[index]);
			}

			T&& data = next->pop_back(index - capacity);
			// TODO: fix boundary performance issue
			if (index == capacity) {
				next.release();
			}
			return std::move(data);
		}

		void push_back(T&& data, size_t index) {
			if (index < capacity) {
				stor[index] = std::move(data);
				return;
			}

			if (not next) {
				next.reset(new Node);
			}
			next->push_back(std::move(data), index - capacity);
		}

		// proxy functions
		T& at(size_t index) { return stor[index]; }
		const T& at(size_t index) const { return stor[index]; }

		// data and next should fit inside a single cache line
		static constexpr auto capacity = detail::efficient_amount(sizeof(T), sizeof(std::unique_ptr<Node>));
		std::array<T, capacity> stor;

		std::unique_ptr<Node> next = nullptr;
	};

public:
	constexpr UnrolledLinkedList() = default;
	~UnrolledLinkedList() = default;

	UnrolledLinkedList& operator=(const UnrolledLinkedList& rhs) {
		head = std::unique_ptr<Node>{new Node{*rhs.head}};
		size_ = rhs.size_;
		return *this;
	}

	T pop_back() {
		T&& data = head->pop_back(size_ - 1);
		--size_;
		return std::move(data);
	}

	void push_back(T data) {
		head->push_back(std::move(data), size_);
		++size_;
	}

	T& at(std::size_t index) {
		if (index >= size_) {
			throw std::out_of_range("Index out of bounds");
		}

		Node* it = head.get();
		while (index > it->capacity) {
			index -= it->capacity;
			it = it->next.get();
		}
		return it->at(index);
	}

	const T& at(std::size_t index) const {
		if (index >= size_) {
			throw std::out_of_range("Index out of bounds");
		}

		Node* it = head.get();
		while (index > it->capacity) {
			index -= it->capacity;
			it = it->next.get();
		}
		return it->at(index);
	}

	void clear() {
		head = std::unique_ptr<Node>{new Node};
		size_ = 0;
	}
	bool empty() const { return size_ == 0; }

	bool contains(const T& data) const { return find(data) != size_; }

	std::size_t find(const T& data) const {
		std::size_t index = 0;
		for (Node* it = head; it != nullptr; it = it->next) {
			if (it->data == data)
				break;
			++index;
		}
		return index;
	}

	T& back() { return at(size() - 1); }
	const T& back() const { return at(size() - 1); }
	std::size_t size() const { return size_; }

private:
	std::unique_ptr<Node> head{new Node};
	std::size_t size_{0u};
};

}  // namespace structures

#endif
