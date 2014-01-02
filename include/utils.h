/*
This class provides various utilities to be used by other classes.
Right now it provides a matrix api for 2D or 3D matrices.
*/

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstdlib>
using namespace std;

template <class T>
class Matrix
{
    int rows, cols, height;
    T *data;

  public:
    Matrix(int num_rows, int num_cols, int h=1);
    T& operator() (int row, int col, int h=0);
    T operator() (int row, int col, int h=0) const;
    ~Matrix();
};

#endif
