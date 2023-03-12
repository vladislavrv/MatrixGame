add_executable(MatrixGameLauncher)

file(GLOB MATRIX_LAUNCHER_SOURCES src/*.cpp)
file(GLOB MATRIX_LAUNCHER_HEADERS src/*.h)
file(GLOB MATRIX_RESOURCES resources/*)

source_group ("MatrixLauncher\\Source Files" FILES ${MATRIX_LAUNCHER_SOURCES})
source_group ("MatrixLauncher\\Header Files" FILES ${MATRIX_LAUNCHER_HEADERS})
source_group ("MatrixLauncher\\Resources" FILES ${MATRIX_RESOURCES})

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
        ${COMPILE_DEFINITIONS}
        -DUNICODE -D_UNICODE)

target_link_libraries(
    MatrixGameLauncher
        MatrixGame
)

target_sources(MatrixGameLauncher PRIVATE
    src/WindowsMain.cpp
)