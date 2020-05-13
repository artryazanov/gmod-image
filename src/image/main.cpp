#if defined( _WIN32 ) && !defined( _X360 )
#include <windows.h>
#endif

#include "interface.h"
#include "eiface.h"
#include "vgui/ISurface.h"
#include "materialsystem/imaterialsystem.h"

#include "backwards_headers/ILuaModuleManager.h"

#include "image.h"
#include "macros.h"

// initialize the Garry's Mod module
GMOD_MODULE( Init, Shutdown );

// engine interfaces
vgui::ISurface* g_pSurface = nullptr;
IMaterialSystem* materialsystem = nullptr;

/*------------------------------------
	CreateProceduralTexture()
------------------------------------*/
LUA_FUNCTION( CreateProceduralTexture )
{
    // push a new procedural texture id
    Lua()->Push( static_cast<float>( g_pSurface->CreateNewTextureID( true ) ) );
    return 1;
}

/*------------------------------------
	Init()
------------------------------------*/
int Init( lua_State* L )
{
    // only the client can use vgui surface.
    if( Lua()->IsClient() )
    {
        // get material system
        auto materialSystemFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("MaterialSystem.dll"), "CreateInterface");
        materialsystem = static_cast<IMaterialSystem*>( materialSystemFactory( MATERIAL_SYSTEM_INTERFACE_VERSION, nullptr ) );
        if( !materialsystem )
            Lua()->Error( "Unable to get the IMaterialSystem interface, screen copy will be unavailable.\n" );

        // fetch the vgui material surface interface
        auto vguiSurfaceFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("vguimatsurface.dll"), "CreateInterface");
        if( vguiSurfaceFactory )
        {
            g_pSurface = static_cast<vgui::ISurface *>( vguiSurfaceFactory( VGUI_SURFACE_INTERFACE_VERSION, nullptr ) );
            if( !g_pSurface )
                Lua()->ErrorNoHalt( "Unable to retreive the vgui surface interface.\n" );
        }
    }

    // create the module
    Lua()->NewGlobalTable( "image" );
    ILuaObject* image = Lua()->GetGlobal( "image" );
    if( image )
    {
        image->SetMember( "CreateImage", CImage::LuaNewImage );
        image->SetMember( "CreateProceduralTexture", CreateProceduralTexture );
    }
    SAFE_UNREF( image );

    // create the pixelwriter object
    ILuaObject* metatable = Lua()->GetMetaTable( "CImage", TYPE_IMAGE );
    if( metatable )
    {
        // garbage collection
        metatable->SetMember( "__gc", CImage::LuaDeleteImage );

        // set methods
        ILuaObject* methods = Lua()->GetNewTable();
        if( methods )
        {
            // functions
            methods->SetMember( "SetPixel", CImage::LuaSetPixel );
            methods->SetMember( "GetPixel", CImage::LuaGetPixel );
            methods->SetMember( "Load", CImage::LuaLoad );
            methods->SetMember( "Save", CImage::LuaSave );
            methods->SetMember( "GetWidth", CImage::LuaGetWidth );
            methods->SetMember( "GetHeight", CImage::LuaGetHeight );

            // only the client should get these, this allows the server to manipulate images
            // and the client to render them.
            if( Lua()->IsClient() )
            {
                methods->SetMember( "Commit", CImage::LuaCommit );
                methods->SetMember( "CopyRT", CImage::LuaCopyRT );
            }

            metatable->SetMember( "__index", methods );

        }
        SAFE_UNREF( methods );

    }
    SAFE_UNREF( metatable );

    return 0;
}

/*------------------------------------
	Shutdown()
------------------------------------*/
int Shutdown( lua_State* L )
{
    return 0;
}