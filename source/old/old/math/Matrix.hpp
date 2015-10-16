/*
 * Math library intended for computer graphics, animation, physics and games
 * (but not restricted to it).
 *
 * Copyright (c) 2014 Renato Utsch <renatoutsch@gmail.com>
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

#ifndef MATH_MATRIX_HPP
#define MATH_MATRIX_HPP

#include "math.hpp"
#include <cstring>
#include <new>
#include <stdexcept>
#include <cfloat>
#include <string>

/**
 * Represents a generic matrix with arbitrary size.
 * @param NumRows The number of rows in the matrix.
 * @param NumColumns The number of columns in the matrix.
 * @param DataType The type of the matrix. Default is float.
 **/
template<size_t NumRows, size_t NumColumns, typename DataType = float>
class Matrix {
    /// The data of the matrix.
    DataType _data[NumRows][NumColumns];

public:
    /**
     * Acessing elements from the data. Fails silently (or not).
     **/
    inline DataType *operator[](size_t index) {
        return _data[index];
    }

    /**
     * Acessing elements from the data. Fails silently (or not).
     * Const version.
     **/
    inline const DataType *operator[](size_t index) const {
        return _data[index];
    }

    /**
     * Accessing elements from the data; throws std::out_of_range in case of
     * error.
     **/
    inline DataType &at(size_t row, size_t column) {
        if(row >= NumRows || column >= NumColumns)
            throw std::out_of_range("Trying to access invalid matrix element");
        return _data[row][column];
    }

    /**
     * Accessing elements from the data; throws std::out_of_range in case of
     * error.
     * Const version.
     **/
    inline const DataType &at(size_t row, size_t column) const {
        if(row >= NumRows || column >= NumColumns)
            throw std::out_of_range("Trying to access invalid matrix element");
        return _data[row][column];
    }

    /**
     * Returns the number of rows in the matrix.
     **/
    inline size_t getNumRows() {
        return NumRows;
    }

    /**
     * Returns the number of columns in the matrix.
     **/
    inline size_t getNumColumns() {
        return NumColumns;
    }

    /**
     * += operator for matrixes.
     * Throws std::runtime_error if the matrixes aren't of the same size.
     **/
    Matrix &operator+=(const Matrix &right) {
        if((getNumRows() != right.getNumRows())
                || getNumColumns() != right.getNumColumns()) {
            throw std::runtime_error("Invalid matrix size");
        }

        for(size_t i = 0; i < getNumRows(); ++i) {
            for(size_t j = 0; j < getNumColumns(); ++j) {
                _data[i][j] += right[i][j];
            }
        }
    }

    /**
     * -= operator for matrixes.
     * Throws std::runtime_error if the matrixes aren't of the same size.
     **/
    Matrix &operator-=(const Matrix &right) {
        if((getNumRows() != right.getNumRows())
                || getNumColumns() != right.getNumColumns()) {
            throw std::runtime_error("Invalid matrix size");
        }

        for(size_t i = 0; i < getNumRows(); ++i) {
            for(size_t j = 0; j < getNumColumns(); ++j) {
                _data[i][j] -= right[i][j];
            }
        }
    }

    /**
     * *= operator by a scalar.
     **/
    Matrix &operator*=(float right) {
        for(size_t i = 0; i < getNumRows(); ++i) {
            for(size_t j = 0; j < getNumColumns(); ++j) {
                _data[i][j] *= right;
            }
        }
    }

    /**
     * /= operator by a scalar.
     * if the scalar is 0, throws std::runtime_error.
     **/
    Matrix &operator/=(float right) {
        if(std::abs(right) < FLT_EPSILON)
            throw std::runtime_error("Division by zero");

        for(size_t i = 0; i < getNumRows(); ++i) {
            for(size_t j = 0; j < getNumColumns(); ++j) {
                _data[i][j] /= right;
            }
        }
    }

    /**
     * *= operator between matrixes.
     * If the matrixes are not square matrixes, throws a std::runtime_error
     * exception.
     **/
    Matrix &operator*=(const Matrix &right) {
        if(getNumRows() != right.getNumColumns()
                || getNumColumns() != right.getNumRows())
            throw std::runtime_error("Invalid matrix size");

        for(size_t i = 0; i < getNumRows(); ++i)
            for(size_t j = 0; j < right.getNumColumns(); ++j)
                for(size_t k = 0; k < getNumColumns(); ++k)
                    _data[i][k] *= right[k][j];
    }
};

/**
 * * operator between matrixes.
 * If the number of columns of the first matrix is different than the
 * number of lines of the second matrix a std::runtime_error exception
 * is thrown.
 **/
template<int a, int b, int c>
Matrix<a, c> operator*(const Matrix<a, b> &left, const Matrix<b, c> &right) {
    Matrix<a, c> m;

    for(size_t i = 0; i < left.getNumRows(); ++i)
        for(size_t j = 0; j < right.getNumColumns(); ++j)
            for(size_t k = 0; k < left.getNumColumns(); ++k)
                m[i][k] = left[i][k] * right[k][j];

    return m;
}

#endif // !MATH_MATRIX_HPP
