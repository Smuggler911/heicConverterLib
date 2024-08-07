cmake_minimum_required(VERSION 3.28)
project(Converter)

set(CMAKE_CXX_STANDARD 17)

include_directories("C:/Program Files/Zulu/zulu-8/include")
include_directories("C:/Program Files/Zulu/zulu-8/include/win32")
include_directories("C:/Users/Smugg/vcpkg/installed/x64-windows/include")
include_directories("C:/Users/Smugg/vcpkg/installed/x64-windows/include/libde265")
include_directories("C:/Users/Smugg/vcpkg/installed/x64-windows/include/libheif")
include_directories("C:/libjpeg-turbo64/include")
link_directories("C:/Program Files/Zulu/zulu-8/include")
link_directories("C:/Program Files/Zulu/zulu-8/include/win32")
link_directories("C:/Users/Smugg/vcpkg/installed/x64-windows/include")
link_directories("C:/Users/Smugg/vcpkg/installed/x64-windows/include/libde265")
link_directories("C:/Users/Smugg/vcpkg/installed/x64-windows/include/libheif")
link_directories("C:/libjpeg-turbo64/include")


add_executable(Converter
        src/JNIConverter.h
        src/JNIConverter.cpp
)
