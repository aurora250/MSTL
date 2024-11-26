#ifndef MSTL_STRING_H__
#define MSTL_STRING_H__
#include "container.h"
#include "vector.hpp"
#include <initializer_list>

MSTL_BEGIN_NAMESPACE__

class string : public container {
public:
	typedef char		value_type;
	typedef string		self;
	typedef char* iterator;
	typedef const char* const_iterator;
	typedef size_t		size_type;

	static const char* const __type__;
	void __det__(std::ostream& _out = std::cout) const;
	void __show_data_only(std::ostream& _out) const;
private:
	char* _data;
	size_type _capacity;

	void __range_check(int _pos) const;
	bool __in_boundary(int _pos) const;
public:
	explicit string(const char* _str = "");
	explicit string(const string& _str); 
	explicit string(string&& _str);
	string& operator =(const string& _str);
	~string();
	void reserve(int _cap);
	void push(char _chr);
	char pop();
	void append(const char* _str);
	void append(const string& _str);
	void insert(int _pos, int _cpte, char _chr);
	void insert(int _pos, const char* _str);
	void insert(int _pos, const string& _str);
	void erase(int _pos, int _len = epos);
	void clear();
	string& copy(int _pos = 0, int _len = epos);
	void swap(string& _str);
	void resize(int _cap, char _chr = '\0');
	int find(char _chr, int _pos = 0);
	int find(const char* _str, int _pos = 0);

	iterator begin();
	iterator end();
	const_iterator const_begin() const;
	const_iterator const_end() const;

	char& operator [](int _pos);
	const char& operator [](int _pos) const;
	string& operator +=(char _chr);
	string& operator +=(const char* _str);

#define MSTL_STRING_CONVERT_DECLARE__(OPT) string& operator +=(OPT _tar)

	MSTL_STRING_CONVERT_DECLARE__(int);
	MSTL_STRING_CONVERT_DECLARE__(double);
	MSTL_STRING_CONVERT_DECLARE__(size_t);

	string& operator +=(const string& _str);
	bool operator <(const string& _str) const;
	bool operator >(const string& _str) const;
	bool operator <=(const string& _str) const;
	bool operator >=(const string& _str) const;
	bool operator !=(const string& _str) const;
	bool operator ==(const string& _str) const;
};

MSTL_END_NAMESPACE__

#endif // MSTL_STRING_H__
