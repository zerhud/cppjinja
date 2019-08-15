get_filename_component(out_name "${out}" NAME)
if("$ENV{${env_var}}" STREQUAL "")
	# download
	message(WARNING "downloading file ${out_name}")
	file(DOWNLOAD "${url}" "${out}")
else()
	# copy from env
	get_filename_component(dir_out "${out}" DIRECTORY)
	get_filename_component(env_name "$ENV{${env_var}}" NAME)
	file(COPY "$ENV{${env_var}}" DESTINATION "${dir_out}")
	file(RENAME "${dir_out}/${env_name}" "${dir_out}/${out_name}")
endif()

