include_directories( ../iconv )

set( SRCS
    minidom.cpp
    minidom.h
    minidom_error.h

    minidom_xml_parser.h
    minidom_ini_parser.h
    minidom_json_parser.h
    minidom_nkv_parser.h
    minidom_http_parser.h
    )

add_library( minidom ${SRCS} )

set_target_properties( minidom
    PROPERTIES DEBUG_POSTFIX d
)

foreach( DOCTYPE ${SUPPORT_DOCTYPE} )
    add_definitions( -DMINIDOM_SUPPORT_${DOCTYPE} )
endforeach( DOCTYPE ${SUPPORT_DOCTYPE} )

add_definitions( -DMINIDOM_TARGET_ENCODING="${DEFAULT_ENCODING}" )
add_definitions( -DMINIDOM_BUFFER_SIZE=${TEMP_BUFFER_SIZE} )

if( ENABLE_STL_MAP )
    add_definitions( -DMINIDOM_ENABLE_MAP )
endif( ENABLE_STL_MAP )

if( ENABLE_DUMP )
    add_definitions( -DMINIDOM_ENABLE_DUMP )
endif( ENABLE_DUMP )

if( ENABLE_ICONV )
    add_definitions( -DMINIDOM_ENABLE_ICONV )

    if( WIN32 )
        target_link_libraries( minidom
            debug ${PROJECT_SOURCE_DIR}/requisite/libiconv/debug/iconvd.lib
            optimized ${PROJECT_SOURCE_DIR}/requisite/libiconv/release/iconv.lib
        )
    endif( WIN32 )

    if( APPLE )
        target_link_libraries( minidom iconv )
    endif( APPLE )

endif( ENABLE_ICONV )

if( BUILD_SHARED_LIBS )
    if( WIN32 )
        add_definitions( -DMINIDOM_DLL -DMINIDOM_EXPORT )
    endif( WIN32 )
endif( BUILD_SHARED_LIBS )

