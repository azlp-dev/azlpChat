cmake_minimum_required(VERSION 3.16)

project(AZChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AZChat
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        app.rc
        logindialog.h logindialog.cpp logindialog.ui
        rc.qrc
        register_dialog.h register_dialog.cpp register_dialog.ui
        global.h global.cpp
        singleton.h
        httpmgr.h httpmgr.cpp
        config.ini
        timerbtn.h timerbtn.cpp
        clickedlable.h clickedlable.cpp
        resetdialog.h resetdialog.cpp resetdialog.ui
        tcpmgr.h tcpmgr.cpp
        usermgr.h usermgr.cpp
        chatdialog.h chatdialog.cpp chatdialog.ui
        clickedbtn.h clickedbtn.cpp
        customizeedit.h customizeedit.cpp
        chatuserlist.h chatuserlist.cpp
        chatuserwid.h chatuserwid.cpp chatuserwid.ui
        itembase.h itembase.cpp
        loading_dialog.h loading_dialog.cpp loading_dialog.ui
        chatpage.h chatpage.cpp chatpage.ui
        chatview.h chatview.cpp
        chatitembase.h chatitembase.cpp
        bubble_frame.h bubble_frame.cpp
        text_bubble.h text_bubble.cpp
        pic_bubble.h pic_bubble.cpp
        msgtextedit.h msgtextedit.cpp
        statewidget.h statewidget.cpp
        searchlist.h searchlist.cpp
        userinfo.h userinfo.cpp
        adduseritem.h adduseritem.cpp adduseritem.ui
        find_success_dlg.h find_success_dlg.cpp find_success_dlg.ui
        clickedoncelb.h clickedoncelb.cpp
        addfriend_dlg.h addfriend_dlg.cpp addfriend_dlg.ui
        friendlb.h friendlb.cpp friendlb.ui
        contactlist.h contactlist.cpp
        conuseritem.h conuseritem.cpp conuseritem.ui
        grouptip_item.h grouptip_item.cpp grouptip_item.ui
        friendapply_page.h friendapply_page.cpp friendapply_page.ui
        friendapply_page.h friendapply_page.cpp friendapply_page.ui
        friendappy_item.h friendappy_item.cpp friendappy_item.ui
        friendapply_list.h friendapply_list.cpp
        find_failed_dlg.h find_failed_dlg.cpp find_failed_dlg.ui
        authenfriend.h authenfriend.cpp authenfriend.ui
        friendinfopage.cpp friendinfopage.h friendinfopage.ui




    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET AZChat APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(AZChat SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(AZChat
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(AZChat PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

add_custom_command(TARGET AZChat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/config.ini
    $<TARGET_FILE_DIR:AZChat>
)

target_include_directories(AZChat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.AZChat)
endif()
set_target_properties(AZChat PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS AZChat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AZChat)

endif()
