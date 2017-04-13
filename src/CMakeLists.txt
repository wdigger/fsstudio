cmake_minimum_required(VERSION 3.16)

find_package(Qt5 COMPONENTS Core Gui Widgets Multimedia Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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

add_executable(FSStudio WIN32 MACOSX_BUNDLE ${FSSTUDIO_SOURCES} ${FSSTUDIO_HEADERS})
target_link_libraries(FSStudio freeserf_data Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Network Qt5::Multimedia)
install(TARGETS FSStudio DESTINATION ".")

if(WIN32)
  include(Windeployqt)
  windeployqt(FSStudio ".")
elseif(APPLE)
  include(Macdeployqt)
  macdeployqt(FSStudio)
endif()

set(CPACK_PACKAGE_VERSION_MAJOR	${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR	${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH	${PROJECT_VERSION_PATCH})
if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WIN_ARCH_STRING x64)
  else()
    set(WIN_ARCH_STRING x86)
  endif()
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${WIN_ARCH_STRING}")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
  set(CPACK_PACKAGE_EXECUTABLES "FSStudio;FSStudio")
  set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
  include(InstallRequiredSystemLibraries)
elseif(APPLE)
  set(CPACK_GENERATOR "DragNDrop")
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
else()
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
endif()
include(CPack)
