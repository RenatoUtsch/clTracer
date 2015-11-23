/*
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

#ifndef CMDARGS_HPP
#define CMDARGS_HPP

#include <string>

/**
 * Represents the command line arguments.
 */
class CmdArgs {
    std::string _input, _output, _programName;
    int _width, _height, _numSamples, _aaLevel;

    /// Returns the given option or NULL if it wasn't found.
    char *getOption(char **begin, char **end, const std::string &option);

    /// Returns true if the given option exists or false if not.
    bool optionExists(char **begin, char **end, const std::string &option);

    /// Prints a command line error message and exits.
    void printErrorAndQuit(int argc, char **argv);

    /// Prints the help message and exits.
    void printHelpAndQuit(int argc, char **argv);

public:
    /**
     * Inits the class with the input from argc and argv.
     * This constructor may finish the program in case the input is invalid.
     */
    CmdArgs(int argc, char **argv);

    /// Returns the input filename.
    inline const std::string &inputFilename() const {
        return _input;
    }

    /// Returns the output filename.
    inline const std::string &outputFilename() const {
        return _output;
    }

    /// Returns the program name.
    inline const std::string &programName() const {
        return _programName;
    }

    /// Returns the width of the screen.
    inline int width() const {
        return _width;
    }

    /// Returns the height of the screen.
    inline int height() const {
        return _height;
    }

    /// Returns the number of samples per pixel part.
    inline int numSamples() const {
        return _numSamples;
    }

    /// Returns the anti aliasing level.
    inline int aaLevel() const {
        return _aaLevel;
    }
};

#endif // !CMDARGS_HPP
