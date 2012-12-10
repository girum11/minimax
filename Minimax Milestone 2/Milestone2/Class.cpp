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

BoardClass::BoardClass(const std::string &n, Object *(*mCreate)(),
 const std::string &fn, Class *viewClass, Class *dlgClass,
 void (*optMutator)(const void *), void *(*optAccess)(), bool useXPos,
 int minPlayers) : Class(n,mCreate), mFriendlyName(fn), mViewClass(viewClass),
 mDlgClass(dlgClass), optionSetter(optMutator), optionGetter(optAccess),
 mUseXPos(useXPos), mMinPlayers(minPlayers) {
    BoardClass *cursor = NULL;
    
    if (mBrdClsHead == NULL) {
       mBrdClsHead = this;
    }
    else {
       cursor = mBrdClsHead;
       while (cursor->mNext != NULL) {
          cursor = cursor->mNext;
       }
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