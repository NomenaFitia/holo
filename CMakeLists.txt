cmake_minimum_required(VERSION 3.20)
project(V2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pour utiliser vcpkg automatiquement
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Trouver DCMTK et DirectX 11
find_package(DCMTK REQUIRED)
find_package(Directxtk REQUIRED)
find_package(glm REQUIRED)

# Fichiers sources
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Exécutable
add_executable(V2 ${SOURCES} ${HEADERS} "src/main.cpp")

# Liaisons
target_include_directories(V2 PRIVATE ${DCMTK_INCLUDE_DIRS} include)
target_link_libraries(V2 PRIVATE DCMTK::dcmdata
    DCMTK::ofstd
    DCMTK::oflog
    DCMTK::dcmimgle
    DCMTK::dcmimage
    d3d11.lib 
    dxgi.lib
    glm::glm)