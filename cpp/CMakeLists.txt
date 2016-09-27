cmake_minimum_required(VERSION 2.8)

find_package(OpenCV 3.1 EXACT REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

project(EBookChecker)

macro(REPLACE_COMPILER_FLAGS)
	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
		)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
endmacro(REPLACE_COMPILER_FLAGS)


set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../data)
file(GLOB IMAGE_DATAS "${TEST_DATA_DIR}/*.jpg")
message(STATUS "image datas: ${IMAGE_DATAS}")
list(LENGTH IMAGE_DATAS NUM_OF_IMAGES)
message(STATUS "num of images in \"${TEST_DATA_DIR}\": ${NUM_OF_IMAGES}")
if( ${NUM_OF_IMAGES} LESS "1" )
	file(MAKE_DIRECTORY ${TEST_DATA_DIR})
	message(FATAL_ERROR "not found image file in data/. please copy one image data scaned novel page")
endif()

list(GET IMAGE_DATAS 0 TEST_DATA_0)
message(STATUS "use this file as test data :\"${TEST_DATA_0}\"")

set(TEST_DATA_SOURCE_NAME testdatapath)
set(TEST_DATA_PATH_SOURCE ${TEST_DATA_SOURCE_NAME}.h.in)
set(TEST_DATA_PATH_OUTPUT ${TEST_DATA_SOURCE_NAME}.h)
set(INCLUDE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
configure_file(${TEST_DATA_PATH_SOURCE} ${INCLUDE_OUTPUT_DIR}/${TEST_DATA_PATH_OUTPUT} @ONLY)

add_definitions(${Qt5Widgets_DEFINITIONS})

include_directories(${OpenCV_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${INCLUDE_OUTPUT_DIR})
link_directories(${OpenCV_LIBRARY_DIRS})

## setting for ebookChecker AT
file(GLOB AT_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_executable(ebookChecker_AT ${AT_SOURCE_FILES} ${TEST_DATA_PATH_OUTPUT})
target_link_libraries(ebookChecker_AT ${OpenCV_LIBRARIES} Qt5::Widgets Qt5::Core)


REPLACE_COMPILER_FLAGS()
