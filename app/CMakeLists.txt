set(DUMMY_APP_NAME dummy)

add_executable(${DUMMY_APP_NAME} "app.cc")

target_link_libraries(${DUMMY_APP_NAME} PRIVATE pybind11::embed)
target_link_libraries(${DUMMY_APP_NAME} PRIVATE surfacepp_lib)

set_property(TARGET ${DUMMY_APP_NAME} PROPERTY CXX_STANDARD 17)
set_target_properties(${DUMMY_APP_NAME} PROPERTIES LINKER_LANGUAGE CXX)