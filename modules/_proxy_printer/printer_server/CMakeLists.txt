cmake_minimum_required(VERSION 3.2.1)

project(printer_server)

include(PCHSupport)
include(CRTLinkage)

set(PROJECT_SOURCES
	src/main.cpp
	src/ProxyServer.cpp
	src/ProxySession.cpp
	)
set(PROJECT_HEADERS
	include/ProxyServer.h
	include/ProxySession.h
	)
	
set(MISC
	src/pch.h
	src/pch.cpp
	)
	
add_executable(${PROJECT_NAME}
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
	${MISC}
	)
	
add_precompiled_headers(${PROJECT_NAME} "src/")

include_directories(include
	)
	
add_definitions(
	-DUNICODE
	-D_UNICODE
	)
	
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "proxy_printer")
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)

