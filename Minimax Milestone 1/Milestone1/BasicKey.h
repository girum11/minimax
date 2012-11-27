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

   const Class *GetClass() const { return &mClass; };

protected:
	std::istream &Read(std::istream &);
	std::ostream &Write(std::ostream &) const;

private:
   static Class mClass;
   static Object *CreateBasicKey();
	static std::vector<BasicKey *> mFreeList;
};

template <unsigned int X>
std::vector<BasicKey<X> *> BasicKey<X>::mFreeList;

template <unsigned int X>
Object *BasicKey<X>::CreateBasicKey() {
   return new BasicKey<X>;
}

template <unsigned int X>
Class BasicKey<X>::mClass(FString("BasicKey<%d", X), &CreateBasicKey);

template <unsigned int X>
bool BasicKey<X>::operator==(const Board::Key &key) const {
	// downcast to a const BasicKey<X> here
	const BasicKey<X> *castedKey = dynamic_cast<const BasicKey<X> *>(&key);
	assert(castedKey != NULL);

	for (unsigned int i = 0; i < X; i++)
		if (this->vals[i] != castedKey->vals[i])
			return false;
	return true;
}

template <unsigned int X>
bool BasicKey<X>::operator<(const Board::Key &key) const {
	// downcast to a const BasicKey<X> here
	const BasicKey<X> *castedKey = dynamic_cast<const BasicKey<X> *>(&key);
	assert(castedKey != NULL);

	for (unsigned int i = 0; i < X; i++) {
		if (this->vals[i] < castedKey->vals[i])
			return true;
		else if (this->vals[i] > castedKey->vals[i])
			return false;
	}

	// == case.
	return false;
}

template <unsigned int X>
void *BasicKey<X>::operator new(size_t size) {
	void *temp;

	if (mFreeList.size()) {
	  temp = mFreeList.back();
	  mFreeList.pop_back();
	} else {
	  temp = ::new char[size];
	}

	mOutstanding++;
	return temp;
}

template <unsigned int X>
void BasicKey<X>::operator delete(void *p) {
	mFreeList.push_back((BasicKey *)p);
	mOutstanding--;
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
