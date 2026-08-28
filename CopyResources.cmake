
# Copy `resources/` (all files and subdirectories) from source to binary dir.
# Usage:
# 1) Per-target, after the target is built:
#      add_executable(myapp ...)
#      add_copy_resources(myapp)
#
# 2) Or create a standalone target and make your target depend on it:
#      add_custom_target(copy_resources ALL ...)            # runs every build if ALL is used
#      add_dependencies(myapp copy_resources)              # copy runs before myapp builds
 
function(add_copy_resources target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            "${CMAKE_CURRENT_BINARY_DIR}/resources"
        COMMENT "Copying resources/ -> ${CMAKE_CURRENT_BINARY_DIR}/resources"
    )
endfunction()

# Standalone target you can build independently or make other targets depend on.
add_custom_target(copy_resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "${CMAKE_CURRENT_BINARY_DIR}/resources"
    COMMENT "Copying resources/ -> ${CMAKE_CURRENT_BINARY_DIR}/resources"
)

