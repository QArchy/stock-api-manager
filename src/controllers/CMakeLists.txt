add_library(controllers_lib
    Logger/logger.cpp
    Logger/logger.h
    Settings/apiSettings.cpp
    Settings/apiSettings.h
    Translator/translator.h
)

target_include_directories(controllers_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(controllers_lib PUBLIC
    Qt6::Core
)
