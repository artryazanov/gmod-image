#define GMOD_USE_SOURCESDK

#if defined( _WIN32 ) && !defined( _X360 )
#include <windows.h>
#endif

#include "interface.h"
#include "eiface.h"
#include "vgui/ISurface.h"
#include "materialsystem/imaterialsystem.h"

#include "backwards_headers/ILuaModuleManager.h"

//#include "image.h"
//#include "macros.h"

// initialize the Garry's Mod module
//GMOD_MODULE( Init, Shutdown );

// engine interfaces
vgui::ISurface* g_pSurface = nullptr;
IMaterialSystem* materialsystem = nullptr;

/*------------------------------------
	CreateProceduralTexture()
------------------------------------*/
LUA_FUNCTION( CreateProceduralTexture )
{
    // push a new procedural texture id
    LUA->PushNumber( static_cast<float>( g_pSurface->CreateNewTextureID( true ) ) );
    return 1;
}

GMOD_MODULE_OPEN()
{
    // get material system
    auto materialSystemFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("MaterialSystem.dll"), "CreateInterface");
    materialsystem = static_cast<IMaterialSystem*>( materialSystemFactory( MATERIAL_SYSTEM_INTERFACE_VERSION, nullptr ) );
    if( !materialsystem )
        LUA->ThrowError( "Unable to get the IMaterialSystem interface, screen copy will be unavailable.\n" );

    // fetch the vgui material surface interface
    auto vguiSurfaceFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("vguimatsurface.dll"), "CreateInterface");
    if( vguiSurfaceFactory )
    {
        g_pSurface = static_cast<vgui::ISurface *>( vguiSurfaceFactory( VGUI_SURFACE_INTERFACE_VERSION, nullptr ) );
        if( !g_pSurface )
            LUA->ThrowError( "Unable to retreive the vgui surface interface.\n" );
    }

    //Lua()->NewGlobalTable( "image" );
    LUA->PushSpecial( SPECIAL_GLOB ); // +1
    LUA->PushString( "image" ); // +1
    LUA->CreateTable();
    LUA->SetTable( -3 ); // -2
    LUA->Pop(); // -1

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}