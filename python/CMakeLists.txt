
pybind11_add_module(kp src/main.cpp)

# Set KOMPUTE_BUILD_PYTHON only for the Python module
target_compile_definitions(kp PRIVATE KOMPUTE_BUILD_PYTHON=1)

include_directories(
        ${PROJECT_SOURCE_DIR}/include/)

target_link_libraries(
        kp PRIVATE
        kompute::kompute)
