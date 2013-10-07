#include "utils.h"

template <class T>
Matrix<T>::Matrix(int num_rows, int num_cols, int h)
{
    rows = num_rows;
    cols = num_cols;
    height = h;
    data = new T[rows*cols*height];
}

template <class T>
T& Matrix<T>::operator() (int row, int col, int h)
{
    if(row>=rows || col>=cols || h>=height)
    {
        cout<<"error occurred: matrix subscripts out of bound!";
        exit(1);
    }
    return data[(cols*row + col)*height + h];
}

template <class T>
T Matrix<T>::operator() (int row, int col, int h) const
{
    if(row>=rows || col>=cols || h>=height)
   {
       cout<<"error occurred: matrix subscripts out of bound!";
       exit(1);
   }
   return data[(cols*row + col)*height + h];
}

template <class T>
Matrix<T>::~Matrix()
{
    delete data;
}

template class Matrix<int>;
template class Matrix<float>;
