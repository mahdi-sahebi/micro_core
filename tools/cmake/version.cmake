# Drops a `<name>-<arch>-<version><suffix>` copy of TARGET next to the
# original build output. Library targets get a `lib` prefix, executables
# do not. Requires PROJECT_VERSION and PROJECT_VERSION_SUFFIX to be set.
function(MC_VERSIONED_NAME TARGET)
    if (PROJECT_VERSION STREQUAL "0.0.0")
        set(_version_part "${PROJECT_VERSION_SUFFIX}")
    else()
        set(_version_part "-${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}")
    endif()

    get_target_property(_type ${TARGET} TYPE)
    if (_type STREQUAL "STATIC_LIBRARY" OR _type STREQUAL "SHARED_LIBRARY" OR _type STREQUAL "MODULE_LIBRARY")
        set(_versioned "lib${TARGET}-${CMAKE_SYSTEM_PROCESSOR}${_version_part}$<TARGET_FILE_SUFFIX:${TARGET}>")
        set(_label "Library")
    else()
        set(_versioned "${TARGET}-${CMAKE_SYSTEM_PROCESSOR}${_version_part}$<TARGET_FILE_SUFFIX:${TARGET}>")
        set(_label "Executable")
    endif()

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "[    ]"
        COMMAND ${CMAKE_COMMAND} -E echo "[    ] [Release ${_label}]"
        COMMAND ${CMAKE_COMMAND} -E echo "[    ] $<TARGET_FILE:${TARGET}>"
        COMMAND ${CMAKE_COMMAND} -E echo "[    ] $<TARGET_FILE_DIR:${TARGET}>/${_versioned}"
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${TARGET}>
            $<TARGET_FILE_DIR:${TARGET}>/${_versioned}
        COMMAND ${CMAKE_COMMAND} -E echo "[    ]"
    )
endfunction()
