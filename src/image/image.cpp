
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "vgui/ISurface.h"
#include "materialsystem/imaterialsystem.h"

#include "image.h"
#include "macros.h"

// engine interfaces
extern vgui::ISurface* g_pSurface;
extern IMaterialSystem* materialsystem;

/*------------------------------------
	CImage::CImage()
------------------------------------*/
CImage::CImage( unsigned int width, unsigned int height )
	: m_Width( width ), m_Height( height ), m_pBits( NULL )
{

	if( m_Width == 0 || m_Height == 0 )
		return;

	// create the image buffer
	unsigned long size = ( m_Width * m_Height * 4 );
	m_pBits = new unsigned char[ size ];

	// zero it all out
	ZeroMemory( m_pBits, size );

}


/*------------------------------------
	CImage::~CImage()
------------------------------------*/
CImage::~CImage( )
{

	SAFE_DELETE_ARRAY( m_pBits );

}

/*------------------------------------
	CImage::SetPixel()
------------------------------------*/
void CImage::SetPixel(unsigned int x, unsigned int y, Color color )
{

	if( !m_pBits || x >= m_Width || y >= m_Height )
		return;

	// set pixel
	unsigned char* pixel = m_pBits + ( x + y * m_Width ) * 4;
	pixel[0] = static_cast<unsigned char>( color.r() );
	pixel[1] = static_cast<unsigned char>( color.g() );
	pixel[2] = static_cast<unsigned char>( color.b() );
	pixel[3] = static_cast<unsigned char>( color.a() );

}

/*------------------------------------
	CImage::GetPixel()
------------------------------------*/
Color CImage::GetPixel( unsigned int x, unsigned int y )
{

	if( !m_pBits || x >= m_Width || y >= m_Height )
		return Color( 255, 0, 255, 255 );

	// get
	unsigned char* pixel = m_pBits + ( x + y * m_Width ) * 4;
	return Color( pixel[0], pixel[1], pixel[2], pixel[3] );

}

/*------------------------------------
	CImage::Load()
------------------------------------*/
bool CImage::Load( const char* filename )
{

	DWORD readBytes;

	// open t he file
	HANDLE handle = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if( handle == INVALID_HANDLE_VALUE )
		return false;

	BITMAPFILEHEADER header;

	// read the header
	ReadFile( handle, &header, sizeof( BITMAPFILEHEADER ), &readBytes, NULL );
	if( header.bfType == 0x4D42 )
	{

		BITMAPINFOHEADER infoHeader;
		ReadFile( handle, &infoHeader, sizeof( BITMAPINFOHEADER ), &readBytes, NULL );

		// we dont' support this?
		if( infoHeader.biBitCount <= 8 || infoHeader.biCompression != BI_RGB )
		{

			// cleanup
			CloseHandle( handle );
			return false;

		}

		m_Width = infoHeader.biWidth;
		m_Height = infoHeader.biHeight;

		unsigned long bitmapSize = infoHeader.biWidth * infoHeader.biHeight * ( infoHeader.biBitCount / 8 );
		unsigned long imageSize = infoHeader.biWidth * infoHeader.biHeight * 4;

		// reallocate
		SAFE_DELETE_ARRAY( m_pBits );
		m_pBits = new unsigned char[ imageSize ];

		// read bitmap data
		unsigned char* buffer = new unsigned char[ bitmapSize ];
		SetFilePointer( handle, header.bfOffBits, NULL, SEEK_SET );
		ReadFile( handle, buffer, bitmapSize, &readBytes, NULL );

		// 16 bit
		if( infoHeader.biBitCount == 16 )
		{

			// convert to RGB32 from RGB555
			for( unsigned int y = 0; y < m_Height; y++ )
			{
				for( unsigned int x = 0; x < m_Width; x++ )
				{

					unsigned short source = *reinterpret_cast<unsigned short*>( buffer + ( x + y * m_Width ) * 2 );
					unsigned char* dest = m_pBits + ( x + ( ( m_Height - 1 ) - y ) * m_Width ) * 4;

					dest[0] = ( ( source & 0x7C00 ) >> 10 ) << 3;
					dest[1] = ( ( source & 0x3E0 ) >> 5 ) << 3;
					dest[2] = ( source & 0x1F ) << 3;
					dest[3] = 255;

				}
			}

		}
		// 24 bit
		else if( infoHeader.biBitCount == 24 )
		{

			// convert to RGB32 from RGB24
			for( unsigned int y = 0; y < m_Height; y++ )
			{
				for( unsigned int x = 0; x < m_Width; x++ )
				{

					unsigned char* source = buffer + ( x + y * m_Width ) * 3;
					unsigned char* dest = m_pBits + ( x + ( ( m_Height - 1 ) - y ) * m_Width ) * 4;

					dest[2] = source[0];
					dest[1] = source[1];
					dest[0] = source[2];
					dest[3] = 255;

				}
			}

		}
		// 32 bit
		else if( infoHeader.biBitCount == 32 )
		{

			// convert to RGB32 from RGB32
			for( unsigned int y = 0; y < m_Height; y++ )
			{
				for( unsigned int x = 0; x < m_Width; x++ )
				{

					unsigned char* source = buffer + ( x + y * m_Width ) * 4;
					unsigned char* dest = m_pBits + ( x + ( ( m_Height - 1 ) - y ) * m_Width ) * 4;

					dest[2] = source[0];
					dest[1] = source[1];
					dest[0] = source[2];
					dest[3] = 255;

				}
			}

		}

		// cleanup
		SAFE_DELETE_ARRAY( buffer );

	}

	// cleanup
	CloseHandle( handle );

	return true;

}

/*------------------------------------
	CImage::Save()
------------------------------------*/
bool CImage::Save( const char* filename )
{

	DWORD wroteBytes;

	// nothing to write!!!
	if( !m_pBits )
		return false;

	// open t he file
	HANDLE handle = CreateFile( filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL );
	if( handle == INVALID_HANDLE_VALUE )
		return false;

	// construct file header
	BITMAPFILEHEADER header;
	header.bfType = 0x4D42;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	header.bfOffBits = header.bfSize;

	// construct info header
	BITMAPINFOHEADER infoHeader;
	infoHeader.biBitCount = 24;
	infoHeader.biClrImportant = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biHeight = m_Height;
	infoHeader.biWidth = m_Width;
	infoHeader.biPlanes = 1;
	infoHeader.biSize = sizeof( BITMAPINFOHEADER );
	infoHeader.biSizeImage = m_Width * m_Height * 2;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;

	unsigned long imageSize = m_Width * m_Height * 3;
	unsigned char* buffer = new unsigned char[ imageSize ];
	
	// convert from RGB32 to RGB24
	for( unsigned int y = 0; y < m_Height; y++ )
	{
		for( unsigned int x = 0; x < m_Width; x++ )
		{

			unsigned char* source = m_pBits + ( x + y * m_Width ) * 4;
			unsigned char* dest = buffer + ( x + ( ( m_Height - 1 ) - y ) * m_Width ) * 3;

			dest[2] = source[0];
			dest[1] = source[1];
			dest[0] = source[2];

		}
	}

	// write to file
	WriteFile( handle, &header, sizeof( BITMAPFILEHEADER ), &wroteBytes, NULL );
	WriteFile( handle, &infoHeader, sizeof( BITMAPINFOHEADER ), &wroteBytes, NULL );
	WriteFile( handle, buffer, imageSize, &wroteBytes, NULL );

	// cleanup
	CloseHandle( handle );

	return true;

}


/*------------------------------------
	CImage::CopyRT()
------------------------------------*/
void CImage::CopyRT( int x, int y, unsigned int width, unsigned int height )
{

	m_Width = width;
	m_Height = height;

	// release old one
	SAFE_DELETE_ARRAY( m_pBits );

	// allocate a new buffer
	unsigned long imageSize = m_Width * m_Height * 4;
	m_pBits = new unsigned char[ imageSize ];

	// read
	IMatRenderContext* context = materialsystem->GetRenderContext();
	context->ReadPixels( x, y, width, height, m_pBits, IMAGE_FORMAT_RGBA8888 );
	SAFE_RELEASE( context );

}



/*------------------------------------
	CImage::LuaNewImage()
------------------------------------*/
int CImage::LuaNewImage( lua_State* L )
{

	Lua()->CheckType( 1, GLua::TYPE_NUMBER );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	int width = Lua()->GetInteger( 1 );
	int height = Lua()->GetInteger( 2 );

	// return a new writer
	CImage* image = new CImage( width, height );
	ILuaObject* metatable = Lua()->GetMetaTable( "CImage", TYPE_IMAGE );
	//!!! Lua()->PushUserData( metatable, image );
	SAFE_UNREF( metatable );

	return 1;

}


/*------------------------------------
	CImage::LuaDeleteImage()
------------------------------------*/
int CImage::LuaDeleteImage( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );

	// cleanup writer
	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );
	SAFE_DELETE( image );

	return 0;

}



/*------------------------------------
	CImage::LuaSetPixel()
------------------------------------*/
/*
int CImage::LuaSetPixel( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );
	Lua()->CheckType( 4, GLua::TYPE_TABLE );

	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );

	int x = Lua()->GetInteger( 2 );
	int y = Lua()->GetInteger( 3 );
	ILuaObject* color = Lua()->GetObject( 4 );

	// write
	image->SetPixel(
		x, y,
		Color(
			color->GetMemberInt( "r", 255 ),
			color->GetMemberInt( "g", 255 ),
			color->GetMemberInt( "b", 255 ),
			color->GetMemberInt( "a", 255 )
		)
	);

	SAFE_UNREF( color );

	return 0;

}
 */

/*------------------------------------
	CImage::LuaGetPixel()
------------------------------------*/
int CImage::LuaGetPixel( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	int x = Lua()->GetInteger( 2 );
	int y = Lua()->GetInteger( 3 );

	// read color
	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );
	Color& color =  image->GetPixel( x, y );

	// return as a lua color
	ILuaObject* obj = Lua()->GetNewTable();
	obj->SetMember( "r", static_cast<float>( color.r() ) );
	obj->SetMember( "g", static_cast<float>( color.g() ) );
	obj->SetMember( "b", static_cast<float>( color.b() ) );
	obj->SetMember( "a", static_cast<float>( color.a() ) );
	obj->Push();
	SAFE_UNREF( obj );

	return 1;

}


/*------------------------------------
	CImage::LuaCommit()
------------------------------------*/
int CImage::LuaCommit( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );
	int textureId = Lua()->GetInteger( 2 );

	// commit
	g_pSurface->DrawSetTextureRGBA( textureId, image->GetBuffer(), image->GetWidth(), image->GetHeight(), false, true );

	return 0;

}


/*------------------------------------
	CImage::LuaLoad()
------------------------------------*/
int CImage::LuaLoad( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );
	const char* filename = Lua()->GetString( 2 );

	// load
	//!!! if( !image->Load( filename ) )
	//!!!	Lua()->Msg( "Please ensure the bitmap is Uncompressed and 16, 24, or 32 bit.\n" );

	return 0;

}

/*------------------------------------
	CImage::LuaSave()
------------------------------------*/
int CImage::LuaSave( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );
	const char* filename = Lua()->GetString( 2 );

	// load
	//!!! if( !image->Save( filename ) )
	//!!!	Lua()->Msg( "Unable to write bitmap to file.\n" );

	return 0;

}

/*------------------------------------
	CImage::LuaCopyRT()
------------------------------------*/
int CImage::LuaCopyRT( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );
	Lua()->CheckType( 4, GLua::TYPE_NUMBER );
	Lua()->CheckType( 5, GLua::TYPE_NUMBER );

	int x = Lua()->GetInteger( 2 );
	int y = Lua()->GetInteger( 3 );
	int width = Lua()->GetInteger( 4 );
	int height = Lua()->GetInteger( 5 );

	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );
	image->CopyRT( x, y, width, height );

	return 0;

}

/*------------------------------------
	CImage::LuaGetWidth()
------------------------------------*/
int CImage::LuaGetWidth( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );

	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );
	
	// return width
	Lua()->Push( static_cast<float>( image->GetWidth() ) );
	return 1;

}

/*------------------------------------
	CImage::LuaGetHeight()
------------------------------------*/
int CImage::LuaGetHeight( lua_State* L )
{

	Lua()->CheckType( 1, TYPE_IMAGE );

	CImage* image = reinterpret_cast<CImage*>( Lua()->GetUserData( 1 ) );
	
	// return width
	Lua()->Push( static_cast<float>( image->GetHeight() ) );
	return 1;

}
