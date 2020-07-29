cmake_minimum_required(VERSION 3.10)

#Set up project
project(RochaTest VERSION 1.0)

add_library(Rocha
    src/Rocha.cpp
    src/Builtin.cpp
    src/RochaAssembler.cpp
)

target_compile_features(Rocha PUBLIC cxx_std_17)

#Set executable
add_executable(${PROJECT_NAME}     
    src/main.cpp
)

#Set C++17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

#Set flags
if(MSVC)
  	target_compile_options(${PROJECT_NAME} PRIVATE 
    	/W4 /WX)
else()
  	target_compile_options(${PROJECT_NAME} PRIVATE 
		-Wall -Wextra -pedantic -Werror)		#Warning flags
endif()

#Finally
target_link_libraries(${PROJECT_NAME} 
    Rocha
)
