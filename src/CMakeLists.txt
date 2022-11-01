set(TARGET_NAME surfacepp_lib)

file(GLOB_RECURSE SURFACEPP_SOURCE_FILES
    *.c
    *.cc
    *.cpp)

file(GLOB_RECURSE SURFACEPP_HEADER_FILES
    *.h
    *.hpp)

add_library(surfacepp_lib ${SURFACEPP_SOURCE_FILES} ${SURFACEPP_HEADER_FILES})
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${TARGET_NAME} PUBLIC ${surfacepp_SOURCE_DIR}/src)
target_include_directories(${TARGET_NAME} PUBLIC "${FREETYPE_INCLUDE_DIR}")
target_include_directories(${TARGET_NAME} PUBLIC "${GLFW_INCLUDE_DIR}")
target_include_directories(${TARGET_NAME} PUBLIC "${GLAD_INCLUDE_DIR}")
target_include_directories(${TARGET_NAME} PUBLIC "${GLM_INCLUDE_DIR}")
target_include_directories(${TARGET_NAME} PUBLIC "${IMGUI_INCLUDE_DIR}")
target_include_directories(${TARGET_NAME} PUBLIC "${STB_IMAGE_INCLUDE_DIR}")
target_include_directories(${TARGET_NAME} PUBLIC "${SOLOUD_INCLUDE_DIR}")
target_include_directories(${TARGET_NAME} PUBLIC "${ENTT_INCLUDE_DIR}")
target_include_directories(${TARGET_NAME} PUBLIC "${YAML_CPP_INCLUDE_DIR}")

target_link_libraries(${TARGET_NAME} PRIVATE pybind11::embed)
target_link_libraries(${TARGET_NAME} PRIVATE "${OPENGL_LIBRARIES}")
target_link_libraries(${TARGET_NAME} PRIVATE "${GLEW_LIBRARIES}")
target_link_libraries(${TARGET_NAME} PRIVATE "${DEFAULT_LIBRARIES}")
target_link_libraries(${TARGET_NAME} PRIVATE "${OPENGL_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${FREETYPE_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${GLFW_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${CMAKE_DL_LIBS}")
target_link_libraries(${TARGET_NAME} PRIVATE "${GLAD_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${ASSIMP_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${IMGUI_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${STB_IMAGE_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${SOLOUD_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${ENTT_LIBRARY}")
target_link_libraries(${TARGET_NAME} PRIVATE "${YAML_CPP_LIBRARY}")

source_group("sources" FILES ${SOURCE_FILES_EXE})
source_group("headers" FILES ${HEADER_FILES_EXE})