#ifndef __QD_HEADER_H_
#define __QD_HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <deque>
#include <algorithm>

#define _QD_BEGIN namespace qd_header{
#define _QD_END }
#define _USING_QD using namespace qd_header;

_QD_BEGIN

// NUM 
#define FIN_END  UINT32_MAX - 1
#define ERR_END  UINT32_MAX
#define QD_INT_32_MAX INT32_MAX


//KEY WORD
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
#define QD_KYW_FUNC     "func"
#define QD_KYW_RET      "ret"
#define QD_KYW_IN       "in"


_QD_END


#endif