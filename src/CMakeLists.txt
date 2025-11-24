add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cpp camera.cpp engine.cpp window.cpp shader.cpp viewport.cpp glad.c)
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)
