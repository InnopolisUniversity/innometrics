function(import_x11)
    find_package(X11 REQUIRED)

    link_libraries(${X11_LIBRARIES})

#    find_path(X11_Xi2_INCLUDE_PATH X11/extensions/XInput2.h              ${X11_INC_SEARCH_PATH})
#    find_path(X11_Xinput2_INCLUDE_PATH X11/extensions/XInput2.h          ${X11_INC_SEARCH_PATH})
#
#    find_library(X11_Xi2_LIB Xi2                ${X11_LIB_SEARCH_PATH})
#    find_library(X11_Xinput2_LIB Xi2             ${X11_LIB_SEARCH_PATH})
#
#    if (X11_Xi2_INCLUDE_PATH AND X11_Xi2_LIB)
#        set(X11_Xi2_FOUND TRUE)
#        set(X11_INCLUDE_DIR ${X11_INCLUDE_DIR} ${X11_Xi2_INCLUDE_PATH})
#    endif ()
#
#    if (X11_Xinput2_INCLUDE_PATH AND X11_Xinput2_LIB)
#        set(X11_Xinput2_FOUND TRUE)
#        set(X11_INCLUDE_DIR ${X11_INCLUDE_DIR} ${X11_Xinput2_INCLUDE_PATH})
#    endif ()
#
#    mark_as_advanced(
#            X11_Xi2_LIB
#            X11_Xi2_INCLUDE_PATH
#            X11_Xinput2_LIB
#            X11_Xinput2_INCLUDE_PATH
#    )
    find_path(X11_Xinput2_INCLUDE_PATH X11/extensions/XInput2.h ${X11_INC_SEARCH_PATH})

    find_library(X11_Xinput2_LIB Xi ${X11_LIB_SEARCH_PATH})

    IF (X11_Xinput2_INCLUDE_PATH AND X11_Xinput2_LIB)
        set(X11_Xinput2_FOUND TRUE)
        set(X11_INCLUDE_DIR ${X11_INCLUDE_DIR} ${X11_Xinput2_INCLUDE_PATH})
        message("X11 Xinput2 found")
    ENDIF (X11_Xinput2_INCLUDE_PATH AND X11_Xinput2_LIB)

    mark_as_advanced(
            X11_Xinput2_INCLUDE_PATH
            X11_Xinput2_LIB
    )


    include_directories(${X11_INCLUDE_DIR})

    link_libraries(${X11_Xmu_LIB})
    include_directories(${X11_Xmu_INCLUDE_PATH})

    include_directories(${X11_Xutil_INCLUDE_PATH})
    link_libraries(${X11_Xi_LIB})
    link_libraries(${X11_Xinput2_LIB})
    link_libraries(${X11_Xext_LIB})
endfunction()