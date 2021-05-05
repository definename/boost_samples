project(openproxy)

set(PROXY_SOURCES
	src/Client.cpp
	src/ClientMgr.cpp	
	src/Core.cpp
	src/Log.cpp
	src/Proxy.cpp
	src/ProxyMgr.cpp
	src/ProxyFactory.cpp
	src/Session.cpp
	src/Utils.cpp
	)
set(PROXY_HEADERS
	include/proxy/Client.h
	include/proxy/ClientMgr.h
	include/proxy/Core.h
	include/proxy/Log.h
	include/proxy/openproxy.h
	include/proxy/Proxy.h
	include/proxy/ProxyMgr.h
	include/proxy/ProxyFactory.h
	include/proxy/Session.h
	include/proxy/Utils.h
	)
set(PROXY_INTERFACE
	include/proxy/IClient.h
	include/proxy/IClientMgr.h
	include/proxy/IProxy.h
	include/proxy/IProxyFactory.h
	include/proxy/IProxyMgr.h
	include/proxy/ISession.h
	include/proxy/Types.h
	)
set(PROXY_PROTOCOL_SOURCES
	src/Event.cpp
	src/Greeting.cpp
	src/Login.cpp
	)
set(PROXY_PROTOCOL_HEADERS
	include/proxy/Event.h
	include/proxy/Greeting.h
	include/proxy/Login.h
	include/proxy/Protocol.h
	)
set(PROJECT_SOURCES
	src/main.cpp
	)
set(PROJECT_HEADERS
	)
set(MISC
	src/pch.h
	src/pch.cpp
	)
	
source_group("Source Files" FILES ${PROJECT_SOURCES})
source_group("Header Files" FILES ${PROJECT_HEADERS})
source_group("Source Files\\proxy" FILES ${PROXY_SOURCES})
source_group("Header Files\\proxy" FILES ${PROXY_HEADERS})
source_group("Header Files\\proxy\\interface" FILES ${PROXY_INTERFACE})
source_group("Source Files\\proxy\\protocol" FILES ${PROXY_PROTOCOL_SOURCES})
source_group("Header Files\\proxy\\protocol" FILES ${PROXY_PROTOCOL_HEADERS})
source_group("" FILES ${MISC})
	
add_executable(${PROJECT_NAME}
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
	${PROXY_SOURCES}
	${PROXY_HEADERS}
	${PROXY_INTERFACE}
	${PROXY_PROTOCOL_HEADERS}
	${PROXY_PROTOCOL_SOURCES}
	${MISC}
	)
	
add_precompiled_headers(${PROJECT_NAME} "src/")

include_directories(
	include
	${Boost_INCLUDE_DIRS}
	)
target_link_libraries(${PROJECT_NAME}
	${Boost_LIBRARIES}
	)
	
add_definitions(
	-DUNICODE
	-D_UNICODE
	-DBOOST_ALL_NO_LIB
	)
	
#add_dependencies(${PROJECT_NAME}
#	)
	
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "_asio")
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)

