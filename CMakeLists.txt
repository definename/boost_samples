cmake_minimum_required(VERSION 3.19)

set(BUILD_WITH_STATIC_RUNTIME_LIBRARY ON)
if(WIN32)
	set(BUILD_WITH_STATIC_RUNTIME_LIBRARY ${BUILD_WITH_MSVC_STATIC_RUNTIME_LIBRARY})
	if(${BUILD_WITH_STATIC_RUNTIME_LIBRARY})
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	else()
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	endif()
endif()

project(boost_samples)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(BUILD_WITH_STATIC_CRT "Use static run-time libraries (/MT or /MTd linker flags)" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries (.dll) instead of static ones (.lib)" OFF)

option(BUILD_WITH_OPENSSL "Build project with openssl support" OFF)
option(BUILD_WITH_MINIUPNPC "Build project with miniupnpc support" OFF)
option(BUILD_WITH_ZLIB "Build project with zlib support" OFF)
option(BUILD_WITH_PROTOBUF "Build project with protobuf support" OFF)
option(BUILD_WITH_WTL "Build project with WTL support" OFF)

# Sets executable output folder
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
# Assets directory path
set(ASSETS_PATH ${CMAKE_SOURCE_DIR}/assets)

include(ExternalProject)
# External directory path
set(EXTERNAL_PATH ${CMAKE_SOURCE_DIR}/external)
# Patch directory path
set(PATCH_PATH ${EXTERNAL_PATH}/patches)
# External install directory path
set(EXTERNAL_INSTALL_PATH ${CMAKE_BINARY_DIR}/external)

set(CMAKE_CXX_STANDARD 14)
if(WIN32)
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm400")
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif()

###########################################################
# Boost

set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ${BUILD_WITH_STATIC_RUNTIME_LIBRARY})
find_package(Boost
	1.72.0
	REQUIRED
	COMPONENTS
		filesystem
		system
		thread
		program_options
		serialization
		# log
	)

###########################################################
# OpenSSL #################################################

if (BUILD_WITH_OPENSSL)
	set(OPENSSL_USE_STATIC_LIBS ON)
	set(OPENSSL_MSVC_STATIC_RT ON)
	set(OPENSSL_ROOT_DIR ${EXTERNAL_PATH}/openssl-1.0.2)

	if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
		set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR}/vc14-x64)
	else()
		set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR}/vc14-x86)
	endif()

	find_package(OpenSSL REQUIRED)
endif()

###########################################################
# MINIUPNPC ###############################################

if(BUILD_WITH_MINIUPNPC)
	set(MINIUPNPC_ROOT ${EXTERNAL_INSTALL_PATH}/MINIUPNPC)
	ExternalProject_Add(
		miniupnpc
		URL "${EXTERNAL_PATH}/miniupnpc-2.0.zip"
		URL_MD5 CAF4D6F3EBA696F7894C0C255F48CB5A
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${MINIUPNPC_ROOT}
			-DBUILD_WITH_STATIC_CRT:BOOL=${BUILD_WITH_STATIC_CRT}
			-DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
			-DUPNPC_BUILD_STATIC:BOOL=TRUE
			-DUPNPC_BUILD_SHARED:BOOL=FALSE
		PATCH_COMMAND 
			${CMAKE_COMMAND} -P ${PATCH_PATH}/miniupnpc-2.0.patches/patch.cmake ${CMAKE_SOURCE_DIR}
	)
	set(MINIUPNPC_INCLUDE_DIR ${MINIUPNPC_ROOT}/include)
	set(MINIUPNPC_LIBRARIES ${MINIUPNPC_ROOT}/lib/miniupnpc.lib )

	SET_PROPERTY(TARGET miniupnpc PROPERTY FOLDER "External")
endif(BUILD_WITH_MINIUPNPC)

###########################################################
# ZLIB ####################################################

if(BUILD_WITH_ZLIB)
	set(ZLIB_ROOT ${EXTERNAL_INSTALL_PATH}/ZLIB)
	ExternalProject_Add(
		zlib
		URL "${EXTERNAL_PATH}/zlib-1.2.8.zip"
		URL_MD5 D34BAA0DA16E33A5EF095E7EEF7D2472
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${ZLIB_ROOT}
			-DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
		PATCH_COMMAND 
			${CMAKE_COMMAND} -P ${PATCH_PATH}/zlib-1.2.8.patches/patch.cmake ${CMAKE_SOURCE_DIR}
		)
	set(ZLIB_INCLUDE_DIR ${ZLIB_ROOT}/include)
	set(ZLIB_LIBRARIES
		debug ${ZLIB_ROOT}/lib/zlibstaticd.lib
		optimized ${ZLIB_ROOT}/lib/zlibstatic.lib
		)

	SET_PROPERTY(TARGET zlib PROPERTY FOLDER "External")
endif(BUILD_WITH_ZLIB)

############################################################
# PROTOBUF #################################################

if(BUILD_WITH_PROTOBUF)
	set(PROTOBUF_ROOT ${EXTERNAL_INSTALL_PATH}/PROTOBUF)
	ExternalProject_Add(
		protobuf
		DEPENDS
			zlib
		URL "${EXTERNAL_PATH}/protobuf-cpp-3.1.0.zip"
		URL_MD5 3325958B606A6AF351E2BFBF4250EC1C
		CMAKE_ARGS
			-DCMAKE_INSTALL_PREFIX:PATH=${PROTOBUF_ROOT}
			-DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
			-DZLIB_ROOT:PATH=${ZLIB_ROOT}
		SOURCE_SUBDIR cmake
		PATCH_COMMAND 
			${CMAKE_COMMAND} -P ${PATCH_PATH}/protobuf-cpp-3.1.0.patches/patch.cmake ${CMAKE_SOURCE_DIR}
		)
	set(Protobuf_INCLUDE_DIR ${PROTOBUF_ROOT}/include)
	set(Protobuf_PROTOC_EXECUTABLE ${PROTOBUF_ROOT}/bin/protoc.exe)
	set(Protobuf_LIBRARY
		debug ${PROTOBUF_ROOT}/lib/libprotobufd.lib
		optimized ${PROTOBUF_ROOT}/lib/libprotobuf.lib)
	set(Protobuf_PROTOC_LIBRARY 
		debug ${PROTOBUF_ROOT}/lib/libprotocd.lib
		optimized ${PROTOBUF_ROOT}/lib/libprotoc.lib)
	set(Protobuf_LITE_LIBRARY
		debug ${PROTOBUF_ROOT}/lib/libprotobuf-lited.lib
		optimized ${PROTOBUF_ROOT}/lib/libprotobuf-lite.lib)

	find_package(protobuf REQUIRED)

	SET_PROPERTY(TARGET protobuf PROPERTY FOLDER "External")
endif(BUILD_WITH_PROTOBUF)

###########################################################
# WTL #####################################################

set(WTL_ROOT ${EXTERNAL_PATH}/wtl91)
if(BUILD_WITH_WTL)
	find_package(WTL REQUIRED)
endif(BUILD_WITH_WTL)

add_subdirectory(modules/_asio)
# add_subdirectory(modules/_algorithm)
# add_subdirectory(modules/_beast)
# add_subdirectory(modules/_variant)
# add_subdirectory(modules/_program_options)
# add_subdirectory(modules/_conversion)
# add_subdirectory(modules/_containers)
# add_subdirectory(modules/_filesystem)
# add_subdirectory(modules/_proxy_printer)
# add_subdirectory(modules/_spirit)
# add_subdirectory(modules/_serialization)
# add_subdirectory(modules/_log)
# add_subdirectory(modules/_multi-threading)
# add_subdirectory(modules/_interprocess)
# add_subdirectory(modules/_scheduling)
# add_subdirectory(modules/_optional)
# add_subdirectory(modules/_uuid)

# add_subdirectory(modules/bind)
# add_subdirectory(modules/date_time)
# add_subdirectory(modules/icmp_scanner)
# add_subdirectory(modules/netbios_scanner)
# add_subdirectory(modules/property_tree)
# add_subdirectory(modules/tribool)
# add_subdirectory(modules/assign)
# add_subdirectory(modules/process)
# add_subdirectory(modules/agent_uuid)

add_custom_target(genexdebug ALL COMMAND ${CMAKE_COMMAND} -E echo "$<IF:$<CONFIG:Debug>,buildDebug,buildRelease>")