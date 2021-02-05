#pragma once

#include "ntl_common.h"

/**
 * Class implementing the rolling function used within the Farfalle-like authenticated encryption construction.
 * 
 * @tparam T The type to work with. Either ZZ_p for GF(p) or ZZ_pE for GF(2^n). 
 */
template <class T>
class RollingFunction
{
	public:
		void operator()(Vec<T> &state) const;
};

#include "rolling_function.ipp"
