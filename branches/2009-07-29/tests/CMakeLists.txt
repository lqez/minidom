include_directories( ../minidom )

foreach( DOCTYPE ${SUPPORT_DOCTYPE} )
	if( ${DOCTYPE} MATCHES XML )
		set( SAMPLES
			${SAMPLES}
			${PROJECT_SOURCE_DIR}/tests/*.xml
			)
		set( ITEMS
			${ITEMS}
			xml_load.cpp
			xml_get.cpp
			xml_query.cpp
			)
		if( ENABLE_DUMP )
			set( ITEMS
				${ITEMS}
				xml_dump.cpp
				xml_dump_to_file.cpp
				)
		endif( ENABLE_DUMP )
		if( ENABLE_ICONV )
			set( ITEMS
				${ITEMS}
				xml_utf8_to_euckr.cpp
				)
		endif( ENABLE_ICONV )
	elseif( ${DOCTYPE} MATCHES NKV )
		set( SAMPLES
			${SAMPLES}
			${PROJECT_SOURCE_DIR}/tests/*.nkv
			)
		set( ITEMS
			${ITEMS}
			nkv_load.cpp
			)
		if( ENABLE_DUMP )
			set( ITEMS
				${ITEMS}
				nkv_dump.cpp
				)
		endif( ENABLE_DUMP )
	elseif( ${DOCTYPE} MATCHES INI )
		set( SAMPLES
			${SAMPLES}
			${PROJECT_SOURCE_DIR}/tests/*.ini
			)
		set( ITEMS
			${ITEMS}
			ini_load.cpp
			ini_get.cpp
			)
		if( ENABLE_DUMP )
			set( ITEMS
				${ITEMS}
				ini_dump.cpp
				)
		endif( ENABLE_DUMP )
	elseif( ${DOCTYPE} MATCHES HTTP )
		set( SAMPLES
			${SAMPLES}
			${PROJECT_SOURCE_DIR}/tests/http_response.txt
			)
		set( ITEMS
			${ITEMS}
			http_response.cpp
			)
	elseif( ${DOCTYPE} MATCHES JSON )
		set( SAMPLES
			${SAMPLES}
			${PROJECT_SOURCE_DIR}/tests/*.json
			)
		set( ITEMS
			${ITEMS}
			json_load.cpp
			)
		if( ENABLE_DUMP )
			set( ITEMS
				${ITEMS}
				json_dump.cpp
				)
		endif( ENABLE_DUMP )
	endif( ${DOCTYPE} MATCHES XML )
endforeach( DOCTYPE ${SUPPORT_DOCTYPE} )

create_test_sourcelist( TESTS
	tests.cpp
	${ITEMS}
	)

add_executable( tests ${TESTS} )
target_link_libraries( tests minidom )

remove( TESTS tests.cpp )

# push test cases into the target
foreach( test ${TESTS} )
	get_filename_component( TName ${test} NAME_WE )
	add_test( ${TName} tests ${TName} )
endforeach( test )

# copy test resources into test directory
file( GLOB TEST_RESOURCES
	${SAMPLES}
	)
foreach( file ${TEST_RESOURCES} )
	add_custom_command( 
		TARGET tests
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy ${file} ${PROJECT_BINARY_DIR}/tests
		)
endforeach( file ${TEST_RESOURCES} )

# copy dynamic library into test directory
if( WIN32 )
file( GLOB DYNAMIC_LIBS
	${PROJECT_SOURCE_DIR}/requisite/libiconv/debug/*.dll
	${PROJECT_SOURCE_DIR}/requisite/libiconv/release/*.dll
	${PROJECT_BINARY_DIR}/minidom/*.dll
	)
foreach( file ${DYNAMIC_LIBS} )
	add_custom_command( 
		TARGET tests
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy ${file} ${PROJECT_BINARY_DIR}/tests
		)
endforeach( file ${DYNAMIC_LIBS} )
endif( WIN32 )

