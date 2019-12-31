cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(cppjinja VERSION 0.0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TODO "enable todo warnings in debug mode" ON)
option(ENABLE_TESTS "enable tests.. it depends on turtle library" ON)
option(LOW_MEMORY "assume we don't have efough memroty for complie grammars (gcc optimizations)" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost COMPONENTS unit_test_framework system REQUIRED)
# BOOST'S CMAKE BUG:
# find_package sets libraries to Boost::unit_test_framework;Boost::system and so on
# and cmake tries to find it in /nix/store/[include_hash]-boost-1.71.0-dev/lib/libboost_unit_test_framework.so.1.71.0 
# but they leaves in /nix/store/[library_hash]/lib... in nix env all libraries can be found
set(Boost_LIBRARIES "libboost_program_options.so;libboost_unit_test_framework.so;libboost_system.so")

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

#parser2
file(GLOB_RECURSE parser2_src LIST_DIRECTORIES false "parser/*.cpp")
file(GLOB_RECURSE parser2_hdr LIST_DIRECTORIES false "parser/*.hpp")
add_library(parser2 STATIC ${parser2_src} ${parser2_hdr})
target_include_directories(parser2 PRIVATE "${Boost_INCLUDE_DIRS}" PUBLIC "${config_files_dir}" "parser")
set_target_properties(parser2 PROPERTIES POSITION_INDEPENDENT_CODE ON)

#loader
file(GLOB_RECURSE loader_src LIST_DIRECTORIES false "loader/*.cpp")
file(GLOB_RECURSE loader_hdr LIST_DIRECTORIES false "loader/*.hpp")
add_library(loader SHARED ${loader_src} ${loader_hdr})
target_include_directories(loader PRIVATE "${Boost_INCLUDE_DIRS}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(loader PUBLIC stdc++fs PRIVATE parser2)

#evaluator
file(GLOB_RECURSE eval_src LIST_DIRECTORIES false "eval/*.cpp")
file(GLOB_RECURSE eval_hdr LIST_DIRECTORIES false "eval/*.hpp")
file(GLOB_RECURSE evaluator_src LIST_DIRECTORIES false "evtree/*.cpp")
file(GLOB_RECURSE evaluator_hdr LIST_DIRECTORIES false "evtree/*.hpp")
#file(GLOB_RECURSE evaluator_src LIST_DIRECTORIES false "evaluator/*.cpp")
#file(GLOB_RECURSE evaluator_hdr LIST_DIRECTORIES false "evaluator/*.hpp")
add_library(evaluator SHARED ${eval_src} ${eval_hdr} ${evaluator_src} ${evaluator_hdr})
target_include_directories(evaluator PRIVATE "${Boost_INCLUDE_DIRS}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(evaluator PUBLIC stdc++fs PRIVATE loader)

if(ENABLE_TESTS)
	include(cmake/tests.inc.cmake)
endif()

