cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(cppjinja VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)
option(ENABLE_TESTS "enable tests.. it depends on turtle library" ON)
option(LOW_MEMORY "assume we don't have efough memroty for complie grammars (gcc optimizations)" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#find_package(Boost COMPONENTS program_options unit_test_framework system REQUIRED)
find_package(Boost COMPONENTS unit_test_framework system REQUIRED)
find_package(Threads REQUIRED)
include(CheckCXXSourceCompiles)
include(cmake/target_sources_local.cmake)
include(cmake/flags.cmake)

CHECK_CXX_SOURCE_COMPILES("#include <filesystem>\nint main(int,char**){return 0;}" HAVE_FILESYSTEM)
if(NOT HAVE_FILESYSTEM)
	message(WARNING "we don't have std::filesystem.. try to use expiremental.\nthis mode is supported only for clang, for build coverage report")
endif()

set(config_files_dir "${CMAKE_CURRENT_BINARY_DIR}/cfg")
set(main_config_file "${config_files_dir}/config.hpp")
configure_file("config.hpp.in" "${main_config_file}")
set(settings_dir ".")


# lohman json
set(lohman_json_file "${CMAKE_CURRENT_BINARY_DIR}/nlohman/json.hpp")
set(lohman_json_url "https://github.com/nlohmann/json/raw/develop/single_include/nlohmann/json.hpp")
add_custom_command(OUTPUT "${lohman_json_file}"
	COMMAND "${CMAKE_COMMAND}"
		-Durl="${lohman_json_url}" -Dout="${lohman_json_file}"
		-Denv_var="nlohman_json_header"
		-P "${CMAKE_CURRENT_LIST_DIR}/cmake/download_or_get.cmake"
	DEPENDS
		"${CMAKE_CURRENT_LIST_DIR}/cmake/download_or_get.cmake"
)

# parser
add_library(parser SHARED "${main_config_file}" parser/grammar.inl parser/traits.hpp)
target_include_directories(parser PRIVATE "${Boost_INCLUDE_DIRS}" PUBLIC "${config_files_dir}")
target_sources_local(parser "grammar.hpp" "grammar.cpp" "parser/")
target_sources_local(parser "helpers.hpp" "helpers.cpp" "parser/")
target_sources_local(parser "ast.hpp" "ast.cpp" "parser/")

# some files may slow down system when compiling..
if(LOW_MEMORY AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set_source_files_properties("parser/grammar.cpp" PROPERTIES COMPILE_FLAGS "--param ggc-min-expand=1024 --param ggc-min-heapsize=4096")
endif()

if(ENABLE_TESTS)
	include(cmake/tests.inc.cmake)
endif()

