
message( STATUS "Project: FAPP" )

fbase_add_directories()

###########################################
# Resources

# Resources inherited from FBase
fbase_get_resources(FBASE_RESOURCES)

# Local resources
file(GLOB FAPP_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/final/data/?*.*) 

set(RESOURCES ${FAPP_RESOURCES} ${FBASE_RESOURCES})

# Process resources
set(RESOURCES_OUT)
foreach(RES_IN ${RESOURCES})
	# Extract file name
	string( REGEX MATCHALL "[a-zA-Z0-9]*[.][a-zA-Z0-9]*" RES_OUT ${RES_IN} )
	# Construct output directory
	set(RES_OUT ${CMAKE_CURRENT_BINARY_DIR}/data/${RES_OUT})

	# Copy it
	add_copy_rule(${RES_IN} ${RES_OUT})

	set(RESOURCES_OUT ${RESOURCES_OUT};${RES_OUT})
endforeach(RES_IN)


##############
# Executable

add_custom_command(TARGET fapp PRE_BUILD COMMAND "${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/data" COMMENT "Creating data dir")
add_executable( fapp src/main.cpp ${RESOURCES_OUT} )
fbase_link_libraries(fapp)


