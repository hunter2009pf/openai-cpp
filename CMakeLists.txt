# CMakeList.txt: openai-cpp 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

SET(CMAKE_TOOLCHAIN_FILE "E:\\openAI_cpp_sdk\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
SET(CURL_INCLUDE_DIR "E:\\openAI_cpp_sdk\\libcurl_openssl_compile\\curl\\builds\\libcurl-vc14-x64-release-static-ssl-static-ipv6\\include")

project ("openai-cpp")

include_directories(
	${PROJECT_SOURCE_DIR}/include/openai
	${PROJECT_SOURCE_DIR}/include/openai/models
	${PROJECT_SOURCE_DIR}/include/openai/utils
	PRIVATE ${CURL_INCLUDE_DIR}
)

set(src
	src/openai_service.cpp
	src/openai_api.cpp
	src/openai_constant.cpp
)

# 将源代码添加到此项目的可执行文件。
add_library (openai-cpp ${src} "include/openai/openai_api.h" "include/openai/openai_config.h" "include/openai/openai_constant.h" "src/openai_api.cpp" "src/openai_constant.cpp" "include/openai/models/openai_model.h" "include/openai/models/completion_data.h" "include/openai/utils/json_util.h" "src/utils/json_util.cpp" "include/openai/models/completion.h" "include/openai/models/edit_data.h" "include/openai/models/edit.h" "include/openai/models/image_data.h"  "include/openai/models/embeddings_data.h" "include/openai/models/embedding.h" "include/openai/models/transcription_data.h" "include/openai/models/openai_file.h" "include/openai/models/file_data.h" "include/openai/models/moderation_data.h" "include/openai/models/moderation.h" "include/openai/models/fine_tune.h" "include/openai/models/fine_tune_data.h" "src/openai_config.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET openai-cpp PROPERTY CXX_STANDARD 20)
endif()

find_package(OpenSSL REQUIRED)
target_link_libraries(openai-cpp OpenSSL::SSL)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(openai-cpp jsoncpp_lib)

# find_package(CURL CONFIG)
# target_link_libraries(openai-cpp ${CURL_LIBRARY})
# 指定引用的外部库的搜索路径
#LINK_DIRECTORIES("E:\\openAI_cpp_sdk\\libcurl_openssl_compile\\curl\\builds\\libcurl-vc14-x64-release-static-ssl-static-ipv6\\lib")
#target_link_libraries(openai-cpp libcurl_a.lib)
target_link_libraries(openai-cpp "E:\\openAI_cpp_sdk\\libcurl_openssl_compile\\curl\\builds\\libcurl-vc14-x64-release-static-ssl-static-ipv6\\lib\\libcurl_a.lib")

# TODO: 如有需要，请添加测试并安装目标。
