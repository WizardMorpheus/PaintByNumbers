cmake_minimum_required(VERSION 3.10)
project(PaintByNumbers)

###############################################################################
## file globbing ##############################################################
###############################################################################

# these instructions search the directory tree when CMake is
# invoked and put all files that match the pattern in the variables
# `sources` and `data`
# file(GLOB_RECURSE sources     src/gl.c src/main.cpp src/**/*.cpp src/**/*.h include/imgui/*.h include/imgui/*.cpp)
file(GLOB_RECURSE sources     src/main.cpp src/gl.c include/imgui/*.cpp src/**/*.cpp)
file(GLOB_RECURSE resources resources/*)
# you can use set(sources src/main.cpp) etc if you don't want to
# use globbing to find files automatically

###############################################################################
## target definitions #########################################################
###############################################################################

# add the data to the target, so it becomes visible in some IDE
add_executable(PaintByNumbers ${sources} ${resources})

# just for example add some compiler flags
target_compile_options(PaintByNumbers PUBLIC -std=c++20 -Wall -Wfloat-conversion)

# this lets me include files relative to the root source directory with a <> pair
target_include_directories(PaintByNumbers PUBLIC
    ./src/
    ./include/
)

target_link_directories(PaintByNumbers PUBLIC
    lib
)

target_link_libraries(PaintByNumbers PUBLIC
    glfw3
) 

# this copies all resource files in the build directory
# we need this, because we want to work with paths relative to the executable
file(COPY ${resources} DESTINATION resources)

###############################################################################
## dependencies ###############################################################
###############################################################################

# this defines the variables Boost_LIBRARIES that contain all library names
# that we need to link to


###############################################################################
## packaging ##################################################################
###############################################################################

# all install commands get the same destination. this allows us to use paths
# relative to the executable.
install(TARGETS PaintByNumbers DESTINATION example_destination)
# this is basically a repeat of the file copy instruction that copies the
# resources in the build directory, but here we tell CMake that we want it
# in the package
install(DIRECTORY resources DESTINATION example_destination)

# now comes everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here
set(CPACK_PACKAGE_NAME "MyExample")
set(CPACK_PACKAGE_VERSION "1.0.0")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)