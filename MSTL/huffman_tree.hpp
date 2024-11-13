#ifndef MSTL_HUFFMAN_TREE_H__
#define MSTL_HUFFMAN_TREE_H__
#include "basiclib.h"
#include <string>
MSTL_BEGIN_NAMESPACE__

template <typename T>
struct _hf_tree_node {
	T data;
	int weight;
	int parent, left, right;
};
template <typename T>
struct hf_code {
	T data;
	std::string code;
};

template <typename T>
class huffman_tree {
public:
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef huffman_tree<T> self;
	typedef hf_code<T> code_type;
	typedef _hf_tree_node<T> node_type;
	typedef _hf_tree_node<T>* link_type;
private:
	link_type elem;
	int length;

	static const int MAX_INT = 32767;
public:
	huffman_tree(const pointer x, const int* w, size_t size) {
		int min1, min2;
		int x, y;
		length = 2 * size;
		elem = new node_type[length];
		for (int i = size; i < length; ++i) {
			elem[i].weight = w[i - size];
			elem[i].data = v[i - size];
			elem[i].parent = elem[i].left = elem[i].right = 0;
		}
		for (int i = size - 1; i > 0; --i) {
			min1 = min2 = MAX_INT;
			x = y = 0;
			for (int j = i + 1; j < length; ++j) {
				if (elem[j].parent == 0) {
					if (elem[j].weight < min1) {
						min2 = min1;
						min1 = elem[j].weight;
						y = x;
						x = j;
					}
					else if (elem[j].weight < min2) {
						min2 = elem[j].weight;
						y = j;
					}
				}
			}
			elem[i].weight = min1 + min2;
			elem[i].left = y;
			elem[i].right = x;
			elem[i].parent = 0;
			elem[x].parent = i;
			elem[y].parent = i;
		}
	}
	void getCode(code_type result[]) {
		int size = length / 2;
		int p, s;
		for (int i = size; i < length; ++i) {
			result[i - size].data = elem[i].data;
			result[i - size].code = "";
			p = elem[i].parent;
			s = i;
			while (p) {
				if (elem[p].left == s) {
					result[i - size].code = '0' + result[i - size].code;
				}
				else result[i - size].code = '1' + result[i - size].code;
				s = p;
				p = elem[p].parent;
			}
		}
	}
	~huffman_tree() { delete[]elem; }
};

MSTL_END_NAMESPACE__
#endif // MSTL_HUFFMAN_TREE_H__
