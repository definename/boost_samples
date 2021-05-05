cmake_minimum_required(VERSION 3.2.1)

project(netbios_scanner)

include(PCHSupport)
include(CRTLinkage)

set(PROJECT_SOURCES
	src/main.cpp
	src/NetBIOSScanner.cpp
	)
set(PROJECT_HEADERS
	include/NetBIOSScanner.h
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
	
target_link_libraries(${PROJECT_NAME}
	Netapi32.lib
	Iphlpapi.lib
	)

#SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "patterns")
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)

