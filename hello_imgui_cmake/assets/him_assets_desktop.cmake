# hello_imgui_bundle_assets_from_folder for Desktop platforms (Windows, Linux, macOS when not using app bundles)
#
# In this case, we will:
#     - copy the assets folder to the output directory (so that we can run the app without install)
#     - install the assets folder and the app exe to the install directory
#     - set the debugger working directory to the output directory (for Win32)

function(hello_imgui_get_real_output_directory app_name r)
    # Warning: RUNTIME_OUTPUT_DIRECTORY is stable, but RUNTIME_OUTPUT_DIRECTORY_CONFIG can vary between Debug/Release configs
    # cf https://cmake.org/cmake/help/latest/prop_tgt/RUNTIME_OUTPUT_DIRECTORY_CONFIG.html
    get_property(runtime_output_directory TARGET ${app_name} PROPERTY RUNTIME_OUTPUT_DIRECTORY)
    if ("${runtime_output_directory}" STREQUAL "")
        set(${r} ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
    else()
        set(${r} ${runtime_output_directory} PARENT_SCOPE)
    endif()
endfunction()


function(_do_copy_asset app_name src dst)
    hello_imgui_get_real_output_directory(${app_name} real_output_directory)

    FILE(COPY "${src}" DESTINATION "${real_output_directory}/${dst}")
    message(VERBOSE "_do_copy_asset=> FILE(COPY ${src} DESTINATION ${real_output_directory}/${dst})")

    if (HELLOIMGUI_ADD_APP_WITH_INSTALL)
        if (IS_DIRECTORY ${src})
            install(DIRECTORY "${src}" DESTINATION "${CMAKE_INSTALL_PREFIX}/${dst}")
            message(VERBOSE "_do_copy_asset=> install(DIRECTORY ${src} DESTINATION ${CMAKE_INSTALL_PREFIX}/${dst}  )")
        else()
            install(FILES "${src}" DESTINATION "${CMAKE_INSTALL_PREFIX}/${dst}")
            message(VERBOSE "_do_copy_asset=> install(FILES ${src} DESTINATION ${CMAKE_INSTALL_PREFIX}/${dst}  )")
        endif()
    endif()
endfunction()


# Bundle assets
function(hello_imgui_bundle_assets_from_folder app_name assets_folder)
    message(VERBOSE "hello_imgui_bundle_assets_from_folder ${app_name} ${assets_folder}")
    FILE(GLOB children ${assets_folder}/*)
    foreach(child ${children})
        _do_copy_asset(${app_name} ${child} assets/)
    endforeach()

    if (WIN32)
        # Fix msvc quirk: set the debugger working dir to the exe dir!
        hello_imgui_get_real_output_directory(${app_name} app_output_dir)
        set_target_properties(${app_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${app_output_dir})
    endif()

    # Install app exe to install directory
    if (HELLOIMGUI_ADD_APP_WITH_INSTALL)
        install(TARGETS ${app_name} DESTINATION ${CMAKE_INSTALL_PREFIX})
    endif()
endfunction()
