#ifndef _YUYU_H_
#define _YUYU_H_

#include "yy_config.h"
#include "yy_info.h"
#include "yy_types.h"
#include "yy_forward.h"
#include "yy_log.h"

constexpr u32 yyVideoDriverAPIVersion = 1;
struct yyVideoDriverAPI
{
	u32 (*GetAPIVersion)() = nullptr;

	bool (*Init)(yyWindow*)=nullptr;
	void (*Destroy)()=nullptr;
};


enum class yySystemState : u32 
{
	Run,
	Quit
};


extern "C"
{
	YY_API yySystemState* YY_C_DECL yyStart();
	YY_API void YY_C_DECL yyStop();
	YY_API void YY_C_DECL yyQuit();
	
	YY_API bool YY_C_DECL yyInitVideoDriver(const char* dl, yyWindow*);
	
	using dl_handle = void*;
	using dl_function = void*;
	YY_API dl_handle YY_C_DECL yyLoadLybrary(const char* libraryName);
	YY_API void YY_C_DECL yyFreeLybrary(dl_handle);
	YY_API dl_function YY_C_DECL yyGetProcAddress(dl_handle,const char* functionName);
}


#endif