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
		Node(const Node& other): stor{other.stor}, size{other.size}, next{other.next ? new Node{*other.next} : nullptr} {}

		T pop_back() {
			// if there is a next, there is more data ahead
			if (next) {
				T&& data = next->pop_back();
				// TODO: fix boundary performance issue
				if (next->size == 0) {
					next.release();
				}
				return std::move(data);
			}
			--size;
			return std::move(stor[size]);
		}

		void push_back(T&& data) {
			// if there is a next, there is more room ahead
			if (next) {
				next->push_back(std::move(data));
			} else {
				if (size == capacity()) {
					next.reset(new Node);
					next->push_back(std::move(data));
				} else {
					stor[size] = std::move(data);
					++size;
				}
			}
		}

		// proxy functions
		T& at(size_t index) { return stor[index]; }
		const T& at(size_t index) const { return stor[index]; }
		constexpr size_t capacity() const { return stor.max_size(); }

		// data and next should fit inside a single cache line
		static constexpr auto rsv = sizeof(uint8_t) + sizeof(std::unique_ptr<Node>);
		std::array<T, detail::efficient_amount(sizeof(T), rsv)> stor;

		// at most 56 bit-sized objects will fit, so int8 is enough
		// TODO: use lower bits of next to store size
		uint8_t size = 0u;
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
		T&& data = head->pop_back();
		--size_;
		return std::move(data);
	}

	void push_back(T data) {
		head->push_back(std::move(data));
		++size_;
	}

	T& at(std::size_t index) {
		if (index >= size_) {
			throw std::out_of_range("Index out of bounds");
		}

		Node* it = head.get();
		while (index > it->size) {
			index -= it->size;
			it = it->next.get();
		}
		return it->at(index);
	}

	const T& at(std::size_t index) const {
		if (index >= size_) {
			throw std::out_of_range("Index out of bounds");
		}

		Node* it = head.get();
		while (index > it->size) {
			index -= it->size;
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
