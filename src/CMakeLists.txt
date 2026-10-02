cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(FSSTUDIO_SOURCES animationview.cpp
                     audioview.cpp
                     colorlabel.cpp
                     datamodel.cpp
                     exportdialog.cpp
                     exporter.cpp
                     main.cpp
                     mainwindow.cpp
                     qpathedit.cpp
                     resourcemodel.cpp
                     resourceview.cpp
                     sourcesdialog.cpp
                     sourcesview.cpp
                     spriteview.cpp
                     sprite-file-qt.cpp
                     ../res/resources.qrc)

set(FSSTUDIO_HEADERS animationview.h
                     audioview.h
                     colorlabel.h
                     datamodel.h
                     exportdialog.h
                     exporter.h
                     mainwindow.h
                     qpathedit.h
                     resourcemodel.h
                     resourceview.h
                     sourcesdialog.h
                     sourcesview.h
                     spriteview.h)

qt_add_executable(${PROJECT_NAME} WIN32 ${FSSTUDIO_SOURCES} ${FSSTUDIO_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE freeserf_data Qt6::Core Qt6::Gui Qt6::Widgets
                      Qt6::Multimedia)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
)

install(TARGETS ${PROJECT_NAME} DESTINATION ".")

if(APPLE)
    # Qt is deployed into the installed bundle (used by cpack), not into the
    # build tree: macdeployqt rewrites and re-signs binaries, so running it on
    # every build breaks incremental builds and the bundle signature.
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt
                 HINTS "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}" REQUIRED)

    # Deploy only the plugins the application needs: they are copied into the
    # bundle and macdeployqt then resolves their dependencies.
    # Package managers (Homebrew in particular) keep plugins of all Qt modules
    # in a single directory (of symlinks), so letting macdeployqt copy
    # everything drags in QML, VirtualKeyboard, WebEngine's QtPdf, etc. with
    # unresolvable dependencies.
    set(DEPLOY_PLUGINS_CODE "")
    foreach(plugin platforms|Qt6::QCocoaIntegrationPlugin
                   styles|Qt6::QMacStylePlugin
                   multimedia|Qt6::QDarwinMediaPlugin)
        string(REPLACE "|" ";" plugin "${plugin}")
        list(GET plugin 0 plugin_type)
        list(GET plugin 1 plugin_target)
        string(APPEND DEPLOY_PLUGINS_CODE "
        get_filename_component(plugin \"$<TARGET_FILE:${plugin_target}>\" REALPATH)
        file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/PlugIns/${plugin_type}\"
             TYPE SHARED_LIBRARY FILES \"\${plugin}\")")
    endforeach()

    # Plugins from package managers depend on third-party libraries
    # referenced via @rpath, so tell macdeployqt where to look for them.
    set(MACDEPLOYQT_ARGS "-no-plugins")
    foreach(dir "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_LIBS}"
                /opt/homebrew/lib /usr/local/lib /opt/local/lib)
        if(IS_DIRECTORY "${dir}")
            list(APPEND MACDEPLOYQT_ARGS "-libpath=${dir}")
        endif()
    endforeach()
    list(JOIN MACDEPLOYQT_ARGS "\" \"" MACDEPLOYQT_ARGS)

    install(CODE "${DEPLOY_PLUGINS_CODE}
        set(bundle \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app\")
        message(STATUS \"Deploying Qt into \${bundle}\")
        execute_process(COMMAND \"${MACDEPLOYQT_EXECUTABLE}\" \"\${bundle}\" \"${MACDEPLOYQT_ARGS}\"
                        RESULT_VARIABLE result)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR \"macdeployqt failed: \${result}\")
        endif()
        execute_process(COMMAND codesign --verify --deep --strict \"\${bundle}\"
                        RESULT_VARIABLE result)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR \"Bundle signature verification failed\")
        endif()
    ")
endif()

if(WIN32)
    # Qt is deployed next to the installed executable (used by cpack), so the
    # build tree keeps running against the Qt installation.
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt
                 HINTS "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}" REQUIRED)

    # The compiler runtime is installed by InstallRequiredSystemLibraries.
    install(CODE "
        set(exe \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.exe\")
        message(STATUS \"Deploying Qt for \${exe}\")
        execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" \"\${exe}\"
                                --no-compiler-runtime
                                --no-translations
                                --no-system-d3d-compiler
                                --no-opengl-sw
                        RESULT_VARIABLE result)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR \"windeployqt failed: \${result}\")
        endif()
        file(GLOB platform_plugins
             \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/platforms/qwindows*.dll\")
        if(NOT platform_plugins)
            message(FATAL_ERROR \"Qt platform plugin was not deployed\")
        endif()
    ")
endif()

set(CPACK_PACKAGE_VERSION_MAJOR  ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR  ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH  ${PROJECT_VERSION_PATCH})
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WIN_ARCH_STRING x64)
    else()
        set(WIN_ARCH_STRING x86)
    endif()
    set(CPACK_GENERATOR "NSIS")
    if("${CMAKE_BUILD_TYPE}" STREQUAL "")
        set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${WIN_ARCH_STRING}-Develop")
    else()
        set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${WIN_ARCH_STRING}-${CMAKE_BUILD_TYPE}")
    endif()
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME};${PROJECT_NAME}")
    # install VC redistributables
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
        set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
    include(InstallRequiredSystemLibraries)
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}")
else()
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}")
endif()
include(CPack)
