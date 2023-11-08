add_library(
    stronk_frontend
    OBJECT
    code_generator.cpp
    parser.cpp
    scanner.cpp
)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:stronk_frontend>
    PARENT_SCOPE)