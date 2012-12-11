#include "Class.h"
#include <assert.h>

using namespace std;

// Static initialization, since C++11 apparently requires it.
Class *Class::mClsHead = NULL;
BoardClass *BoardClass::mBrdClsHead = NULL;

Class::Class(const string &n, Object *(*c)()) : mName(n), mCreate(c) {
   Class *cursor = NULL;
   
   if (mClsHead == NULL)
      mClsHead = this;
   else {
      cursor = mClsHead;

      while (cursor->mNext != NULL)
         cursor = cursor->mNext;

      cursor->mNext = this;
      mNext = NULL;
   }
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
   BoardClass *cursor = NULL;
   
   if (!mBrdClsHead)
      mBrdClsHead = this;
   else {
      cursor = mBrdClsHead;

      while (cursor->mNext != NULL)
         cursor = cursor->mNext;

      cursor->mNext = this;
      this->mNext = NULL;
   }
}

void *BoardClass::GetOptions() const {
   return (*optionGetter)();
}

void BoardClass::SetOptions(void *options) const {
   (*optionSetter)(options);
}

const BoardClass *BoardClass::ForName(const string &name) {
   BoardClass *cursor = BoardClass::mBrdClsHead;

   while (cursor != NULL && cursor->mName.compare(name) != 0)
      cursor = cursor->mNext;  
   
   return cursor;
}