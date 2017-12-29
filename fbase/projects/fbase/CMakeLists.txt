
set( SRC ) 
set(FBASE_RESOURCES)

set( SRC_FBASE 
	src/const.h
	src/FObj.h
	src/FObj.cpp
	src/File.cpp
	src/File.h
	src/Window.cpp
	src/Window.h
	src/DebugTools.h
	src/WinDebug.h
)

set( SRC_3D 
	src/3d/Mesh.h
	src/3d/Camera.h
	src/3d/VertexType.h
	src/3d/VertexBuffer.cpp
	src/3d/VertexBuffer.h
	src/3d/IndexBuffer.cpp
	src/3d/IndexBuffer.h )

set( SRC_ALGO
	src/math/algorithm/RadixSort.h
	src/math/algorithm/RadixSort.cpp )

set( SRC_MATH 
	src/math/Color.h
	src/math/Fraction.h
	src/math/math.h
	src/math/Matrix.h
	src/math/misc.h
	src/math/RectSize.h
	src/math/Vector.h
)

set( FX_D3D10 
	fx/d3d10/Flat.fx
	fx/d3d10/Particle.fx
)

set( FXH_D3D10 
	fx/d3d10/Common.fxh
)

set( SRC_D3D10 
	src/d3d10/D3d10_mingw.h
	src/d3d10/D3d10.cpp
	src/d3d10/D3d10.h
	src/d3d10/D3dEffect.cpp
	src/d3d10/D3dEffect.h
	src/d3d10/D3dEnums.cpp
	src/d3d10/D3dEnums.h
	src/d3d10/D3dIndexBuffer.cpp
	src/d3d10/D3dIndexBuffer.h
	src/d3d10/D3dVertexBuffer.cpp
	src/d3d10/D3dVertexBuffer.h
	src/d3d10/D3dTexture.cpp
	src/d3d10/D3dTexture.h
	src/d3d10/D3dRenderTarget.h
	${FX_D3D10}
	${FXH_D3D10}
)

set( SRC_CONTAINERS
	src/containers/Array.h
	src/containers/Array.cpp
	src/containers/Map.cpp
	src/containers/Map.h
	src/containers/String.h
	src/containers/ByteStream.h
	src/containers/containers.h
)


set( SRC ${SRC} ${SRC_FBASE} )
set( SRC ${SRC} ${SRC_CONTAINERS} )
set( SRC ${SRC} ${SRC_3D} )
set( SRC ${SRC} ${SRC_MATH} )
set( SRC ${SRC} ${SRC_ALGO} )

set_property(GLOBAL PROPERTY FBASE_INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src )
set_property(GLOBAL PROPERTY FBASE_LINK_LIBS fbase )
set_property(GLOBAL PROPERTY RESOURCES )

if(${WIN32})
	message( STATUS "WIN32 ON" )
	set_property(GLOBAL APPEND PROPERTY FBASE_LINK_LIBS winmm )
endif()

if(${D3D10})
	message( STATUS "D3D10 ON" )

	set( SRC ${SRC} ${SRC_D3D10} )

	include_directories($ENV{FBASEDIR}/SDK/DirectX/Include)
	set_property(GLOBAL APPEND PROPERTY FBASE_INC_DIRS $ENV{FBASEDIR}/SDK/DirectX/Include )
	set_property(GLOBAL PROPERTY FBASE_LIB_DIRS $ENV{FBASEDIR}/SDK/DirectX/Lib/x86 )
	set_property(GLOBAL APPEND PROPERTY FBASE_LINK_LIBS d3dx10 d3d10 )

	# Add shader compilation commands
	find_program( FXC_EXEC fxc.exe )
	foreach(FX ${FX_D3D10})
		# Set FXO so that the path is changed to tmp/ and extension from fx to fxo
		set(FXO ${FX})
		string( REPLACE ".fx" ".fxo" FXO ${FXO} )
		string( REPLACE "fx/d3d10/" "tmp/" FXO ${FXO} )

		# Add to global path list
		set(FXO_D3D10_GLOBAL ${FXO_D3D10_GLOBAL};${CMAKE_CURRENT_SOURCE_DIR}/${FXO} )

		# Command line
		set(FXC_CMD ${FXC_EXEC} /Tfx_4_0 /Ges /Fo ${CMAKE_CURRENT_SOURCE_DIR}/${FXO} ${CMAKE_CURRENT_SOURCE_DIR}/${FX})

		add_custom_command( OUTPUT ${FXO}
			COMMAND ${FXC_CMD}
			DEPENDS ${FX}
		        COMMENT Compiling ${FX}	)
		set( RESOURCES ${FXO} ${RESOURCES} )
	endforeach(FX)
	set_property(GLOBAL APPEND PROPERTY FBASE_RESOURCES ${FXO_D3D10_GLOBAL} )
	
	file(GLOB FBASE_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/fx/d3d10/*.fx )

	source_group( "fbase\\d3d10" src/d3d10/[a-zA-Z0-9]*.\(cpp|h\) )
	source_group( "fbase\\d3d10\\shaders" fx/d3d10/[a-zA-Z0-9]*.fx? )

endif(${D3D10})

message( STATUS "Resources: " ${RESOURCES} )

add_library( fbase STATIC
	${SRC} ${RESOURCES}
)	

source_group( "fbase" src/[a-zA-Z0-9]*.\(cpp|h\) )
source_group( "fbase\\containers" FILES ${SRC_CONTAINERS} )
source_group( "fbase\\3d" FILES ${SRC_3D} )
source_group( "fbase\\math" FILES ${SRC_MATH} )
source_group( "fbase\\math\\algorithm" FILES ${SRC_ALGO} )

