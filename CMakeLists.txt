cmake_minimum_required(VERSION 2.8.12.2)
project(linux_c C)

set(CMAKE_C_STANDARD 99)

add_executable(linux_c main.c)
add_executable(process_communicate_pipe process_communicate/pipe.c)
add_executable(process_communicate_pipe_fifo process_communicate/pipe_fifo.c)
#add_executable(process_communicate_signal process_communicate/signal.c)
add_executable(file file/file.c)

