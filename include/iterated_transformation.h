#pragma once

#include "ntl_common.h"

/**
 * Class implementing the iterated transformation. 
 * The number of rounds and the round constants determine the exact permutation.
 * In practice, this is either P_N or P_R.
 *
 * @tparam T Type to work with. Either ZZ_p for GF(p) or ZZ_pE for GF(2^n).
 */
template <class T>
class IteratedTransformation
{
	private:
		const long rounds;
		const Vec<T> round_constants;
	public:
		IteratedTransformation(const long rounds, const Vec<T> &round_constants);
		void operator()(Vec<T> &state) const;
	private:
		void round(Vec<T> &state, const long roundIndex) const;
};

#include "iterated_transformation.ipp"
