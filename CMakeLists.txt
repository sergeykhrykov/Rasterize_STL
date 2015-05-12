# CMAKE Build file
# -------------------------------- 
# Expects passed vars: 
# MB_BIT, 
# MB_BUILD_CONFIG,  
# MB_INSTALL_DEPENDENCY_DLLS.
# Note that platform (32/64 bit) is actually chosen with cmake -G command line option. 
# MB_BIT won't affect target platform
# --------------------------------

# SET UP VERSION INFORMATION
cmake_minimum_required(VERSION 3.1)

# No idea what it does
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
if(POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

project("Rasterize_STL")

# --------------------------------
# Set variables for build configuration
# Restrict possible configs to the chosen one
set(CMAKE_CONFIGURATION_TYPES "${MB_BUILD_CONFIG}" CACHE STRING "Type of configuration" FORCE)

set (CMAKE_BUILD_TYPE ${MB_BUILD_CONFIG} CACHE STRING "Choose the type of build." FORCE)

# Set proper build type for selecting QT libraries
if(CMAKE_BUILD_TYPE EQUAL "Debug")
	set (QT_BUILD_TYPE "DEBUG")
elseif(CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
	set (QT_BUILD_TYPE "RELEASE")
endif(CMAKE_BUILD_TYPE EQUAL "Debug")
	
# Set variables for 32 or 64 bit
if(${CMAKE_SIZEOF_VOID_P} MATCHES 4)
	message(STATUS "Configuring for 32 bit")
	set(BIT 32)
	set(BIT_S "32")	
	
else(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	message(STATUS "Configuring for 64 bit")
	set(BIT 64)
	set(BIT_S "64")		
endif()

set(OpenGL_VERSION ${MB_OPENGL_VERSION})
message(STATUS "OpenGL version is ${OpenGL_VERSION}")

# Set path to find VTK
set(VTK_ROOT_PATH "C:/dev/VTK/_install/${BIT}bit.${OpenGL_VERSION}/${CMAKE_BUILD_TYPE}" CACHE PATH "VTK directory" FORCE)
set(VTK_PATH "${VTK_ROOT_PATH}/lib/cmake/vtk-6.3/")

# --------------------------------

# Set path to find ITK
set(ITK_ROOT_PATH "C:/dev/ITK/_install/${BIT}bit/${CMAKE_BUILD_TYPE}" CACHE PATH "ITK directory" FORCE)
set(ITK_PATH "${ITK_ROOT_PATH}/lib/cmake")

# --------------------------------

if (BIT MATCHES 32)
	set (QT_PATH ${QT_PATH_32})
else (BIT MATCHES 64)
	set (QT_PATH ${QT_PATH_64})
endif()

set(PROJ_NAME "Rasterize")

# For MinGW, I suppose
if (NOT MSVC) 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(NOT MSVC)

# Find VTK
message ("")
message (STATUS "--------------------------------")
set (CMAKE_PREFIX_PATH "${VTK_PATH};${CMAKE_PREFIX_PATH}")
message (STATUS "Looking for VTK at: ${VTK_PATH}")

set (VTK_MODULES vtkFiltersModeling vtkIOGeometry)

if("${OpenGL_VERSION}" STREQUAL "OpenGL")
	list (APPEND VTK_MODULES vtkRenderingOpenGL)
	
elseif("${OpenGL_VERSION}" STREQUAL "OpenGL2")
	list (APPEND VTK_MODULES vtkRenderingOpenGL2)	
	
endif("${OpenGL_VERSION}" STREQUAL "OpenGL")

find_package(VTK REQUIRED NO_MODULE COMPONENTS ${VTK_MODULES} PATHS ${VTK_PATH} NO_DEFAULT_PATH)

include(${VTK_USE_FILE})

if (VTK_FOUND)
	message (STATUS "Found VTK at: ${VTK_DIR}")
	
elseif (NOT VTK_FOUND)
	message (FATAL_ERROR "Could not find VTK, please check the path above")
	
endif (VTK_FOUND)

# Find ITK
message ("")
message (STATUS "--------------------------------")
set (CMAKE_PREFIX_PATH "${ITK_PATH};${CMAKE_PREFIX_PATH}")
message (STATUS "Looking for ITK at: ${ITK_PATH}")

# set (ITK_MODULES ITKVTK ITKSpatialObjects ) # needs rework

find_package(ITK REQUIRED PATHS ${ITK_PATH} NO_DEFAULT_PATH)

include(${ITK_USE_FILE})

if (ITK_FOUND)
	message (STATUS "Found ITK at: ${ITK_DIR}")
	
elseif (NOT ITK_FOUND)
	message (FATAL_ERROR "Could not find ITK, please check the path above")
	
endif (ITK_FOUND)

message (STATUS ":::ITK_LIBRARIES = ${ITK_LIBRARIES}")

add_executable( ${PROJ_NAME} Code.cxx )
target_link_libraries( ${PROJ_NAME} debug ${ITK_LIBRARIES} debug ${VTK_LIBRARIES})

#install( TARGETS ConvertTriangleMeshToBinaryImage
#  DESTINATION bin/ITKExamples/Core/Mesh
#  COMPONENT Runtime
#  )

#install( FILES Code.cxx CMakeLists.txt
#  DESTINATION share/ITKExamples/Code/Core/Mesh/ConvertTriangleMeshToBinaryImage
#  COMPONENT Code
#  )

message (STATUS "MB_INSTALL_DEPENDENCY_DLLS=${MB_INSTALL_DEPENDENCY_DLLS}")

if (MB_INSTALL_DEPENDENCY_DLLS EQUAL 1)
	
	# --------------------------------
	# INSTALL VTK DLLS
	
	# get list of VTK files with paths
	set(VTK_DLLS_PATH "${VTK_ROOT_PATH}/bin")

	foreach (VTK_LIBRARY ${VTK_LIBRARIES})
		set(FILENAME_WITH_PATH "${VTK_DLLS_PATH}/${VTK_LIBRARY}-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}.dll")
	#	message(STATUS "File to copy: ${FILENAME_WITH_PATH}")
		list(APPEND VTK_DLLS_TO_INSTALL ${FILENAME_WITH_PATH})
	endforeach (VTK_LIBRARY)

	#message(STATUS "DLLs to copy: ${VTK_DLLS_TO_INSTALL}")
	# Determine where to copy
	set (VTK_DLLS_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
	message(STATUS "Installing VTK libraries to: ${VTK_DLLS_INSTALL_DIR}")
	file(INSTALL ${VTK_DLLS_TO_INSTALL} DESTINATION ${VTK_DLLS_INSTALL_DIR})	
	
	# --------------------------------
	# INSTALL ITK DLLS
	
	# get list of ITK files with paths
	set(ITK_DLLS_PATH "${ITK_ROOT_PATH}/bin")

	foreach (ITK_LIBRARY ${ITK_LIBRARIES})
		set(FILENAME_WITH_PATH "${ITK_DLLS_PATH}/${ITK_LIBRARY}-${ITK_MAJOR_VERSION}.${ITK_MINOR_VERSION}.dll")
	#	message(STATUS "File to copy: ${FILENAME_WITH_PATH}")
		list(APPEND ITK_DLLS_TO_INSTALL ${FILENAME_WITH_PATH})
	endforeach (ITK_LIBRARY)

	#message(STATUS "DLLs to copy: ${ITK_DLLS_TO_INSTALL}")
	# Determine where to copy
	set (ITK_DLLS_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
#	message(STATUS "Installing ITK libraries to: ${ITK_DLLS_INSTALL_DIR}")
#	file(INSTALL ${ITK_DLLS_TO_INSTALL} DESTINATION ${ITK_DLLS_INSTALL_DIR})	
	
else(MB_INSTALL_DEPENDENCY_DLLS EQUAL 1)
	message ("Not installing dependency dlls since user did not specify --install-dependency-dlls key")

endif (MB_INSTALL_DEPENDENCY_DLLS EQUAL 1)
