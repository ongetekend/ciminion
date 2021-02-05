#pragma once

#include "iterated_transformation.h"
#include "rolling_function.h"
#include "key_schedule.h"
#include "ntl_common.h"

using namespace std;

/**
 * Class implementing the Farfalle-like construction for authenticated encryption.
 *
 * @tparam T Type to work with. Either ZZ_p for GF(p) or ZZ_pE for GF(2^n). 
 */
template <class T>
class FarfalleLike
{
	private:
		const IteratedTransformation<T> &p_n;
		const IteratedTransformation<T> &p_r;
		const RollingFunction<T> &roll;
		const KeySchedule<T> ks;
		
		/**
		 * The publically known value that is set to 1.
		 */
		const T KS_CHI = T(1L);
	public:
		FarfalleLike(IteratedTransformation<T> &p_n, IteratedTransformation<T> &p_r, RollingFunction<T> &roll); 
		tuple<Vec<T>, T> operator()(const Vec<T> &MK, const T &nonce, const Vec<T> &Mseq) const;
};

#include "farfalle_like.ipp"
