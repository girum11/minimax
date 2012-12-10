#include "Class.h"
#include <assert.h>

using namespace std;

// Static initialization, since C++11 apparently requires it.
Class *Class::mClsHead = NULL;
BoardClass *BoardClass::mBrdClsHead = NULL;

Class::Class(const string &n, Object *(*mCreate)()) 
 : mName(n), mCreate(mCreate) {
   // Add this Class * to the linked list of Class *'s
   this->mNext = Class::mClsHead;
   Class::mClsHead = this;
}

Object *Class::NewInstance() const {
   return (*mCreate)();
}

const Class *Class::ForName(const string &name) {
   Class *cursor = Class::mClsHead;

   while (cursor != NULL && cursor->mName.compare(name) != 0)
      cursor = cursor->mNext;  
   
   return cursor;
}

BoardClass::BoardClass(const string &n, Object *(*mCreate)(), const string &fn, 
 Class *mViewClass, Class *mDlgClass, void (*optionSetter)(const void *), 
 void *(*optionGetter)(void), bool useXPos, int minPlayers) 
 : Class(n,mCreate), mFriendlyName(fn), mViewClass(mViewClass), 
 mDlgClass(mDlgClass), optionSetter(optionSetter), optionGetter(optionGetter),
 mUseXPos(mUseXPos), mMinPlayers(mMinPlayers) {
   // Add this BoardClass * to the linked list of BoardClass *'s
   this->mNext = BoardClass::mBrdClsHead;
   BoardClass::mBrdClsHead = this;
}

void *BoardClass::GetOptions() const {
   return (*optionGetter)();
}

void BoardClass::SetOptions(void *options) const {
   (*optionSetter)(options);
}