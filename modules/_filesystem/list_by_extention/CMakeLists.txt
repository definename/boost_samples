project(list_by_extention)

set(PROJECT_SOURCES
	src/main.cpp
	)
set(PROJECT_HEADERS
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

SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROPERTY_FOLDER})
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)
add_precompiled_headers(${PROJECT_NAME} "src/")