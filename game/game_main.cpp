#include "yy.h"
#include "yy_window.h"

#include "yy_ptr.h"

#include "yy_async.h"
#include "yy_resource.h"

#include <cstdio>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;
#include <string>

#include "yy_image.h"

// for auto create\delete
struct EngineContext
{
	EngineContext()
	{
		m_state = yyStart(); // allocate memory for main class inside yuyu.dll
	}
	~EngineContext()
	{
		yyStop(); // destroy main class, free memory
	}

	yySystemState * m_state = nullptr;
};

void window_onCLose(yyWindow* window)
{
	yyQuit(); // change yySystemState - set yySystemState::Quit
}

// you can implement what you want (write to file, write to game console)
void log_onError(const char* message)
{
	fprintf(stderr,message);
}
void log_onInfo(const char* message)
{
	fprintf(stdout,message);
}

yyImage * g_img1 = nullptr;
yyImage * g_img2 = nullptr;
void asyncLoadEventHandler(u32 userIndex, yyResource resource, void* rawData)
{
	switch (userIndex)
	{
	default:
		break;
	case 1: g_img1 = (yyImage*)rawData; break;
	case 2: g_img2 = (yyImage*)rawData; break;
	}
}

int main()
{
	// I don't want to use stack memory, so for class\struct I will create new objects using heap
	// use yyPtr if you want auto destroy objects

	yyPtr<EngineContext> engineContext = new EngineContext;
	auto p_engineContext = engineContext.m_data;

	yySetAsyncLoadEventHandler(asyncLoadEventHandler);

	// set callbacks if you want to read some information 
	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onError);

	yyPtr<yyWindow> window = new yyWindow;
	auto p_window = window.m_data;

	if(!p_window->init(800,600))
	{
		YY_PRINT_FAILED;
		return 1;
	}

	p_window->m_onClose = window_onCLose;

	// init video driver
	const char * videoDriverType = "opengl.yyvd"; // for example read name from .ini
	if( !yyInitVideoDriver(videoDriverType, p_window) )
	{
		yyLogWriteWarning("Can't load video driver : %s\n", videoDriverType);

		// if failed, try to init other type
		std::vector<std::string> vidDrivers;
		for(const auto & entry : fs::directory_iterator(fs::current_path()))
		{
			auto path = entry.path();
			if(path.has_extension())
			{
				auto ex = path.extension();
				if(ex == ".yyvd")
				{
					yyLogWriteWarning("Trying to load video driver : %s\n", path.generic_string().c_str());

					if( yyInitVideoDriver(path.generic_string().c_str(), p_window) )
					{
						goto vidOk;
					}
					else
					{
						yyLogWriteWarning("Can't load video driver : %s\n", path.generic_string().c_str());
					}
				}
			}
		}
		YY_PRINT_FAILED;
		return 1;
	}

vidOk:
	
	auto videoDriver = yyGetVideoDriverAPI();
	videoDriver->SetClearColor(0.3f,0.3f,0.4f,1.f);

	yyLoadImageAsync("../res/grass.dds",1);
	yyLoadImageAsync("../res/grass.png",2);

	bool run = true;
	while( run )
	{
#ifdef YY_PLATFORM_WINDOWS
		MSG msg;
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			GetMessage( &msg, NULL, 0, 0 );
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
#else
#error For windows
#endif
		yyUpdateAsyncLoader();

		switch(*p_engineContext->m_state)
		{
		default:
			run = false;
			break;
		case yySystemState::Run:
		{
			videoDriver->BeginDrawClearAll();
			videoDriver->EndDraw();
		}break;
		}
	}

	if(g_img1) yyDeleteImage(g_img1);
	if(g_img2) yyDeleteImage(g_img2);

	return 0;
}