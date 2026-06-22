add_executable(amoeba_dsp_test amoeba_dsp_test.cpp)
target_include_directories(amoeba_dsp_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../source/include")
target_compile_features(amoeba_dsp_test PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(amoeba_dsp_test PRIVATE /W4)
else()
    target_compile_options(amoeba_dsp_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME amoeba_dsp_test COMMAND amoeba_dsp_test)
