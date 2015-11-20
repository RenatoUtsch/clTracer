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

- The command line arguments changed a little in order to support additional
parameters. The only thing that is different from the specification is how
to change the width and height of the output image: now instead of passing them
as the 3th and 4th arguments, the command must be "-w <width> -h <height>".

- Run "raytracer --help" for more information about the available commands.

- In order to use the -ss option for soft shadows, a slightly different version
of the input scene file format has to be used. The test<1-6>.in files distributed
with the project were all converted to test<1-6>_ext.in files that run with
the -ss option. The only differences between the file formats are a 424242 magic
number at the beginning of the file and an added light size field to the lights.

- In case the execution fails, try commenting the lines 103 and 104 (that
enable optimizations) of source/clSampler/Sampler.cpp, so that the string passed
as parameter is only "-DMAX_DEPTH=" STR(MAX_DEPTH). This is known to work in
some Intel CPUs.

- To force OpenCL to execute on the CPU instead of the GPU change the
SAMPLER_DEVICE_TYPE in source/clSampler/Sampler.cpp at lines 49~53.

== Implementation Decisions
===========================
This implementation implements all the minimum requirements (70% of the total
score), although using OpenCL for much higher performance. Soft shadows and
supersampling (using distributed raytracing) were also implemented.

The RayTracer was made in C++ with OpenCL.

The implementation divided the code in a class to interpret the input arguments,
a class to specify the constant world objects, a class to specify the screen
where the rays will pass through in the scene and a class to interact with
the OpenCL kernel. A class that intended to be used for the realtime OpenGL
renderer was not finished. A class that represents a PPM image was also created.

The class that interacts with the OpenCL kernel uses another class that generates
OpenCL code that represents the constant world objects. This generated code is
concatenated with the OpenCL functions before compiling the OpenCL kernel and
executing it.

The source/clSampler/sampler.cl file contains all the OpenCL source code. I
should have divided it into multiple files, but I was lazy as this wasn't
trivial in OpenCL and I didn't have much time.

The implemented RayTracer uses Phong model for local illumination and has
recursive calls (simulated through a stack as OpenCL doesn't support recursion)
for reflection and refraction. The RayTracer has options to be distributed and
generate soft shadows and do supersampling for antialiasing.

