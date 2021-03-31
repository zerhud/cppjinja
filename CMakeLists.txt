cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(cppjinja VERSION 0.0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET( CMAKE_CXX_EXTENSIONS "OFF" )

option(ENABLE_TODO "enable todo warnings in debug mode" ON)
option(ENABLE_TESTS "enable tests.. it depends on turtle library" ON)
option(LOW_MEMORY "assume we don't have efough memroty for complie grammars (gcc optimizations)" ON)
option(CODE_COVERAGE "enable code coverage generation" OFF)
option(USE_LIBCXX_DEBUG "enable glibcxx debug" OFF)

if(USE_LIBCXX_DEBUG AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	message("=== use debug version of libcxx")
	add_compile_definitions(_GLIBCXX_DEBUG _GLIBCXX_DEBUG_PEDANTIC)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost COMPONENTS unit_test_framework system REQUIRED)
# BOOST'S CMAKE BUG:
# find_package sets libraries to Boost::unit_test_framework;Boost::system and so on
# and cmake tries to find it in /nix/store/[include_hash]-boost-1.71.0-dev/lib/libboost_unit_test_framework.so.1.71.0 
# but they leaves in /nix/store/[library_hash]/lib... in nix env all libraries can be found
#set(Boost_LIBRARIES "libboost_program_options.so;libboost_unit_test_framework.so;libboost_system.so")

find_package(Threads REQUIRED)
add_subdirectory(absd)
include(CheckCXXSourceCompiles)
include(CMakePackageConfigHelpers)
include(cmake/target_sources_local.cmake)
include(cmake/flags.cmake)

if(CODE_COVERAGE)
	include(cmake/code-coverage.cmake)
	add_code_coverage()
	add_code_coverage_all_targets(EXCLUDE tests/*)
endif()


CHECK_CXX_SOURCE_COMPILES("#include <filesystem>\nint main(int,char**){return 0;}" HAVE_FILESYSTEM)
if(NOT HAVE_FILESYSTEM)
	message(WARNING "we don't have std::filesystem.. try to use expiremental.\nthis mode is supported only for clang, for build coverage report")
endif()

set(config_files_dir "${CMAKE_CURRENT_BINARY_DIR}/cfg")
set(main_config_file "${config_files_dir}/config.hpp")
configure_file("config.hpp.in" "${main_config_file}")
set(settings_dir ".")

set(inst_incdir "include/${PROJECT_NAME}/")
set(inst_rtdir "lib/${PROJECT_NAME}/")


# lohman json
set(lohman_json_file "${CMAKE_CURRENT_BINARY_DIR}/nlohman/cppjinja/json.hpp")
set(lohman_json_url "https://github.com/nlohmann/json/raw/develop/single_include/nlohmann/json.hpp")
add_custom_command(OUTPUT "${lohman_json_file}"
	COMMAND "${CMAKE_COMMAND}"
		-Durl="${lohman_json_url}" -Dout="${lohman_json_file}"
		-Denv_var="nlohman_json_header"
		-P "${CMAKE_CURRENT_LIST_DIR}/cmake/download_or_get.cmake"
	DEPENDS
		"${CMAKE_CURRENT_LIST_DIR}/cmake/download_or_get.cmake"
)

#parser2
file(GLOB_RECURSE parser2_src LIST_DIRECTORIES false "parser/*.cpp")
file(GLOB_RECURSE parser2_hdr LIST_DIRECTORIES false "parser/*.hpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${parser2_src} ${parser2_hdr})
add_library(parser2 STATIC ${parser2_src} ${parser2_hdr})
target_include_directories(parser2
	PRIVATE "${Boost_INCLUDE_DIRS}"
	PUBLIC
		"$<BUILD_INTERFACE:${config_files_dir}>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/parser>"
		"$<INSTALL_INTERFACE:${inst_incdir}>"
	)
set_target_properties(parser2 PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(parser2 PRIVATE -DPARSER_PRIVATE)
install(DIRECTORY parser DESTINATION "${inst_incdir}" FILES_MATCHING PATTERN "*.hpp")

#loader
file(GLOB_RECURSE loader_src LIST_DIRECTORIES false "loader/*.cpp")
file(GLOB_RECURSE loader_hdr LIST_DIRECTORIES false "loader/*.hpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${loader_src} ${loader_hdr})
add_library(loader SHARED ${loader_src} ${loader_hdr})
target_include_directories(loader PRIVATE "${Boost_INCLUDE_DIRS}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(loader PUBLIC stdc++fs PRIVATE parser2)
install(DIRECTORY loader DESTINATION "${inst_incdir}" FILES_MATCHING PATTERN "*.hpp")

#evaluator
file(GLOB_RECURSE eval_src LIST_DIRECTORIES false "eval/*.cpp")
file(GLOB_RECURSE eval_hdr LIST_DIRECTORIES false "eval/*.hpp")
file(GLOB_RECURSE evaluator_src LIST_DIRECTORIES false "evtree/*.cpp")
file(GLOB_RECURSE evaluator_hdr LIST_DIRECTORIES false "evtree/*.hpp")
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${eval_src} ${eval_hdr} ${evaluator_src} ${evaluator_hdr})
list(APPEND eval_headers ${lohman_json_file})
list(APPEND eval_headers ${evaluator_hdr})
list(APPEND eval_headers ${eval_hdr})
add_library(evaluator SHARED ${eval_src} ${evaluator_src} ${eval_headers})
set_target_properties(evaluator PROPERTIES PUBLIC_HEADER "${lohman_json_file}")
target_include_directories(evaluator
        PRIVATE
                "${Boost_INCLUDE_DIRS}"
                "${CMAKE_CURRENT_SOURCE_DIR}"
                "${CMAKE_CURRENT_SOURCE_DIR}/evtree"
		"${CMAKE_CURRENT_SOURCE_DIR}/parser"
        SYSTEM PUBLIC
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/nlohman>"
        )
target_link_libraries(evaluator PUBLIC absd stdc++fs PRIVATE loader)
install(DIRECTORY eval DESTINATION "${inst_incdir}" FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY evtree DESTINATION "${inst_incdir}" FILES_MATCHING PATTERN "*.hpp")

if(ENABLE_TESTS)
	include(cmake/tests.inc.cmake)
	if(CODE_COVERAGE)
		target_code_coverage(evaluator)
	endif()
endif()

set(config_file_dir "${CMAKE_CURRENT_BINARY_DIR}/configs/")
configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in"
	"${config_file_dir}/${PROJECT_NAME}-config.cmake"
	INSTALL_DESTINATION "${inst_rtdir}"
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
	"${config_file_dir}/${PROJECT_NAME}-config-version.cmake"
	COMPATIBILITY AnyNewerVersion)
install(
	TARGETS parser2 loader evaluator
	EXPORT "${PROJECT_NAME}-export"
	ARCHIVE DESTINATION "${inst_rtdir}" COMPONENT bin
	LIBRARY DESTINATION "${inst_rtdir}" COMPONENT dev
	INCLUDES DESTINATION "${inst_incdir}/../" "${inst_incdir}/"
	PUBLIC_HEADER COMPONENT dev   DESTINATION "${inst_incdir}")
install(
	EXPORT ${PROJECT_NAME}-export
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION "${inst_rtdir}"
	COMPONENT dev)
install(
	FILES
		"${config_file_dir}/${PROJECT_NAME}-config.cmake"
		"${config_file_dir}/${PROJECT_NAME}-config-version.cmake"
	COMPONENT dev
	DESTINATION "${inst_rtdir}")

