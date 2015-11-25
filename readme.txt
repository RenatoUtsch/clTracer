OpenCL Path Tracer
Renato Utsch Gonçalves

== Compiling
=============
To compile clTracer, CMake, OpenCL and a compliant C++14 compiler are needed.
If on Windows, Visual C++ 2015 is recommended.

The compilation was tested on OS X (using OpenCL on the CPU) and on Windows
(using OpenCL on a NVIDIA GPU).

Steps:
1. Extract the zip file to a new folder.
2. Browse to this folder with the terminal.
3. Create a new folder called "build".
4. Enter this new "build" folder.
5. Execute the command: "cmake .."
6.1. For OS X and Linux: run "make"
6.2. For Windows: open the Visual Studio project and compile it.
7. Run the generated binary as specified by the .pdf distributed with the project.

Some notes:

- The input file format changed a little in order to support additional
parameters. Now the light section doesn't exist anymore, but the spheres
have 3 aditional parameters that specify their emission. If a sphere emits
light, it is considered to not reflect or transmit any light. Also, the
ambient light coefficient was removed from the materials too.
- The command line arguments also changed a little. To specify the width
and height, the command must be "-w <width> -h <height>".

- Run "raytracer --help" for more information about the available commands.

- An option for anti-aliasing is available as "-aa level", where level is
the square root of the number of divisions per pixel for the multisampling.

- In case the execution fails, try commenting the lines 77 and 78 (that
enable optimizations) of source/clSampler/SamplerImpl.cpp.
This is known to work in some Intel CPUs.

- To force OpenCL to execute on the GPU instead of the CPU change the
SAMPLER_DEVICE_TYPE in source/clSampler/SamplerImpl.cpp at line 38.

== Implementation Decisions
===========================
This implementation implements all the minimum requirements (70% of the total
score), although using OpenCL for much higher performance. Importance
sampling and anti-aliasing (through supersampling) were also implemented.

clTracer was made in C++ with OpenCL.

The implementation divided the code in a class to interpret the input arguments,
a class to specify the constant world objects, a class to specify the screen
where the rays will pass through in the scene and a class to interact with
the OpenCL kernel. A class that represents a PPM image was also created.

The class that interacts with the OpenCL kernel uses another class that generates
OpenCL code that represents the constant world objects. This generated code is
concatenated with the OpenCL functions before compiling the OpenCL kernel and
executing it.

The source/clSampler/cl folder contains all the OpenCL source code.

The implemented PathTracer didn't divide between direct and indirect lights,
considering all the light on each iteration of the recursion, including
emitted light.
