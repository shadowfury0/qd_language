#ifndef __QD_HEADER_H__
#define __QD_HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <memory>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <array>
#include <vector>
#include <list>
#include <deque>
#include <queue>
#include <algorithm>


#include "qd_err.h"


#define _QD_BEGIN namespace qd_header{
#define _QD_END }
#define _USING_QD using namespace qd_header;

_QD_BEGIN

#define QD_API		extern

// NUM 
#define FIN_END  SIZE_MAX - 1
#define ERR_END  SIZE_MAX
#define QD_SIZE_MAX  SIZE_MAX
#define QD_INT32_MAX  INT32_MAX


// KEY WORD
#define QD_KYW_IF       "if"
#define QD_KYW_ELIF     "elif"
#define QD_KYW_ELSE     "else"
#define QD_KYW_TRUE     "true"
#define QD_KYW_FALSE    "false"
#define QD_KYW_JUMP     "jump"
#define QD_KYW_PASS     "pass"
#define QD_KYW_FOR      "for"
#define QD_KYW_WHILE    "while"
#define QD_KYW_GLOBAL   "global"
#define QD_KYW_LOCAL    "local"
#define QD_KYW_FUNC     "fun"
#define QD_KYW_RET      "ret"
#define QD_KYW_BRK      "break"
#define QD_KYW_IN       "in"
#define QD_KYW_DEFINE   "def"
#define QD_KYW_USING    "using"

//这个以后再全部重新改一下
using  _qd_b       =  bool;
using  _qd_int     =  int;
using  _qd_st      =  size_t;
using  _qd_uint    =  unsigned int;
using  _qd_char    =  char;
using  _qd_double    =  double;

#define  QD_STACK_MAX  100
#define  QD_CRLF  '13'
#define  QD_BUF_BLK_SIZ  1024


_QD_END


#endif