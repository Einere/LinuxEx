set(src main.cpp)

add_executable(FiboTest ${src})

target_include_directories(FiboTest PUBLIC ../fibolib)
target_link_libraries(${PROJECT_NAME} fibolib)


