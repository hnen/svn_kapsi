
cmake_minimum_required(VERSION 2.8)
project(fdev)

set(WIN32 1)
set(D3D10 1)

macro(fbase_link_libraries TARGETNAME)
	get_property(FBASE_LIBS GLOBAL PROPERTY FBASE_LINK_LIBS )
	target_link_libraries( ${TARGETNAME} ${FBASE_LIBS} )
endmacro(fbase_link_libraries)

macro(fbase_add_directories)
	get_property(FBASE_INCDIRS GLOBAL PROPERTY FBASE_INC_DIRS )
	include_directories(${FBASE_INCDIRS})
	get_property(FBASE_LIBDIRS GLOBAL PROPERTY FBASE_LIB_DIRS )
	link_directories(${FBASE_LIBDIRS})
endmacro(fbase_add_directories)

macro(fbase_get_resources DESTVAR)
	get_property(${DESTVAR} GLOBAL PROPERTY FBASE_RESOURCES )
endmacro(fbase_get_resources)

macro(add_copy_rule FROM TO)
	add_custom_command( OUTPUT ${TO}
		COMMAND ${CMAKE_COMMAND} -E copy ${FROM} ${TO}
		DEPENDS ${FROM} COMMENT Copying ${FROM} )
endmacro(add_copy_rule)

if(${WIN32})
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(${WIN32})

add_subdirectory(fbase)
add_subdirectory(fapp)

