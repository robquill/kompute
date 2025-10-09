# Kompute Image Copy Example

This folder contains an end to end Kompute Example that implements an image copy and blur.
This example is structured such that you will be able to extend it for your project.
It contains a CMake build configuration that can be used in your production applications.

## Building the example

You will notice that it's a standalone project, so you can re-use it for your application.
It uses CMake's [`fetch_content`](https://cmake.org/cmake/help/latest/module/FetchContent.html) to consume Kompute as a dependency.
To build you just need to run the CMake command in this folder as follows:

```bash
git clone https://github.com/KomputeProject/kompute.git
cd kompute/examples/image_copy
mkdir build
cd build
cmake ..
cmake --build .
```

## Executing

Form inside the `build/` directory run:

### Linux

```bash
./kompute_image_copy <path-to-jpg-input-image>
```

### Windows

```bash
.\Debug\kompute_image_copy.exe <path-to-jpg-input-image>
```

## Pre-requisites

In order to run this example, you will need the following dependencies:

* REQUIRED
    + The Vulkan SDK must be installed

For the Vulkan SDK, the simplest way to install it is through [their website](https://vulkan.lunarg.com/sdk/home). You just have to follow the instructions for the relevant platform.
