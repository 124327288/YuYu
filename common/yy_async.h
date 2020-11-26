﻿#ifndef _YUYU_ASYNC_H_
#define _YUYU_ASYNC_H_

#include "yy_resource.h"

//class yyAsyncEventHandler
//{
//public:
//	yyAsyncEventHandler(){}
//	virtual ~yyAsyncEventHandler(){}
//	virtual void onLoad(u32 userIndex, yyResource resource) = 0;
//};

// rawData only for:
//   - async yyImage loading
using yyAsyncLoadEventHandler = void(*)(u32 userIndex, yyResource resource, void* rawData);

extern "C"
{
	YY_API void YY_C_DECL yySetAsyncLoadEventHandler(yyAsyncLoadEventHandler);
}


#endif