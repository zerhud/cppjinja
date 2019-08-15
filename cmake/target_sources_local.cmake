function(target_sources_local target header source prefix)
	target_sources(${target}
		PUBLIC
			"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${prefix}${header}>"
			"$<INSTALL_INTERFACE:${prefix}${header}>"
		PRIVATE
			"${prefix}${source}"
		)
endfunction()

function(target_sources_copy target dir)
	list(REMOVE_AT ARGV 0)
	list(REMOVE_AT ARGV 0)
	set(result_src)
	foreach(src ${ARGV})
		get_filename_component(src_name "${src}" NAME)
		get_filename_component(src_ext "${src}" EXT)
		if("${src_ext}" STREQUAL ".info")
			set(target_src "${dir}/../${src_name}")
		else()
			set(target_src "${dir}/${src_name}")
		endif()
		list(APPEND result_src "${target_src}")
		add_custom_command(
			OUTPUT "${target_src}"
			COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/${src}" "${target_src}"
			DEPENDS "${CMAKE_CURRENT_LIST_DIR}/${src}"
			)
	endforeach()
	target_sources(${target} PRIVATE ${src})
	target_sources(${target} PRIVATE ${result_src})
endfunction()

