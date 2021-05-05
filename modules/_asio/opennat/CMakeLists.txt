project(opennat)

set(NAT_SOURCES
	src/Client.cpp
	src/Core.cpp
	src/ProxyMgr.cpp
	src/Server.cpp
	src/Session.cpp
	src/SessionMgr.cpp
	src/SubscribeMgr.cpp
	src/opennat.cpp
	src/Kernel.cpp
	src/Log.cpp
	src/Message.cpp
	src/Utils.cpp
	src/UPNPCore.cpp
	src/UPNPClient.cpp
	)
set(NAT_HEADERS
	include/nat/Client.h
	include/nat/Core.h
	include/nat/ProxyMgr.h
	include/nat/Errors.h
	include/nat/Server.h
	include/nat/Session.h
	include/nat/SessionMgr.h
	include/nat/SubscribeMgr.h
	include/nat/Kernel.h
	include/nat/opennat.h
	include/nat/Log.h
	include/nat/Types.h
	include/nat/Message.h
	include/nat/Utils.h
	include/nat/UPNPCore.h
	include/nat/UPNPClient.h
	)
set(NAT_INTERFACE
	include/nat/IClient.h
	include/nat/IProxyMgr.h
	include/nat/IServer.h
	include/nat/ISession.h
	include/nat/ISessionMgr.h
	include/nat/ISubscribeMgr.h
	include/nat/IKernel.h
	)
set(MISC
	src/pch.h
	src/pch.cpp
	)

set(PROTOFILE_PATH ${CMAKE_CURRENT_LIST_DIR}/protofiles)
set(PROTOFILES
	${PROTOFILE_PATH}/Command.proto
	)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOFILES})
set(OPENNAT_PROTOBUF_SOURCES
	${PROTO_SRCS}
	${PROTO_HDRS}
	)
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES COMPILE_FLAGS "/Y-")

source_group("Source Files\\nat" FILES ${NAT_SOURCES})
source_group("Header Files\\nat" FILES ${NAT_HEADERS})
source_group("Header Files\\nat\\interface" FILES ${NAT_INTERFACE})
source_group("Protobuf" FILES ${OPENNAT_PROTOBUF_SOURCES})
source_group("" FILES ${MISC})

add_library(${PROJECT_NAME} STATIC
	${MISC}
	${OPENNAT_PROTOBUF_SOURCES}
	${NAT_SOURCES}
	${NAT_HEADERS}
	${NAT_INTERFACE}
	)

add_precompiled_headers(${PROJECT_NAME} "src/")

include_directories(
	include
	${CMAKE_CURRENT_BINARY_DIR}
	${ZLIB_INCLUDE_DIR}
	${Protobuf_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${MINIUPNPC_INCLUDE_DIR}
	)
target_link_libraries(${PROJECT_NAME}
	${ZLIB_LIBRARIES}
	${Protobuf_LIBRARIES}
	${Boost_LIBRARIES}
	${MINIUPNPC_LIBRARIES}
	ws2_32.lib
	iphlpapi.lib
	)

add_definitions(
	-DUNICODE
	-D_UNICODE
	-DBOOST_ALL_NO_LIB
	-DMINIUPNP_STATICLIB
	)

add_dependencies(${PROJECT_NAME}
	zlib
	protobuf
	miniupnpc
	)

#set(OPENNAT_INCLUDES ${CMAKE_SOURCE_DIR}/modules/opennat/include PARENT_SCOPE)
set(OPENNAT_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/include PARENT_SCOPE)
set(OPENNAT_LIBRARIES ${PROJECT_NAME} PARENT_SCOPE)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "_asio")