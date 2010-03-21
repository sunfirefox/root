// @(#)root/memstat:$Id$
// Author: Anar Manafov (A.Manafov@gsi.de) 2010-03-02

/*************************************************************************
* Copyright (C) 1995-2010, Rene Brun and Fons Rademakers.               *
* All rights reserved.                                                  *
*                                                                       *
* For the licensing terms see $ROOTSYS/LICENSE.                         *
* For the list of contributors see $ROOTSYS/README/CREDITS.             *
*************************************************************************/
#include "TMemStatBacktrace.h"

// STD
#include <cstdlib>

#ifndef __CINT__
// TODO: We need MacOSX at least 10.5. Implement version check in Makefile of
// ROOT when YAMS will be a pat of it.
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include <dlfcn.h>
// ROOT
#include "TString.h"

#if defined(R__GNU) && (defined(R__LINUX) || defined(R__HURD) || defined(__APPLE__)) && !defined(__alpha__)
#define SUPPORTS_MEMSTAT
#endif

// This is a global variable set at MSManager init time.
// It marks the highest used stack address.
void *g_global_stack_end = NULL;

#if defined(SUPPORTS_MEMSTAT)
// Comment from Anar:
// HACK: there is an ugly bug in gcc (Bug#8743): http://gcc.gnu.org/bugzilla/show_bug.cgi?id=8743
// "receiving result from __builtin_return_address() beyond stack top causes segfault"
// NOTE that __builtin_return_address should only be used with a non-zero argument for
// debugging purposes. So we use it on our risk.
// A workaround:
// This means the address is out of range.  Note that for the
// toplevel we see a frame pointer with value NULL which clearly is out of range.
// NOTE 2: With gcc 4.1, some optimization levels (e.g., -O, -Os, -O2) have started to imply -fomit-frame-pointer.
// One should use GCC builtin function with -fno-omit-frame-pointer option.
#define G__builtin_return_address(N) \
   ((__builtin_frame_address(N) == NULL)  || \
    (__builtin_frame_address(N) >= g_global_stack_end) || \
    (__builtin_frame_address(N) < __builtin_frame_address(0))) ? \
   NULL : __builtin_return_address(N)
// __builtin_return_address(0) yields the address to which the current
// function will return.  __builtin_return_address(1) yields the address to
// which the caller will return, and so on up the stack.
#define _RET_ADDR(x)   case x: return G__builtin_return_address(x);

#endif

namespace memstat {


//______________________________________________________________________________
   static void *return_address(int _frame)
   {
      // we have a limit on the depth = 35

#if defined(SUPPORTS_MEMSTAT)
      switch(_frame) {
            _RET_ADDR(0);
            _RET_ADDR(1);
            _RET_ADDR(2);
            _RET_ADDR(3);
            _RET_ADDR(4);
            _RET_ADDR(5);
            _RET_ADDR(6);
            _RET_ADDR(7);
            _RET_ADDR(8);
            _RET_ADDR(9);
            _RET_ADDR(10);
            _RET_ADDR(11);
            _RET_ADDR(12);
            _RET_ADDR(13);
            _RET_ADDR(14);
            _RET_ADDR(15);
            _RET_ADDR(16);
            _RET_ADDR(17);
            _RET_ADDR(18);
            _RET_ADDR(19);
            _RET_ADDR(20);
            _RET_ADDR(21);
            _RET_ADDR(22);
            _RET_ADDR(23);
            _RET_ADDR(24);
            _RET_ADDR(25);
            _RET_ADDR(26);
            _RET_ADDR(27);
            _RET_ADDR(28);
            _RET_ADDR(29);
            _RET_ADDR(30);
            _RET_ADDR(31);
            _RET_ADDR(32);
            _RET_ADDR(33);
            _RET_ADDR(34);
            _RET_ADDR(35);
         default:
            return 0;
      }
#else
      return 0;
#endif
   }

//______________________________________________________________________________
   size_t builtin_return_address(void **_container, size_t _limit)
   {
      size_t i(0);
      void *addr;
      for(i = 0; (i < _limit) && (addr = return_address(i)); ++i)
         _container[i] = addr;

      return i;
   }
//______________________________________________________________________________
   size_t getBacktrace(void **_trace, size_t _size, Bool_t _bUseGNUBuiltinBacktrace)
   {
      // Get the backtrace
      // _trace - array of pointers
      // _size - maximal deepness of stack information
      // _bUseGNUBuiltinBacktrace - whether to use gcc builtin backtrace or C library one.
      // The builtin version is much faster, but very sensitive and in some conditions could fail to return a proper result.
      // return value =  min(stack deepness, dsize)

      if(_bUseGNUBuiltinBacktrace) {
#if defined(SUPPORTS_MEMSTAT)
         // Initialize the stack end variable.
         return builtin_return_address(_trace, _size);
#endif
         return 0;
      }
      return backtrace(_trace, _size);
   }

//______________________________________________________________________________
   void getSymbols(void *_pAddr,
                   TString &/*_strInfo*/, TString &_strLib, TString &_strSymbol, TString &/*_strLine*/)
   {
      // get the name of the function and library

#if defined(SUPPORTS_MEMSTAT)
      Dl_info info;
      if(0 ==  dladdr(_pAddr, &info)) {
         return;
      }
      if(NULL != info.dli_sname) {
         int status(0);
         char *ch = abi::__cxa_demangle(info.dli_sname, 0, 0, &status);
         if(status < 0 || !ch)
            return;

         _strSymbol = (!status) ? ch : info.dli_sname;

         // it's our responsibility to free that pointer
         free(ch);
      }
      if(NULL != info.dli_fname)
         _strLib = info.dli_fname;
#else
      if(!_pAddr) {
         _strInfo = "";
         _strLib = "";
         _strSymbol = "";
      }
#endif
   }

//______________________________________________________________________________
   void getSymbolFullInfo(void *_pAddr, TString *_retInfo, const char *const _seporator)
   {

      if(!_retInfo)
         return;

#if defined(SUPPORTS_MEMSTAT)
      TString strInfo;
      TString strLib;
      TString strFun;
      TString strLine;
      getSymbols(_pAddr, strInfo, strLib, strFun, strLine);
      *_retInfo +=
         strInfo + _seporator +
         strLib + _seporator +
         strFun;
#else

#endif
   }

//______________________________________________________________________________
   void demangle(char *_codeInfo, TString &_str)
   {
      // demangle symbols

#if defined(SUPPORTS_MEMSTAT)
      int status = 0;
      char *ch = abi::__cxa_demangle(_codeInfo, 0, 0, &status);
      if(ch) {
         _str = ch;
         free(ch);
      } else {
         _str = "unknown";
      }
#else
      if(!_codeInfo) {
         _str = "";
      }
#endif
   }

}