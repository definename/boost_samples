project(opennattest)

set(PROJECT_SOURCES
	src/main.cpp
	)
set(PROJECT_HEADERS
	)
set(MISC
	src/pch.h
	src/pch.cpp
	)
	
set(PROTOFILE_PATH ${CMAKE_CURRENT_LIST_DIR}/protofiles)
set(PROTOFILES
	${PROTOFILE_PATH}/Event.proto
	)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOFILES})
set(TEST_OPENNAT_PROTOBUF_SOURCES
	${PROTO_SRCS}
	${PROTO_HDRS}
	)
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES COMPILE_FLAGS "/Y-")

source_group("Source Files" FILES ${PROJECT_SOURCES})
source_group("Header Files" FILES ${PROJECT_HEADERS})
source_group("Protobuf" FILES ${TEST_OPENNAT_PROTOBUF_SOURCES})
source_group("" FILES ${MISC})

add_executable(${PROJECT_NAME}
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
	${MISC}
	${TEST_OPENNAT_PROTOBUF_SOURCES}
	)

add_precompiled_headers(${PROJECT_NAME} "src/")

include_directories(
	include
	${CMAKE_CURRENT_BINARY_DIR}
	${Boost_INCLUDE_DIRS}
	${OPENNAT_INCLUDES}
	${ZLIB_INCLUDE_DIR}
	${Protobuf_INCLUDE_DIRS}
	)
target_link_libraries(${PROJECT_NAME}
	${Boost_LIBRARIES}
	${OPENNAT_LIBRARIES}
	${ZLIB_LIBRARIES}
	${Protobuf_LIBRARIES}
	ws2_32.lib
	iphlpapi.lib
	)

add_definitions(
	-DUNICODE
	-D_UNICODE
	-DBOOST_ALL_NO_LIB
	)

add_dependencies(${PROJECT_NAME}
	zlib
	protobuf
	opennat
	)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "_asio")
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)

