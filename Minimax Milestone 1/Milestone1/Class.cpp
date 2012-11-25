#include "Class.h"

using namespace std;

// Static initialization, just to be sure on C++11...
// TODO: Yank this.
Class *Class::mClsHead = NULL;
BoardClass *BoardClass::mBrdClsHead = NULL;

Class::Class() {
   // TODO: Delete me.
}

Class::Class(const string &n, Object *(*c)()) {
   this->mName = n;
   this->mCreate = c;

   // Add this Class * to the linked list of Class *'s
   this->mNext = Class::mClsHead;
   Class::mClsHead = this;
}

Object *Class::NewInstance() const {
   return NULL;
}

const Class *Class::ForName(const string &name) {
   return NULL;
}


BoardClass::BoardClass(const string &n, Object *(*c)(), const string &fn, 
 Class *mViewClass, Class *mDlgClass, void (*setter)(int), int (*getter)(void),
 bool useXPos, int minPlayers) {

   this->mName = n;
   this->mCreate = c;
   this->mFriendlyName = fn;
   this->mViewClass = mViewClass;
   this->mDlgClass = mDlgClass;
   this->setter = setter;
   this->getter = getter;
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

