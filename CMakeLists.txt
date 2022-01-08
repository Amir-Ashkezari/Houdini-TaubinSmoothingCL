cmake_minimum_required( VERSION 3.6 )

project( SOP_TaubinSmoothingCl )

list( APPEND CMAKE_PREFIX_PATH "$ENV{HFS}/toolkit/cmake" )

find_package( Houdini REQUIRED )

set( library_name SOP_TaubinSmoothingCl )

houdini_generate_proto_headers( FILES SOP_TaubinSmoothingCl.cpp )

add_library( ${library_name} SHARED
    SOP_TaubinSmoothingCl.cpp
    SOP_TaubinSmoothingCl.h
    SOP_TaubinSmoothingCl_kernel.cpp
    SOP_TaubinSmoothingCl_kernel.h
    ThreadedObject.cpp
    ThreadedObject.h
)

set( OPENCL "$ENV{HFS}/custom/houdini/dsolib/OpenCL.lib" )

target_link_libraries( ${library_name} Houdini ${OPENCL} )

target_include_directories( ${library_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )

houdini_configure_target( ${library_name} )