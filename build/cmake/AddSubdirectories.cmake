macro(add_subdirectories rootdir exclude)
 file(GLOB localsub "${rootdir}/*" )
  foreach(dir ${localsub})
    if(IS_DIRECTORY ${dir})
        
	foreach (exclude_var ${exclude})
		#message(STATUS "exclud var (${exclude_var}) dir (${dir})")
		if ( ${dir} STREQUAL ${exclude_var})
			message(STATUS "excluding directory (${dir})")
        		set( ${dir}_exclude ON )
		endif()
	endforeach(exclude_var)

	if ( EXISTS ${dir}/CMakeLists.txt AND NOT ${dir}_exclude )
		message(STATUS "adding directory (${dir})")
        	add_subdirectory( ${dir} )
	endif()

    endif()
  endforeach(dir)
endmacro()