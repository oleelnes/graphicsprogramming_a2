# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("pacman_3D")

# executable file creation
add_executable(${PROJECT_NAME})

# Add subdirectory with dependencies
add_subdirectory(external)

# Additional source files added here
target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/source/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/source/main.h 

    "source/window.cpp" 
    "source/window.h" 
    "source/game.cpp" 
    "source/game.h" 
    "source/Shader.cpp" 
    "source/Shader.h" 
    "source/GameRenderer.cpp" 
    "source/GameRenderer.h" 
    "source/MapLoader.cpp" 
    "source/MapLoader.h" 
    "source/Externs.h" 
      
    "source/Ghost.h")

# Include directories
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/source
    ${CMAKE_SOURCE_DIR}/stb/include
    )

# Linking to the external libraries
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    glad::glad
    glfw
    glm
    tinyobjloader
    stbimage::stbimage)

    # Copying the triangle.vs file to current binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/triangle.vs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders)

# Copying the triangle.fs file to current binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/triangle.fs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders)

     # Copying the level0.txt file to current binary directory 
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/other/level0.txt
 #    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/other)

 # Copying the pacman.png file to current binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/woodenwall.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/textures)

     # Copying the pacman.png file to current binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/pellet.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/textures)

          # Copying the pacman.png file to current binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/floor.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/textures)

               # Copying the pacman.png file to current binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/roof.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/textures)

                    # Copying the pacman.png file to current binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/ghost.jpg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/textures)


# Copy resources to binary directory
add_custom_target(
    copy_shaders ALL
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
    COMMAND ${CMAKE_COMMAND} -E echo "Copied resource directory to binary directory ${CMAKE_CURRENT_BINARY_DIR}"
)