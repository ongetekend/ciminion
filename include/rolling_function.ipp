#pragma once

template <class T>
void RollingFunction<T>::operator()(Vec<T> &state) const
{
	// Create the linear layer
	Mat<T> A;
	A.SetDims(3,3);
	A[0][2] = 1;
	A[1][0] = 1;
	A[2][1] = 1;

	// Create the vector (\iota_{a}, \ioata_{b}, \ioata_{c} + \ioata_{a}*\ioata_{b})^{\top}
	Vec<T> b = state;
	b[2] += b[0]*b[1]; 

	// Apply the rolling function and store the result back into state
	state =  A*b;
}
