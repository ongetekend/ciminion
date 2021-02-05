#pragma once

template <class T>
IteratedTransformation<T>::IteratedTransformation(const long rounds, const Vec<T> &round_constants) : rounds(rounds), round_constants(round_constants) 
{
	if (round_constants.length() != 4*rounds) {
		throw "The number of round constants doesn't match the number of rounds times four.";
	}
}

template <class T>
void IteratedTransformation<T>::round(Vec<T> &state, const long roundIndex) const
{
	// Create the linear layer
	Mat<T> A;
	A.SetDims(3,3);
	A[0][2] = 1;
	A[1][0] = 1;
	A[2][1] = 1;
	A[2][2] = 1;
	A[1][1] = round_constants[4*roundIndex+3];
	A[1][2] = round_constants[4*roundIndex+3];

	// Create the vector (\mathit{RC3}_{\ell}, \mathit{RC1}_{\ell},\mathit{RC2}_{\ell})^{\top}
	Vec<T> c;
	c.SetLength(3);
	c[0] = round_constants[4*roundIndex+2];
	c[1] = round_constants[4*roundIndex];
	c[2] = round_constants[4*roundIndex+1];

	// Create the vector (a_{i-1}, b_{i-1}, c_{i-1}+a_{i-1}*b_{i-1})^{\top}
	Vec<T> b = state;
	b[2] += b[0]*b[1]; 

	// Apply the round function and store the result in state
	state = A*b+c;
}

template <class T>
void IteratedTransformation<T>::operator()(Vec<T> &state) const
{
	for (long i = 0; i < rounds; i++) {
		round(state, i);
	}
}
