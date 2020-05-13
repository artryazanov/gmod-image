set(SOURCES
		image.cpp
		image.h
		macros.h
		main.cpp)

add_library(image SHARED ${SOURCES} ../../include/backwards_headers/ILuaInterface.h ../../include/backwards_headers/ILuaInterface.cpp ../../include/backwards_headers/ILuaModuleManager.h ../../include/backwards_headers/ILuaModuleManager.cpp ../../include/backwards_headers/ILuaObject.h ../../include/backwards_headers/ILuaObject.cpp ../../include/backwards_headers/ILuaTable.h ../../include/backwards_headers/LuaBase.h ../../include/backwards_headers/LuaInterface.h ../../include/backwards_headers/Types.h ../../include/backwards_headers/UserData.h)
target_link_libraries(image gmod-image)

set_gmod_suffix_prefix(image)