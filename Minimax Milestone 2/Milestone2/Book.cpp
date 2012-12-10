#include "Book.h"
#include "Board.h"

using namespace std;

Book::~Book()
{
   Book::iterator itr;
   for (itr = begin(); itr != end(); ++itr) {
      delete itr->first;
   }
}

istream &Book::Read(istream &is, const Class *brdCls)
{
   Board *brd = dynamic_cast<Board*>(brdCls->NewInstance());
   Board::Key *key;
   Board::Move *move = brd->CreateMove(), *replyMove = brd->CreateMove();
   long value = 0;
   char temp;
   
   is.read(&temp, sizeof(char));
   mLevel = temp;
   
   while (!is.eof()) {
      key = const_cast<Board::Key *> (brd->GetKey());
      is >> *key;
      is >> *move;
      is.read(&temp, sizeof(char));
      if (temp == 1){
         is >> *replyMove;
      }
      is.read((char*)&value, sizeof(long));
      
      insert(value_type(key, BestMove(move->Clone(), temp == 1 ?
       replyMove->Clone() : 0, EndianXfer(value), mLevel, 0)));
   }
   
   delete brd;
   delete move;
   
   return is;
}

ostream &Book::Write(ostream &os)
{
   Book::iterator itr;
   char temp = mLevel;
   long value;
   
   os.write(&temp, sizeof(char));
   
   for (itr = begin(); itr != end(); ++itr) {
      os << *(itr->first);
      os << *(itr->second.move);
      temp = itr->second.replyMove ? 1 : 0;
      os.write(&temp, sizeof(char));
      if (temp == 1){
         os << *(itr->second.replyMove);
      }
      
      value = EndianXfer(itr->second.value);
      
      os.write((char*)&value, sizeof(long));
   }
   
   return os;
}