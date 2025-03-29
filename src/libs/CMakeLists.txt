add_library(eigen_headers INTERFACE)
target_include_directories(eigen_headers INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
