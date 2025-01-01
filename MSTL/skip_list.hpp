#ifndef MSTL_SKIP_LIST_HPP__
#define MSTL_SKIP_LIST_HPP__
#include "vector.hpp"
#include "algobase.hpp"
MSTL_BEGIN_NAMESPACE__

static const double Prob = 0.25;
static const unsigned short MAX_LEVEL = 32;

struct __skip_list_base_node {
	typedef unsigned short			level_type;
	typedef __skip_list_base_node*	base_ptr;
	typedef vector<base_ptr>		map_type;

	__skip_list_base_node(level_type level) 
		: next_(nullptr), 
		level_(level),
		nexts_(level, nullptr) {}

	base_ptr next_;
	level_type level_;
	map_type nexts_;
};

template <typename T>
struct __skip_list_node : public __skip_list_base_node {
	__skip_list_node(const T& value, level_type level) 
		: __skip_list_base_node(level), data_(value) {}

	~__skip_list_node() = default;

	T data_;
};

template<class Key, class Value>
class skip_list {
public:
	typedef pair<Key, Value>	value_type;
	typedef Key					key_type;
	typedef size_t				size_type;
	typedef __skip_list_node<value_type>		node_type;
	typedef __skip_list_node<value_type>*		link_type;
	typedef __skip_list_base_node::level_type	level_type;
	typedef __skip_list_base_node::base_ptr		base_ptr;
	typedef __skip_list_base_node::map_type		map_type;

private:
	level_type random_level() {
		level_type level = 1;
		while ((rand() / (RAND_MAX + 1.0) < Prob) && level < MAX_LEVEL)
			level++;
		return level;
	}

public:
	skip_list() {
		head_ = new node_type(value_type(), MAX_LEVEL);
	}
	~skip_list() {
		while (head_) {
			link_type next = link_type(head_->next_);
			delete head_;
			head_ = next;
		}
	}

	value_type get(const key_type& key) {
		if (not empty()) {
			level_type index = level_ - 1;
			node_type* cur = head_;

			while (index >= 0) {
				if (cur->nexts_[index] == nullptr)
					index--;
				else if (link_type(cur->nexts_[index])->data_.first < key)
					cur = cur->nexts_[index];
				else if (link_type(cur->nexts_[index])->data_.first >= key)
					index--;
				else
					return link_type(cur->nexts_[index])->data_;
			}
		}
		return value_type();
	}

	value_type put(const value_type& value) {
		level_type index = level_ - 1;
		base_ptr cur = head_;
		bool find = false;
		vector<link_type> prevs(level_);

		while (index >= 0) {
			if (cur->nexts_[index] == nullptr)
				prevs[index--] = cur;
			else if (link_type(cur->nexts_[index])->data_.first < value.first)
				cur = cur->nexts_[index];
			else if (link_type(cur->nexts_[index])->data_.first > value.first)
				prevs[index--] = cur;
			else {
				cur = cur->nexts_[index];
				find = true;
				break;
			}
		}
		if (find) {
			value_type tmp = link_type(cur)->data_;
			link_type(cur)->data_ = value;
			return tmp;
		}

		level_type newLevel = random_level();
		link_type newNode = new node_type(value, newLevel);
		for (int i = 0; i < newLevel; i++) {
			if (i >= level_) {
				head_->nexts_[i] = newNode;
			}
			else {
				newNode->nexts_[i] = prevs[i]->nexts_[i];
				prevs[i]->nexts_[i] = newNode;
			}
		}

		level_ = MSTL::maximum(newLevel, level_);
		size_++;
		return value;
	}

	bool remove(const key_type& key) {
		level_type index = level_ - 1;
		node_type* cur = head_;
		bool find = false;
		vector<node_type*> prevs(level_);

		while (index >= 0) {
			if (cur->nexts_[index] == nullptr)
				prevs[index--] = cur;
			else if (link_type(cur->nexts_[index])->data_.first < key)
				cur = cur->nexts_[index];
			else if (link_type(cur->nexts_[index])->data_.first > key)
				prevs[index--] = cur;
			else {
				prevs[index--] = cur;
				find = true;
			}
		}
		if (!find) return false;

		node_type* delNode = prevs[0]->nexts_[0];

		for (size_type i = 0; i < delNode->level_; i++)
			prevs[i]->nexts_[i] = delNode->nexts_[i];

		while (level_ > 0 && head_->nexts_[level_ - 1] == nullptr)
			level_--;

		size_--;
		delete delNode;
		return true;
	}
	bool empty() { return size_ == 0; }

private:
	size_t size_ = 0;
	link_type head_;
	level_type level_ = 0;
};

MSTL_END_NAMESPACE__
#endif // MSTL_SKIP_LIST_HPP__
