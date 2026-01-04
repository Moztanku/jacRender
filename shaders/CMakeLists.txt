## CMakeLists for generating the shaders
add_custom_target(shaders)

set(SHADER_SOURCES
    generic.vert
    generic.frag
)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/compiled)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/compiled)
endif()

foreach(SHADER ${SHADER_SOURCES})
    add_custom_command(
        TARGET shaders
        COMMAND glslc ${SHADER} -DMAX_POINT_LIGHTS=${MAX_POINT_LIGHTS} -o ${CMAKE_CURRENT_SOURCE_DIR}/compiled/${SHADER}.spv
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Compiling ${SHADER} to SPIR-V"
        VERBATIM
        POST_BUILD
    )
endforeach()
