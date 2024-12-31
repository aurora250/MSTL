#ifndef MSTL_SKIP_LIST_HPP__
#define MSTL_SKIP_LIST_HPP__
#include <cmath>
#include <cassert>

template<class K, class V>
class SkipList
{
	struct ListNode
	{
		ListNode(const K& key, const V& value, int level) :
			_key(key), _value(value), _next(nullptr), _level(level), _nexts(new ListNode* [_level])
		{
			for (int i = 0; i < level; i++)
				_nexts[i] = nullptr;
		}

		~ListNode()
		{
			delete[] _nexts;
		}

		K _key;
		V _value;
		ListNode* _next;
		int _level;
		ListNode** _nexts;
	};


	typedef ListNode Node;

public:
	SkipList()
	{
		_head = new Node(K(), V(), MAX_LEVEL);
	}

	~SkipList()
	{

		while (_head)
		{
			Node* next = _head->_next;
			delete _head;
			_head = next;
		}

	}

	V get(const K& key)
	{
		assert(!empty());

		int index = _level - 1;
		Node* cur = _head;

		while (index >= 0)
		{
			if (cur->_nexts[index] == nullptr)
				index--;
			else if (cur->_nexts[index]->_key < key)
				cur = cur->_nexts[index];
			else if (cur->_nexts[index]->_key >= key)
				index--;
			else
				return cur->_nexts[index]->_value;
		}

		return V();
	}


	V put(const K& key, const V& value)
	{

		int index = _level - 1;
		Node* cur = _head;
		bool find = false;

		Node** prevs = new  Node * [_level];

		while (index >= 0)
		{
			if (cur->_nexts[index] == nullptr)
				prevs[index--] = cur;
			else if (cur->_nexts[index]->_key < key)
				cur = cur->_nexts[index];
			else if (cur->_nexts[index]->_key > key)
				prevs[index--] = cur;
			else
			{
				cur = cur->_nexts[index];
				find = true;
				break;
			}

		}//end of while

		if (find)
		{
			V tmp = cur->_value;
			cur->_value = value;

			delete[] prevs;
			return  tmp;
		}

		int newLevel = randomLevel();
		Node* newNode = new Node(key, value, newLevel);
		for (int i = 0; i < newLevel; i++)
		{
			if (i >= _level)
			{
				_head->_nexts[i] = newNode;
			}
			else
			{
				newNode->_nexts[i] = prevs[i]->_nexts[i];
				prevs[i]->_nexts[i] = newNode;
			}
		}

		_level = __max(newLevel, _level);
		_size++;


		delete[] prevs;
		return value;
	}


	bool remove(const K& key)
	{

		int index = _level - 1;
		Node* cur = _head;
		bool find = false;

		Node** prevs = new  Node * [_level];

		while (index >= 0)
		{
			if (cur->_nexts[index] == nullptr)
				prevs[index--] = cur;
			else if (cur->_nexts[index]->_key < key)
				cur = cur->_nexts[index];
			else if (cur->_nexts[index]->_key > key)
				prevs[index--] = cur;
			else {
				prevs[index--] = cur;
				find = true;
			}
		}

		if (!find)
		{
			delete[] prevs;
			return false;
		}

		Node* delNode = prevs[0]->_nexts[0];

		for (int i = 0; i < delNode->_level; i++)
			prevs[i]->_nexts[i] = delNode->_nexts[i];

		//更新层数
		while (_level > 0 && _head->_nexts[_level - 1] == nullptr)
			_level--;

		_size--;
		delete delNode;
		delete[] prevs;
		return true;
	}


	int randomLevel()
	{

		int level = 1;
		while ((rand() / (RAND_MAX + 1.0) < P) && level < MAX_LEVEL)
			level++;

		return level;
	}
	bool empty()
	{
		return _size == 0;
	}

public:
	const double P = 0.25;			//概率 一般取 0.5 或0.25
	const int MAX_LEVEL = 32;
	size_t _size = 0;
	Node* _head;
	int _level = 0;
};


#endif // MSTL_SKIP_LIST_HPP__
