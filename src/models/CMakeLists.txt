add_library(models_lib
    obanalyzer.cpp
    obanalyzer.h
)

target_include_directories(models_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(models_lib PUBLIC
    Qt6::Core
    dao_lib
)
