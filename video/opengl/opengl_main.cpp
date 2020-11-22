#define YY_EXPORTS
#include "yy.h"

#include "OpenGL.h"

yyVideoDriverAPI g_api;

u32 GetAPIVersion()
{
	return yyVideoDriverAPIVersion;
}

extern "C"
{
	YY_API yyVideoDriverAPI* YY_C_DECL GetAPI()
	{
		g_api.GetAPIVersion = GetAPIVersion;

		return &g_api;
	}
}
