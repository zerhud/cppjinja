find_file(turtle turtle/mock.hpp)
get_filename_component(turtle "${turtle}" DIRECTORY)
get_filename_component(turtle "${turtle}" DIRECTORY)

find_package(Boost COMPONENTS unit_test_framework program_options REQUIRED)
add_executable(jparse tests/jparse.cpp tests/jparse/json_prov.cpp)
target_link_libraries(jparse PRIVATE
	evaluator loader parser2 absd ${Boost_LIBRARIES})
target_include_directories(jparse PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(jparse PRIVATE -DBOOST_JSON_STANDALONE)

# unit tests...
enable_testing()
macro(add_unit_test tname path lib)
	add_executable(${tname}_test
		"${path}${tname}_test.cpp" "tests/mocks.hpp")
	target_link_libraries(${tname}_test
		PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
	target_include_directories(${tname}_test
		SYSTEM PRIVATE
			"${Boost_INCLUDE_DIR}"
			"${turtle}"
			"${CMAKE_CURRENT_SOURCE_DIR}"
			"${CMAKE_CURRENT_SOURCE_DIR}/${path}"
			"${CMAKE_CURRENT_SOURCE_DIR}/tests"
			)
	# in new boost are lot of warnings in tests.. temporaty fix it
	target_compile_definitions(${tname}_test PRIVATE
		-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
	add_test(NAME ${tname} COMMAND "${tname}_test" --random)
	if(CODE_COVERAGE)
		target_code_coverage(${tname}_test AUTO ALL OBJECTS ${lib})
	endif()
endmacro()

set(tests_parser2 parser_helpers rules_expr
	rules_opterm rules_single rules_blocks rules_tmpls
	errors)
foreach(tname ${tests_parser2})
	add_unit_test(${tname} "tests/parser/" "parser2")
endforeach()

set(tests_loader load_common)
foreach(tname ${tests_loader})
	add_unit_test(${tname} "tests/loader/" "loader")
endforeach()

set(tests_eval
	eval_tree
	eval_op_out
	eval_tabs
	eval_completeness
	eval_exenv
	eval_expr
	eval_nodes
	eval_node_for
	eval_tmpl_compiler
	eval_builtins
	)
foreach(tname ${tests_eval})
	add_unit_test(${tname} "tests/eval/" "evaluator;parser2")
endforeach()
