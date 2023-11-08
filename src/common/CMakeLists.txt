add_library(
    stronk_common
    OBJECT
    number_generator.cpp
    utils.cpp
    value.cpp
)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:stronk_common>
    PARENT_SCOPE)