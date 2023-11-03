add_library(
    stronk_compiler
    OBJECT
    compiler.cpp
    constant_pool.cpp
)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:stronk_compiler>
    PARENT_SCOPE)