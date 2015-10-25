/*
 * Author: Renato Utsch Gonçalves
 * Computer Science, UFMG
 * Advanced Computer Graphics
 * Practical exercise 1 - Distributed Ray Tracer
 *
 * Copyright (c) 2015 Renato Utsch <renatoutsch@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "CmdArgs.hpp"
#include "error.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>

char *CmdArgs::getOption(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

bool CmdArgs::optionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

void CmdArgs::printErrorAndQuit(int argc, char **argv) {
    std::cerr << argv[0] << ": invalid command line arguments\n"
        << "Try '" << argv[0] << "--help' for more information."
        << std::endl;
    exit(1);
}

void CmdArgs::printHelpAndQuit(int argc, char **argv) {
    std::cerr << "Usage: " << argv[0] << " input output [options]\n"
        << "Raytrace a scene specified from input file to output.\n"
        << "\nMandatory positional arguments:\n"
        << "input\t\tInput filename with the scene description\n"
        << "output\t\tOutput image filename\n"
        << "\nOptions:\n"
        << "--help\t\tShow help information\n"
        << "--realtime\t\tRun raytracer in realtime\n"
        << "-w <arg>\t\tSet the width of the image to <arg>\n"
        << "-h <arg>\t\tSet the height of the image to <arg>\n"
        << "-aa <arg>\t\tSet the antialiasing level of the image to <arg>\n"
        << "-ss <arg>\t\tSet the soft shadows level of the image to <arg>";

    std::cerr << std::endl;
    exit(1);
}

bool CmdArgs::getExtSpec(const std::string &input) {
    // Get the object file format.
    std::ifstream in(input);
    stop_if(!in.is_open(), "failed to open input file: %s", input.c_str());
    int magic;
    in >> magic;

    if(magic == 424242)
        return true;
    else
        return false;
}

CmdArgs::CmdArgs(int argc, char **argv) {
    if(optionExists(argv, argv + argc, "--help"))
        printHelpAndQuit(argc, argv);
    if(argc < 3)
        printErrorAndQuit(argc, argv);

    // Parse positional arguments.
    _input = argv[1];
    _output = argv[2];

    // Init options with default values.
    _width = 800;
    _height = 600;
    _aa = 1;
    _ss = 1;
    _extSpec = getExtSpec(_input);
    _realtime = false;

    // Parse options.
    if(optionExists(argv, argv + argc, "--realtime")) {
        _realtime = true;
    }
    if(optionExists(argv, argv + argc, "-w")) {
        char *opt = getOption(argv, argv + argc, "-w");
        if(!opt) printErrorAndQuit(argc, argv);

        _width = (int) strtol(opt, NULL, 10);
    }
    if(optionExists(argv, argv + argc, "-h")) {
        char *opt = getOption(argv, argv + argc, "-h");
        if(!opt) printErrorAndQuit(argc, argv);

        _height = (int) strtol(opt, NULL, 10);
    }
    if(optionExists(argv, argv + argc, "-aa")) {
        char *opt = getOption(argv, argv + argc, "-aa");
        if(!opt) printErrorAndQuit(argc, argv);

        _aa = (int) strtol(opt, NULL, 10);

        int result = _aa != 1 && _aa != 2 && _aa != 4 && _aa != 8 && _aa != 16;
        stop_if(result, "Invalid aa level: must be 1, 2, 4, 8, or 16.");
    }
    if(optionExists(argv, argv + argc, "-ss")) {
        char *opt = getOption(argv, argv + argc, "-ss");
        if(!opt) printErrorAndQuit(argc, argv);

        if(!_extSpec) {
            printf("Input file must use extended specification for -ss to be used.\n");
            exit(1);
        }

        _ss = (int) strtol(opt, NULL, 10);

        int result = _ss != 1 && _ss != 2 && _ss != 4 && _ss != 8 && _ss != 16;
        stop_if(result, "Invalid ss level: must be 1, 2, 4, 8 or 16.");
    }
}
