/*
 * BasicKey.h
 *
 *  Created on: Oct 31, 2012
 *      Author: girum
 */

#ifndef BASICKEY_H_
#define BASICKEY_H_

#include "Board.h"

template <unsigned int X>
class BasicKey : public Board::Key {
public:
	unsigned long vals[X];

	// Compare two keys for equality or less-than.
	bool operator==(const Board::Key &) const;
	bool operator<(const Board::Key &) const;

	// Override new and delete in order to maintain freelists
	// and mOutstanding count
	static void *operator new(size_t);
	static void operator delete(void *p);

	const Class *GetClass() const;

protected:
	std::istream &Read(std::istream &);
	std::ostream &Write(std::ostream &) const;

private:
	// TODO: Statically instantiate this freeList?
	static std::vector<BasicKey *> mFreeList;
};

template <unsigned int X>
std::vector<BasicKey<X> *> BasicKey<X>::mFreeList;

template <unsigned int X>
bool BasicKey<X>::operator==(const Board::Key &key) const {
	// downcast to a const BasicKey<X> here
	const BasicKey<X> *castedKey = dynamic_cast<const BasicKey<X> *>(&key);
	assert(castedKey != 0);

	for (unsigned int i = 0; i < X; i++)
		if (this->vals[i] != castedKey->vals[i])
			return false;
	return true;
}

template <unsigned int X>
bool BasicKey<X>::operator<(const Board::Key &key) const {
	// downcast to a const BasicKey<X> here
	const BasicKey<X> *castedKey = dynamic_cast<const BasicKey<X> *>(&key);
	assert(castedKey != 0);

	for (unsigned int i = 0; i < X; i++) {
		if (this->vals[i] < castedKey->vals[i])
			return true;
		else if (this->vals[i] > castedKey->vals[i])
			return false;
	}

	// == case.
	return false;
}

// TODO: Test BasicKey operator new() [template code isn't emitted until it runs]
template <unsigned int X>
static void *BasicKey<X>::operator new(size_t size) {
	void *temp;

	if (mFreeList.size()) {
	  temp = mFreeList.back();
	  mFreeList.pop_back();
	} else {
	  temp = ::new char[size];
	}

	Board::Key::mOutstanding++;
	return temp;
}

// TODO: Test BasicKey operator delete() [template code isn't emitted until it runs]
template <unsigned int X>
static void BasicKey<X>::operator delete(void *p) {

	mFreeList.push_back((BasicKey *)p);

	Board::Key::mOutstanding--;
}

// TODO: Implement reflection
template <unsigned int X>
const Class *BasicKey<X>::GetClass() const {
	return 0;
}

template <unsigned int X>
std::istream &BasicKey<X>::Read(std::istream &in) {
	for (unsigned int i = 0; i < X; i++)
		in >> vals[i];

	return in;
}

template <unsigned int X>
std::ostream &BasicKey<X>::Write(std::ostream &out) const {
	for (unsigned int i = 0; i < X; i++)
		out << vals[i];

	return out;
}


#endif /* BASICKEY_H_ */
