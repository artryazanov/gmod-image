
#pragma once

#include "Color.h"

#include "backwards_headers/ILuaModuleManager.h"

// defines
#define TYPE_IMAGE Type::USERDATA

/*------------------------------------
	CImage
------------------------------------*/
class CImage
{
public:

	// construction/destruction
	CImage( unsigned int width, unsigned int height );
	~CImage();

	// pixel get/set
	void SetPixel(unsigned int x, unsigned int y, Color color );
	Color GetPixel( unsigned int x, unsigned int y );

	// load/save
	bool Load( const char* filename );
	bool Save( const char* filename );

	// load rendertarget
	void CopyRT( int x, int y, int width, int height );

	// accessors
	unsigned int GetWidth() { return m_Width; }
	unsigned int GetHeight() { return m_Height; }
	unsigned char* GetBuffer() { return m_pBits; }

	// lua methods
	static int LuaNewImage( lua_State* L );
	static int LuaDeleteImage( lua_State* L );
	static int LuaSetPixel( lua_State* L );
	static int LuaGetPixel( lua_State* L );
	static int LuaCommit( lua_State* L );
	static int LuaLoad( lua_State* L );
	static int LuaSave( lua_State* L );
	static int LuaCopyRT( lua_State* L );
	static int LuaGetHeight( lua_State* L );
	static int LuaGetWidth( lua_State* L );

private:

	unsigned int m_Width;
	unsigned int m_Height;

	// image bits
	unsigned char* m_pBits;

};