find_file(turtle turtle/mock.hpp)
get_filename_component(turtle "${turtle}" DIRECTORY)
get_filename_component(turtle "${turtle}" DIRECTORY)

# unit tests...
enable_testing()
macro(add_unit_test tname path lib)
	add_executable(${tname}_test "${path}${tname}_test.cpp")
	target_link_libraries(${tname}_test PRIVATE ${lib} ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
	target_include_directories(${tname}_test SYSTEM PRIVATE "${Boost_INCLUDE_DIR}" "${turtle}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
	add_test(NAME ${tname} COMMAND ${tname}_test)
endmacro()

set(tests_parser2 rules_common rules_opterm rules_single rules_blocks rules_tmpls)
foreach(tname ${tests_parser2})
	add_unit_test(${tname} "tests/parser/" "parser2")
endforeach()

set(tests_loader load_common)
foreach(tname ${tests_loader})
	add_unit_test(${tname} "tests/loader/" "loader")
endforeach()

set(tests_eval eval_common eval_op_out)
foreach(tname ${tests_eval})
	add_unit_test(${tname} "tests/eval/" "evaluator;parser2")
endforeach()
