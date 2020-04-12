cmake_minimum_required(VERSION 3.13)
project(RustCompiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(RustCompiler main.cpp StateMachine.cpp StateMachine.h State.cpp State.h Transition.cpp Transition.h IDState.cpp IDState.h NumState.cpp NumState.h StringState.cpp StringState.h IgnoreState.cpp IgnoreState.h SeparatorState.cpp SeparatorState.h)