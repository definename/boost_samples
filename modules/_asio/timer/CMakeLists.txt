project(asio_timer)

set(SOURCE_FILES src/main.cpp)
set(HEADER_FILES src/project_header.h)

source_group("Source files" FILES ${SOURCE_FILES})
source_group("Header files" FILES ${HEADER_FILES})

add_executable(${PROJECT_NAME}
	${SOURCE_FILES}
	${HEADER_FILES}
	)
target_precompile_headers(${PROJECT_NAME}
	PRIVATE
		src/project_header.h
		)
target_include_directories(${PROJECT_NAME}
	PRIVATE
		${Boost_INCLUDE_DIRS}
		)
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		${Boost_LIBRARIES}
		)