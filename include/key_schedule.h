#pragma once

#include "iterated_transformation.h"
#include "ntl_common.h"

/**
 * Class implementing the key schedule used in the Farfalle-like authenticated encryption construction.
 *
 * @tparam T The type to work with. Either ZZ_p for GF(p) or ZZ_pE for GF(2^n).
 */
template <class T>
class KeySchedule
{
	private:
		const IteratedTransformation<T> &p_n;
	public:
		KeySchedule(IteratedTransformation<T> &p_n);
		Vec<T> operator()(const Vec<T> &key_state, const long n) const;	
};

#include "key_schedule.ipp"
