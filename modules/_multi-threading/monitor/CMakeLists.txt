project(monitor)

set(PROJECT_SOURCES
	src/main.cpp
	src/Monitor.cpp
	)
set(PROJECT_HEADERS
	include/Monitor.h
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

include_directories(
	include
	${Boost_INCLUDE_DIRS}
	)
target_link_libraries(${PROJECT_NAME}
	${Boost_LIBRARIES}
	)
add_definitions(
#	-DUNICODE
#	-D_UNICODE
	)

add_precompiled_headers(${PROJECT_NAME} "src/")

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "_multi-threading")
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)