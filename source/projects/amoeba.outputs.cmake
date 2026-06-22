include("${MAX_SDK_BASE_DIR}/script/max-pretarget.cmake")

include_directories(
    "${MAX_SDK_INCLUDES}"
    "${MAX_SDK_MSP_INCLUDES}"
    "${MAX_SDK_JIT_INCLUDES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)

file(GLOB PROJECT_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(${PROJECT_NAME} MODULE ${PROJECT_SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-cast-function-type-mismatch -Wno-variadic-macro-arguments-omitted)
endif()

include("${MAX_SDK_BASE_DIR}/script/max-posttarget.cmake")
