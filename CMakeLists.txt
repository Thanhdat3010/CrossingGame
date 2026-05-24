cmake_minimum_required(VERSION 3.15)
project(CrossingGame C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cấu hình thư mục output cho file chạy
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include các đường dẫn header
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/extern/sdl3/include
)

# Danh sách file nguồn
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Font.cpp
    src/People.cpp
)

# Tạo file thực thi
add_executable(CrossingGame ${SOURCES})

# Liên kết các thư viện tĩnh (.lib) cho linker
target_link_libraries(CrossingGame PRIVATE
    ${CMAKE_SOURCE_DIR}/extern/sdl3/lib/SDL3.lib
    ${CMAKE_SOURCE_DIR}/extern/sdl3/lib/SDL3_image.lib
    ${CMAKE_SOURCE_DIR}/extern/sdl3/lib/SDL3_mixer.lib
)

# Tự động copy file DLL sang thư mục chứa file .exe sau khi build thành công
add_custom_command(TARGET CrossingGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/extern/sdl3/bin/SDL3.dll"
        "${CMAKE_SOURCE_DIR}/extern/sdl3/bin/SDL3_image.dll"
        "${CMAKE_SOURCE_DIR}/extern/sdl3/bin/SDL3_mixer.dll"
        $<TARGET_FILE_DIR:CrossingGame>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:CrossingGame>/assets"
    COMMENT "Copying SDL3 DLLs and assets to output directory..."
)
