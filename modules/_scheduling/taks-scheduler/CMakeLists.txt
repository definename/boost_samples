project(taks-scheduler)

set(PROJECT_HEADERS
	include/App.h
	include/Log.h
	include/Scheduler.h
	include/Task.h
	)
set(PROJECT_SOURCES
	src/main.cpp
	src/Log.cpp
	)
set(MISC
	src/pch.h
	src/pch.cpp
	)

source_group("Header Files" FILES ${PROJECT_HEADERS})
source_group("Source Files" FILES ${PROJECT_SOURCES})
source_group("" FILES ${MISC})

add_executable(${PROJECT_NAME}
	${PROJECT_HEADERS}
	${PROJECT_SOURCES}
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
	)

add_precompiled_headers(${PROJECT_NAME} "src/")
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "_scheduling")
# set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)