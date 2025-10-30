# cmake_minimum_required(VERSION 3.16)
# project(sdl3Game)

# # set the output directory for built objects.
# # This makes sure that the dynamic library goes into the build directory automatically.
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# # This assumes the SDL source is available in vendored/SDL
# add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# # This assumes the SDL_image source is available in vendored/SDL_image
# # add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)

# # Create your game executable target as usual
# add_executable(main WIN32 src/main.cpp)

# # Link to the actual SDL3 library. SDL3_image::SDL3_image
# target_link_libraries(main PRIVATE SDL3::SDL3)

 cmake_minimum_required(VERSION 3.16)
 project(sdl3Game)

 # set the output directory for built objects.
 # This makes sure that the dynamic library goes into the build directory automatically.
 set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
 set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

 # Use vendored libs
 set(SDLTTF_VENDORED ON)

 # This assumes the SDL source is available in vendored/SDL
 add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

 # This assumes the SDL_ttf source is available in vendored/SDL_ttf
 # add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

 # Create your game executable target as usual
 add_executable(main WIN32 src/main.cpp)

 # Link to the actual SDL3 library.SDL3_ttf::SDL_ttf
 target_link_libraries(main PRIVATE SDL3::SDL3)






# cmake_minimum_required(VERSION 3.16)
# project(sdl3Game)

# # Set the output directory for built objects
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# # Find SDL3 and SDL_ttf packages
# find_package(SDL3 REQUIRED)
# find_package(SDL3_ttf REQUIRED)

# # Create your game executable
# add_executable(main WIN32 src/main.cpp)

# # Link to SDL3 and SDL_ttf libraries
# target_link_libraries(main PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

# # Include directories
# target_include_directories(main PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_TTF_INCLUDE_DIRS})
