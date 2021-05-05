project(singleton)

include(PCHSupport)
include(CRTLinkage)

set(PROJECT_SOURCES
	src/main.cpp
	src/Kernel.cpp
	src/Emulator.cpp
	)
set(PROJECT_HEADERS
	include/IKernel.h
	include/Kernel.h
	include/Emulator.h
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

