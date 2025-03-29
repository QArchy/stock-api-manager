add_library(ui_lib
    mainwindow/mainwindow.cpp
    mainwindow/mainwindow.h
    mainwindow/mainwindow.ui
)

target_include_directories(ui_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ui_lib PUBLIC
    Qt6::Widgets
)
