cmake_minimum_required(VERSION 4.4.2)
project(Sokoban LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(DLL_NAME ${PROJECT_NAME}_game)
set(SHARED_LIB_NAME ${PROJECT_NAME}_common)
set(DIR_ASSETS_ORIGIN ${CMAKE_SOURCE_DIR}/assets)
set(DIR_ASSETS_DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)
set(WINDOWS_LIBRARIES winmm)

# Automatically find all .lib files in lib/
file(GLOB_RECURSE LIB_FILES "${CMAKE_SOURCE_DIR}/lib/*.lib")

# EXE-only source
set(EXE_EXCLUSIVE
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

# Shared source, compiled once into a static lib, linked into both EXE and DLL
set(SHARED_SOURCES
    ${CMAKE_SOURCE_DIR}/src/arena.cpp
    ${CMAKE_SOURCE_DIR}/src/image.cpp
)

# Everything else in src/ (game logic) goes into the DLL
file(GLOB_RECURSE DLL_EXCLUSIVE "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM DLL_EXCLUSIVE ${EXE_EXCLUSIVE} ${SHARED_SOURCES})

# SHARED STATIC LIB
add_library(${SHARED_LIB_NAME} STATIC ${SHARED_SOURCES})
target_include_directories(${SHARED_LIB_NAME} PRIVATE include)

# EXE
add_executable(${PROJECT_NAME} ${EXE_EXCLUSIVE})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${LIB_FILES}
    ${SHARED_LIB_NAME}
    ${WINDOWS_LIBRARIES}
)

# GAME DLL
add_library(${DLL_NAME} SHARED ${DLL_EXCLUSIVE})
target_include_directories(${DLL_NAME} PRIVATE include)
target_link_libraries(${DLL_NAME} PRIVATE
    ${LIB_FILES}
    ${SHARED_LIB_NAME}
)

# Copy DLLs and assets to the executable's output directory
file(GLOB_RECURSE DLL_FILES "${CMAKE_SOURCE_DIR}/lib/*.dll")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_FILES}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${DIR_ASSETS_ORIGIN} ${DIR_ASSETS_DESTINATION}
    VERBATIM
)