cmake_minimum_required ( VERSION 2.6 )

project ( minidom )

set( DEFAULT_ENCODING euc-kr
	CACHE STRING "Set your default encoding."
	)

set( ENABLE_ICONV OFF
	CACHE BOOL "Set on to apply xml encodings with iconv library."
	)

set( ENABLE_DUMP OFF
	CACHE BOOL "Set on to enable method that dump(save) dom into string or file."
	)

set( TEMP_BUFFER_SIZE 4096
	CACHE STRING "Buffer size for element name and its value while minidom parsing & dumping."
	)

set( SUPPORT_DOCTYPE XML
	CACHE STRING "Build with specified doctypes, separate with semicolon. XML;INI;JSON;HTTP;NKV are supported."
	)

set( BUILD_SHARED_LIBS OFF
	CACHE BOOL "Turn on if you want to build shared(dylib,so,dll) libraries."
	)

add_subdirectory( minidom )
enable_testing()
add_subdirectory( tests )
