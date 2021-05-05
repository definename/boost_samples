project(serialization)

set(PROJECT_SOURCES
	src/main.cpp
	)
set(PROJECT_HEADERS
	include/Character.h
	include/DarthVader.h
	include/LukeSkywalker.h
	include/Test.h
	)
set(MISC
	src/pch.h
	src/pch.cpp
	)

source_group ("Source Files" FILES ${PROJECT_SOURCES})
source_group ("Header Files" FILES ${PROJECT_HEADERS})
source_group ("" FILES ${MISC})

add_executable(${PROJECT_NAME}
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
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

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "_serialization")
#set_target_properties(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)

