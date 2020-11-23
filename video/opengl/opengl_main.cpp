#define YY_EXPORTS
#include "yy.h"

#include "OpenGL.h"

yyVideoDriverAPI g_api;

u32 GetAPIVersion()
{
	return yyVideoDriverAPIVersion;
}

OpenGL * g_openGL = nullptr;

bool Init(yyWindow* window)
{
	if(g_openGL)
		return true;

	g_openGL = new OpenGL;
	if(!g_openGL->Init(window))
	{
		delete g_openGL;
		g_openGL = nullptr;
		return false;
	}
	return true;
}
void Destroy()
{
	if(g_openGL)
	{
		delete g_openGL;
		g_openGL = nullptr;
	}
}

extern "C"
{
	YY_API yyVideoDriverAPI* YY_C_DECL GetAPI()
	{
		g_api.GetAPIVersion = GetAPIVersion;
		g_api.Init          = Init;
		g_api.Destroy       = Destroy;

		return &g_api;
	}
}
