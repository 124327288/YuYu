﻿#ifndef _YY_VID_H_
#define _YY_VID_H_

struct yyVideoDriverObjectOpenGL
{
	yyVideoDriverObjectOpenGL() {}
};
struct yyVideoDriverObjectD3D11
{
	yyVideoDriverObjectD3D11()
	{

	}
	void * m_device;
	void * m_context;
};

const u32 yyVideoDriverAPIVersion = 1;
struct yyVideoDriverAPI
{
	enum class TextureSlot : u32
	{
		Texture0,
		Texture1,
		Texture2,
		Texture3,
		Texture4,
		Texture5,
		Texture6,
		Texture7,
		Texture8,
		Texture9,
		Texture10,
		Texture11,
		Texture12,
		Texture13,
		Texture14,
		Texture15,
		Texture16,
		Texture17,
		Texture18,
		Texture19,
		Texture20,

		Count
	};

	u32 (*GetAPIVersion)();

	bool (*Init)(yyWindow*);
	void (*Destroy)();
	
	void (*UseVSync)(bool);
	void(*UseDepth)(bool);
	void (*UseBlend)(bool);
	
	void (*SetClearColor)(f32 r, f32 g, f32 b, f32 a);
	
	// Вызвать перед началом рисования. После рисования обязательно нужно вызвать EndDraw и SwapBuffers
	void (*BeginDraw)();
	void (*ClearDepth)();  // очистить буфер глубины
	void (*ClearColor)();  // закрасить фон
	void (*ClearAll)();    // ClearDepth и ClearColor
	// завершение рисования
	// драйвер должен установить render target окна
	void (*EndDraw)();    
	// imgui рисуется между EndDraw и SwapBuffers
	void (*SwapBuffers)();       // Present\swapbuffers
	void(*UpdateMainRenderTarget)(const v2i& windowsSize, const v2f& bufferSize);

	void (*BeginDrawGUI)();
	void (*EndDrawGUI)();

	// Каждый вызов Create... создаёт абсолютно новый ресурс который останется в памяти на всё время
	// Ресурс хранит индекс на массив с указателями, которые указывают на реализацию ресурса (например текстура)
	// Реализацию можно выгрузить из памяти, и загрузить снова. Всё основано на подсчёте ссылок.
	//   это нужно иметь ввиду.
	// Если создаётся ресурс с использованием например yyImage* , 
	//   то, чтобы работали Unload... Load... нужно хранить этот yyImage* (ведь нужно знать из чего делать текстуру)
	// Unload... убавит счётчик ссылок на 1, и если будет 0 то реализация уничтожится
	// Load... Прибавит счётчик на 1. Если будет 1 то нужно заново создать реализацию. Если будет
	//   доступен resource->m_source то будет создано на основе него (если это текстура то m_source должен быть yyImage*)
	//   иначе будет попытка загрузить реализацию из файла
	yyResource* (*CreateTexture)(yyImage*, bool useLinearFilter, bool useComparisonFilter);
	yyResource* (*CreateTextureFromFile)(const char* fileName, bool useLinearFilter, bool useComparisonFilter, bool load);
	void (*UnloadTexture)(yyResource*); // --m_refCount; or unload
	void (*LoadTexture)(yyResource*); // ++m_refCount; or load

	yyResource* (*CreateModel)(yyModel*);
	yyResource* (*CreateModelFromFile)(const char* fileName, bool load);
	void (*UnloadModel)(yyResource*);
	void (*LoadModel)(yyResource*);
	void(*DeleteModel)(yyResource* r);
	void(*DeleteTexture)(yyResource* r);

	// yyResource::m_type MUST BE yyResourceType::Texture
	void (*SetTexture)(yyVideoDriverAPI::TextureSlot, yyResource*);
	// yyResource::m_type MUST BE yyResourceType::Model
	void (*SetModel)(yyResource*);
	void (*SetMaterial)(yyMaterial* mat);
	// draw what we set above
	void (*Draw)();
	
	void (*DrawSprite)(yySprite*);
	void (*DrawLine3D)(const v4f& _p1, const v4f& _p2, const yyColor& color);

	enum MatrixType
	{
		World,
		View,
		Projection,
		ViewProjection, //For 3d line
		WorldViewProjection,
		LightView,
		LightProjection,
	};
	void(*SetMatrix)(MatrixType, const Mat4&);
	
	v2f* (*GetSpriteCameraPosition)();
	v2f* (*GetSpriteCameraScale)();

	void (*GetTextureSize)(yyResource* r, v2i*);

	void (*MapModelForWriteVerts)(yyResource* r, u8** v_ptr);
	void(*UnmapModelForWriteVerts)(yyResource* r);

	yyResource* (*CreateRenderTargetTexture)(const v2f& size, bool useLinearFilter, bool useComparisonFilter);
	void(*SetRenderTarget)(yyResource*);
	void(*SetViewport)(f32 x, f32 y, f32 width, f32 height);


	void*(*GetVideoDriverObjects)();

	void(*test_draw)();
	
	const char* (*GetVideoDriverName)();

};

#endif