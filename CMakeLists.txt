cmake_minimum_required(VERSION VERSION 3.22.1)

project(Vulkan_Sandbox VERSION 1.0.0)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
# set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_CXX_STANDARD 17)

# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file (
  GLOB_RECURSE SOURCES_LIST 
  ${PROJECT_SOURCE_DIR}/src/*.h; 
  ${PROJECT_SOURCE_DIR}/src/*.cpp; 
  ${PROJECT_SOURCE_DIR}/src/**/*.h;
  ${PROJECT_SOURCE_DIR}/src/**/*.cpp;
)

# Testing purpose
# file (
#   ${PROJECT_SOURCE_DIR}/src/Main.cpp;
# )

# Vulkan
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
  message ("-- Vulkan found.")
else (Vulkan_FOUND)
  message ("-- Vulkan not found.")
endif ()

# Vulkan shader compiler
find_program(GLSLC_EXECUTABLE NAMES glslc HINTS Vulkan::glslc)

if (GLSLC_EXECUTABLE)
  message("-- glslc found.")
else(GLSLC_EXECUTABLE)
  message("-- glslc not find.")
endif ()

# GLFW
set (GLFW_BUILD_DOCS OFF)
set (GLFW_BUILD_TESTS OFF)
set (GLFW_BUILD_EXAMPLES OFF)

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/glfw)

set (INCLUDE_DIR 
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/third_party/glfw/include;
  ${PROJECT_SOURCE_DIR}/third_party/glm;
  ${PROJECT_SOURCE_DIR}/third_party/spdlog/include;
)

# Maintain folder structure inside the build
# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES_LIST})

add_executable(${PROJECT_NAME} ${SOURCES_LIST})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan)

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS} ${INCLUDE_DIR})

# preprocessor definitions
target_compile_definitions(
  ${PROJECT_NAME} 
  PUBLIC 
  "$<$<CONFIG:DEBUG>:VKS_DEBUG>"
  "$<$<CONFIG:Release>:VKS_RELEASE>"
)

# target_compile_options() # compile flags
# target_link_options() # linker flags

# Copy assets to build folder
# add_custom_command(
#     TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_CURRENT_SOURCE_DIR}/assets
#     ${CMAKE_CURRENT_BINARY_DIR}/assets
# )

#==============================================================================
# COMPILE SHADERS
#==============================================================================

set(SHADER_SOURCE_DIR ${PROJECT_SOURCE_DIR}/assets/shaders)
set(SHADER_BINARY_DIR ${PROJECT_SOURCE_DIR}/assets/shaders/spv)

file(GLOB_RECURSE SHADERS_FILES
  ${SHADER_SOURCE_DIR}/*.vert
  ${SHADER_SOURCE_DIR}/*.frag
  ${SHADER_SOURCE_DIR}/*.comp
  ${SHADER_SOURCE_DIR}/*.geom
)

foreach(source IN LISTS SHADERS_FILES)
  get_filename_component(FILENAME ${source} NAME)
  add_custom_command(
    COMMAND ${GLSLC_EXECUTABLE} ${source} -o ${SHADER_BINARY_DIR}/${FILENAME}.spv 
    OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
    DEPENDS ${source}
    COMMENT "Compiling ${FILENAME}"
  )
  list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})