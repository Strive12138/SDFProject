
project(glad)

include_directories( 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


add_library( ${PROJECT_NAME}  STATIC
    src/glad.c
)