set(SOURCES
		image.cpp)

add_library(image SHARED ${SOURCES})
target_link_libraries(image gmod-module-base)

set_gmod_suffix_prefix(image)