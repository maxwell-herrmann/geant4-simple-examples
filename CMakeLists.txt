#--Adding this stuff from other things I've seen hopefully it all works

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

add_subdirectory(simplest)
add_subdirectory(obscurer)

add_custom_target(PE DEPENDS simplest obscurer)
