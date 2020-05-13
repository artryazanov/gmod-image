set(SOURCES
		main.cpp)

add_library(image SHARED ${SOURCES})
target_link_libraries(image gmod-image)

set_gmod_suffix_prefix(image)