project(async-client-ssl)

set(PROJECT_HEADERS
	include/targetver.h
	)
set(PROJECT_SOURCES
	src/main.cpp
	)
set(MISC
	src/pch.h
	src/pch.cpp
	)

source_group ("Header Files" FILES ${PROJECT_HEADERS})
source_group ("Source Files" FILES ${PROJECT_SOURCES})
source_group ("" FILES ${MISC})

add_executable(${PROJECT_NAME}
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
	${MISC}
	)

include_directories(
	include
	${Boost_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIR}
	)
target_link_libraries(${PROJECT_NAME}
	${Boost_LIBRARIES}
	${OPENSSL_LIBRARIES}
	)
add_definitions(
	-DUNICODE
	-D_UNICODE
	-DBOOST_ALL_NO_LIB
	)
#add_dependencies(${PROJECT_NAME}
#	)

add_precompiled_headers(${PROJECT_NAME} "src/")
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "_asio/simple")
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)