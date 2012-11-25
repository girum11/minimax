#include "Class.h"

using namespace std;

// Static initialization, just to be sure on C++11...
// TODO: Yank this.
Class *Class::mClsHead = NULL;
BoardClass *BoardClass::mBrdClsHead = NULL;

Class::Class(const string &n, Object *(*mCreate)()) {
   this->mName = n;
   this->mCreate = mCreate;

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
 void *(*optionGetter)(void), bool useXPos, int minPlayers) {

   this->mName = n;
   this->mCreate = mCreate;
   this->mFriendlyName = fn;
   this->mViewClass = mViewClass;
   this->mDlgClass = mDlgClass;
   this->optionSetter = optionSetter;
   this->optionGetter = optionGetter;
   this->mUseXPos = useXPos;
   this->mMinPlayers = minPlayers;

   // Add this BoardClass * to the linked list of BoardClass *'s
   this->mNext = BoardClass::mBrdClsHead;
   BoardClass::mBrdClsHead = this;
}

void *BoardClass::GetOptions() const {
   return NULL;
}

void BoardClass::SetOptions(void *) const {

}

const BoardClass *BoardClass::ForName(const string &name) {
   BoardClass *cursor = BoardClass::mBrdClsHead;

   while (cursor != NULL && cursor->mName.compare(name) != 0)
      cursor = cursor->mNext;  
   
   return cursor;
}