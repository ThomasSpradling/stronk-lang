add_subdirectory(backend)
add_subdirectory(common)
add_subdirectory(frontend)

add_library(stronk STATIC ${ALL_OBJECT_FILES})

set(STRONKS_LIBS
    stronk_backend
    stronk_common
    stronk_frontend
    )

target_link_libraries(stronk ${STRONK_LIBS})

target_include_directories(
        stronk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)