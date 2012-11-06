#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>

// All classes represented by a Class object must derive from Object
class Class;

class Object {
public:
   virtual const Class *GetClass() const = 0;
};

// Exactly one object of Class type exists for each subclass C of Object.  The
// Class object supplies information about C itself, not about any one instance
// of C.  For instance, the Class object for OthelloBoard supplies information
// about the OthelloBoard class itself.
//
// You will want to obtain the Class object for a particular Object subclass
// based on its typename.  The ForName method provides this service.  For instance:
//
// Class::ForName("OthelloBoard")
//
// would return a pointer to the one-and-only Class object for the OthelloBoard
// class. Implementing ForName requires a means of getting to all Class objects.
// You will need to arrange a linked list of Class objects, with mClsHead as the
// head pointer.  Don't dynamically allocate Class objects just because you have 
// a linked list of them and pointers to them.  The phrase "new Class..." should
// never appear in your code.

class Class {
public:
   Class(const std::string &n, Object *(*c)());

   virtual Object *NewInstance() const;
   std::string GetName() const {return mName;}

   // Return the Class object for the given type name.
   static const Class *ForName(const std::string &name);

protected:
   std::string mName;       // Class type name (e.g. "OthelloBoard")
   Object *(*mCreate)();    // Pointer to creation function for subclass
   Class *mNext;            // Next Class on general list.

   static Class *mClsHead;  // Pointer to general list of Classes
};

// BoardClass provides more class-specific information relevant to a
// Board-derived class, in particular:
// 
// 1. A "friendly name" used to describe the game the Board plays e.g. "Chess".
//
// 2. Access to the view and dialog classes that go with the Board
//
// 3. Two methods that return and modify the static Rules for the board class.  
// Caller gets ownership of the object returned from GetOptions, 
// and retains ownership of the object passed to SetOptions
//
// 4. An indication of whether or not to use a transposition table when
// doing minimax.
//
// 5. A count of the minimum number of players needed to run a game
//
// 6. A method to return a vector of all BoardClass objects in existence.

class BoardClass : public Class {
public:
   
   // NOTE: Keep your added parameters and member data in the order shown!
   // NOTE: Do note place any methods inline except those already provided 
   // as inline.
   
   BoardClass(const std::string &n, Object *(*c)(), const std::string &fn,
    // Parameter to initialize the mViewClass member
    // Parameter to initialize the mDlgClass member
    // Function pointer parameter to the mutator for the options
    // Function pointer parameter to the accessor for the options
    bool useXPos = false, int minPlayers = 2);

   virtual std::string GetFriendlyName() const {return mFriendlyName;}
   virtual const Class *GetViewClass() const {return mViewClass;}
   virtual const Class *GetDlgClass() const {return mDlgClass;}
   virtual void *GetOptions() const;
   virtual void SetOptions(void *) const;
   virtual bool UseTransposition() const {return mUseXPos;}
   virtual int  GetMinPlayers() const {return mMinPlayers;}
   static std::vector<const BoardClass *> GetAllClasses();

protected:
   std::string mFriendlyName;
   const Class *mViewClass;
   const Class *mDlgClass;
   // Function pointer for SetOptions
   // Function pointer for GetOptions
   bool mUseXPos;      
   int mMinPlayers;             // Min number of players for game.      
   BoardClass *mNext;         
   
   static BoardClass *mBrdClsHead;
};

#endif
