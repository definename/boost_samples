project(boost_log)

include(PCHSupport)
include(CRTLinkage)

set(PROJECT_SOURCES
	src/Log.cpp
	src/main.cpp
	)
set(PROJECT_HEADERS
	include/Log.h
	)
set(MISC
	src/pch.h
	src/pch.cpp
	src/targetver.h
	)

source_group ("Source Files" FILES ${PROJECT_SOURCES})
source_group ("Header Files" FILES ${PROJECT_HEADERS})
source_group ("" FILES ${MISC})

add_definitions(
	-DUNICODE
	-D_UNICODE
	)

add_executable(${PROJECT_NAME}
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
	${MISC}
	)
include_directories(include
	${Boost_INCLUDE_DIRS}
	)
target_link_libraries(${PROJECT_NAME}
	${Boost_LIBRARIES}
	)

add_precompiled_headers(${PROJECT_NAME} "src/")

SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "_log")
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)

