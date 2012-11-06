#ifndef MYLIB_H
#define MYLIB_H

#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <exception>

#pragma warning (disable: 4996)

class FString : public std::string {
public:
   FString(char *fmt, ...) {
      static const int bufLen = 1024;
      static char buf[bufLen];
      va_list args;

      va_start(args, fmt);
      vsnprintf(buf, bufLen, fmt, args);
      va_end(args);

      *(std::string *)this = std::string(buf);
   }
};

// The consts in the parameter lists seem silly, but they keep the compiler
// from griping about mismatches between const long and long, for instance.
template <class T>
inline T Sqr(const T val) {return val*val;}

template <class T>
inline T TAbs(const T val) {return val < 0 ? -val : val;}

template <class T>
inline T TMax(const T v1, const T v2) {return v2 < v1 ? v1 : v2;}

template <class T>
inline T TMin(const T v1, const T v2) {return v1 < v2 ? v1 : v2;}

template <class T>
inline T Bound(const T lo, const T val, const T hi)
 {return val < lo ? lo : hi < val ? hi : val;}

template <class T>
inline int InOrder(const T lo, const T x, const T hi)
 {return !(x < lo || hi < x);}

template <class T>
inline int InRange(const T lo, const T x, const T hi)
 {return !(x < lo) && x < hi;}

template <class T>
struct TPtr {
   TPtr(T *p) : ptr(p) {}
   ~TPtr(void) {}

   T &operator*(void) const {return *ptr;}
   T *operator->(void) const {return ptr;}
   operator T*(void) const {return ptr;}

protected:
   T *ptr;
};

template <class T>
struct TCmpPtr : public TPtr<T> {
   TCmpPtr(T *p) : TPtr<T>(p) {}
   ~TCmpPtr(void) {}

   bool operator<(const TCmpPtr<T> &rhs) const 
    {return *TPtr<T>::ptr < *rhs.ptr;}

   bool operator==(const TCmpPtr<T> &rhs) const 
    {return *TPtr<T>::ptr == *rhs.ptr;}
};

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef char *CStr;

#ifdef LITTLE_ENDIAN

inline ushort EndianXfer(ushort val) {return val >> 8 | val << 8;}
inline short  EndianXfer(short val)  {return (val >> 8)&0xFF | val << 8;}
inline int    EndianXfer(int val)    {}
inline uint   EndianXfer(uint val)   {}

#else

inline ushort EndianXfer(ushort val) {return val;}
inline short  EndianXfer(short val)  {return val;}
inline int    EndianXfer(int val)    {return val;}
inline uint   EndianXfer(uint val)   {return val;}

#endif

class BaseException : public std::exception {
public:
   BaseException(const std::string &err) : mErr(err) {}
   ~BaseException() throw() {};
   const char *what() const throw() {return mErr.c_str();}

protected:
   std::string mErr;
};

#endif

