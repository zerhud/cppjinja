cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project(absd VERSION 0.0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_TESTS "enable tests.. it depends on turtle (and boost) library" ON)

if(USE_LIBCXX_DEBUG AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	message("=== use debug version of libcxx")
	add_compile_definitions(_GLIBCXX_DEBUG _GLIBCXX_DEBUG_PEDANTIC)
endif()

set(inst_incdir "include/${PROJECT_NAME}/")
set(inst_rtdir "lib/${PROJECT_NAME}/")
file(GLOB_RECURSE src_list LIST_DIRECTORIES false "absd/*.cpp")
file(GLOB_RECURSE hdr_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} LIST_DIRECTORIES false "absd/*.hpp")
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${src_list} ${hdr_list})
add_library(${PROJECT_NAME} STATIC ${src_list} ${hdr_list})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${hdr_list}")
target_include_directories(${PROJECT_NAME}
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<INSTALL_INTERFACE:${inst_incdir}>"
	)

install(TARGETS ${PROJECT_NAME} 
	EXPORT "${PROJECT_NAME}-export"
	ARCHIVE DESTINATION "${inst_rtdir}" COMPONENT bin
	INCLUDES DESTINATION "${inst_incdir}/../" "${inst_incdir}/"
	PUBLIC_HEADER COMPONENT dev   DESTINATION "${inst_incdir}")
install(EXPORT ${PROJECT_NAME}-export
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION "${inst_rtdir}"
	COMPONENT dev)

if(ENABLE_TESTS)
	enable_testing()
	find_file(turtle turtle/mock.hpp)
	get_filename_component(turtle "${turtle}" DIRECTORY)
	get_filename_component(turtle "${turtle}" DIRECTORY)
	find_package(Boost COMPONENTS unit_test_framework REQUIRED)
	macro(add_unit_test tname path lib)
		add_executable(${tname}_test "${path}${tname}_test.cpp" "${path}mocks.hpp")
		target_link_libraries(${tname}_test PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
		target_include_directories(${tname}_test SYSTEM PRIVATE
			"${Boost_INCLUDE_DIR}" "${turtle}"
			"${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
		# in new boost are lot of warnings in tests.. temporaty fix it
		target_compile_definitions(${tname}_test PRIVATE -DBOOST_BIND_GLOBAL_PLACEHOLDERS)
		add_test(NAME ${tname} COMMAND "${tname}_test" --random)
	endmacro()

	add_unit_test(absd "tests/" "${PROJECT_NAME}")
	add_unit_test(to_json "tests/" "${PROJECT_NAME}")
	add_unit_test(simple_data_holder "tests/" "${PROJECT_NAME}")
endif()
