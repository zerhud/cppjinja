find_file(turtle turtle/mock.hpp)
get_filename_component(turtle "${turtle}" DIRECTORY)
get_filename_component(turtle "${turtle}" DIRECTORY)

# unit tests...
enable_testing()
macro(add_unit_test tname path lib)
	add_executable(${tname}_test "${path}${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
	target_include_directories(${tname}_test SYSTEM PRIVATE "${Boost_INCLUDE_DIR}" "${turtle}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
	add_test(NAME ${tname} COMMAND "${tname}_test" --random)
	target_code_coverage(${tname}_test AUTO ALL OBJECTS ${lib})
endmacro()

set(tests_parser2 parser_helpers rules_common rules_opterm rules_single rules_blocks rules_tmpls)
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
	eval_nodes
	eval_tmpl_compiler
	)
foreach(tname ${tests_eval})
	add_unit_test(${tname} "tests/eval/" "evaluator;parser2")
endforeach()
