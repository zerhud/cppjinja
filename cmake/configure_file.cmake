separate_arguments(cnt_list)
foreach(cnt ${cnt_list})
	file(READ "${${cnt}_file}" cnt_u)
	string(CONFIGURE "${cnt_u}" ${cnt}_cnt) # @ONLY doesn't work here :(
	unset(cnt_u)
endforeach()

configure_file("${input}" "${output}" @ONLY)

