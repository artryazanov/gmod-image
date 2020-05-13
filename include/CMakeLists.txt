set(SOURCES
	backwards_headers/ILuaInterface.h
	backwards_headers/ILuaInterface.cpp
	backwards_headers/ILuaModuleManager.h
	backwards_headers/ILuaModuleManager.cpp
	backwards_headers/ILuaObject.h
	backwards_headers/ILuaObject.cpp
	backwards_headers/ILuaTable.h
	backwards_headers/LuaBase.h
	backwards_headers/LuaInterface.h
	backwards_headers/Types.h
	backwards_headers/UserData.h)

add_library(gmod-image INTERFACE)
target_include_directories(gmod-image INTERFACE ./ source-sdk-2013/mp/src/public source-sdk-2013/mp/src/public/tier0 source-sdk-2013/mp/src/public/tier1)

function(set_gmod_suffix_prefix library)
	SET_TARGET_PROPERTIES(${library} PROPERTIES PREFIX "gmcl_")

	if(APPLE)
		if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
			SET_TARGET_PROPERTIES(${library} PROPERTIES SUFFIX "_osx.dll")
		else()
			SET_TARGET_PROPERTIES(${library} PROPERTIES SUFFIX "_osx64.dll")
		endif()
	elseif(UNIX)
		if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
			SET_TARGET_PROPERTIES(${library} PROPERTIES SUFFIX "_linux.dll")
		else()
			SET_TARGET_PROPERTIES(${library} PROPERTIES SUFFIX "_linux64.dll")
		endif()
	elseif(WIN32)
		if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
			SET_TARGET_PROPERTIES(${library} PROPERTIES SUFFIX "_win32.dll")
		else()
			SET_TARGET_PROPERTIES(${library} PROPERTIES SUFFIX "_win64.dll")
		endif()
	endif()
endfunction()