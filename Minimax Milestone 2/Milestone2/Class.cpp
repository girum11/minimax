#include "Class.h"
#include <assert.h>

using namespace std;

// Static initialization, since C++11 apparently requires it.
Class *Class::mClsHead;
BoardClass *BoardClass::mBrdClsHead;


// Add this Class * to the linked list of Class *'s
Class::Class(const string &n, Object *(*c)()) : mName(n), mCreate(c) {
   Class *cursor = NULL;

   if (mClsHead == NULL) {
      mClsHead = this;
   }
   else {
      cursor = mClsHead;
      while(cursor->mNext != NULL) {
         cursor = cursor->mNext;
      }
      cursor->mNext = this;
      mNext = NULL;
   }
}

const Class *Class::ForName(const string &name) {
   Class *cursor = mClsHead;
   
   while (cursor && cursor->mName != name) {
      cursor = cursor->mNext;
   }
   return cursor;
}

Object *Class::NewInstance() const {
   return (*mCreate)();
}

// Constructor for a BoardClass should add that BoardClass to the static
// list of BoardClasses, NOT the static list of regular Classes.
BoardClass::BoardClass(const std::string &n, Object *(*c)(),
 const std::string &fn, Class *viewClass, Class *dlgClass,
 void (*setter)(const void *), void *(*getter)(), bool useXPos,
 int minPlayers) : Class(n,c), mFriendlyName(fn), mViewClass(viewClass),
 mDlgClass(dlgClass), optionSetter(setter), optionGetter(getter),
 mUseXPos(useXPos), mMinPlayers(minPlayers) {
    BoardClass *cursor = NULL;
    
    if (mBrdClsHead == NULL)
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
   return optionGetter();
}

void BoardClass::SetOptions(void *rules) const {
   optionSetter(rules);
}

vector<const BoardClass *> BoardClass::GetAllClasses() {
   vector<const BoardClass *> boardClassList;
   BoardClass *boardClassCursor = NULL;

   boardClassCursor = mBrdClsHead;
   
   while (boardClassCursor != NULL) {
      boardClassList.push_back(boardClassCursor);
      boardClassCursor = boardClassCursor->mNext;
   }

   return boardClassList;
}