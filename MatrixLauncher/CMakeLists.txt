add_executable(MatrixGameLauncher)

file(GLOB MATRIX_LAUNCHER_SOURCES src/*.cpp src/*.c)
file(GLOB MATRIX_LAUNCHER_HEADERS src/*.hpp src/*.h)

set(MATRIX_RESOURCES
    MatrixGame.ico
    MatrixGame.rc
    Resource.h)

target_sources(
    MatrixGameLauncher
    PRIVATE
        ${MATRIX_LAUNCHER_SOURCES}
        ${MATRIX_LAUNCHER_HEADERS}
        ${MATRIX_RESOURCES})

target_include_directories(
    MatrixGameLauncher
    PUBLIC
        .
        src
        "$<$<C_COMPILER_ID:MSVC>:${DIRECTX9_INCLUDE_DIRS}>")

target_compile_options(
    MatrixGameLauncher
    PUBLIC
        ${COMPILE_OPTIONS})

target_compile_definitions(
    MatrixGameLauncher
    PUBLIC
        MATRIXGAME_EXPORTS
        $<$<CONFIG:Release>:ASSERT_OFF>
        _ALLOW_RTCc_IN_STL
        WINDOWS_IGNORE_PACKING_MISMATCH
        ${COMPILE_DEFINITIONS})

target_link_options(
    MatrixGameLauncher
    PUBLIC
        "$<$<C_COMPILER_ID:MSVC>:-SAFESEH:NO>"
        ${LINK_OPTIONS})

target_link_libraries(
    MatrixGameLauncher
        MatrixGame
)

source_group("Resources" FILES ${MATRIX_RESOURCES})

target_sources(MatrixGameLauncher PRIVATE
    src/WindowsMain.cpp
)