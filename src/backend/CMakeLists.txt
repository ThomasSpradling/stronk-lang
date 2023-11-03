add_library(
    stronk_backend
    OBJECT
    vm.cpp
)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:stronk_backend>
    PARENT_SCOPE)