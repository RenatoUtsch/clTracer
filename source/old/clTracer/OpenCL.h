#ifndef RAYTRACER_OPENCL_H
#define RAYTRACER_OPENCL_H

#ifdef __APPLE__
#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>
#else
#include <CL/cl.h>
#include <CL/cl_gl.h>
#endif

#endif // !RAYTRACER_OPENCL_H
