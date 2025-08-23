cmake_minimum_required(VERSION 3.20)
project(V2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pour utiliser vcpkg automatiquement
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Trouver DCMTK
find_package(DCMTK REQUIRED)

# Fichiers sources
file(GLOB SOURCES "src/*.cpp" "utils/*")
file(GLOB HEADERS "include/*.h")

# Exécutable
add_executable(V2 ${SOURCES} ${HEADERS} "src/main.cpp" "include/Utils.h")

# Liaisons
target_include_directories(V2 PRIVATE ${DCMTK_INCLUDE_DIRS} include)
target_link_libraries(V2 PRIVATE DCMTK::dcmdata
    DCMTK::ofstd
    DCMTK::oflog
    DCMTK::dcmimgle
    DCMTK::dcmimage
)