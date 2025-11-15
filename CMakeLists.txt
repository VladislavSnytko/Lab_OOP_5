cmake_minimum_required(VERSION 3.14)
project(lab_05 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к заголовочным файлам
include_directories(${CMAKE_SOURCE_DIR}/include)

# Основная программа
add_executable(lab_05_app
    src/main.cpp
)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Для Windows: предотвращаем переопределение флагов компилятора
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Тесты
add_executable(lab_05_tests
    tests/test_all.cpp
)

target_link_libraries(lab_05_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(lab_05_tests)
