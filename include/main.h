#pragma once

#include <stdint.h>
#include <stdio.h>

extern "C" {
        #include "Keccak_readable_and_compact.h"
}
#include "iterated_transformation.h"
#include "farfalle_like.h"
#include "rolling_function.h"
#include "ntl_common.h"

using namespace std;

const long NUMBER_OF_ROUNDS_N = 134;
const long NUMBER_OF_ROUNDS_R = 10;
const long NUMBER_OF_ROUND_CONSTANTS_N = 4*NUMBER_OF_ROUNDS_N;
const long NUMBER_OF_ROUND_CONSTANTS_R = 4*NUMBER_OF_ROUNDS_R;
const long START_R = NUMBER_OF_ROUND_CONSTANTS_N - NUMBER_OF_ROUND_CONSTANTS_R;
const long ELEMENTSIZE_IN_BYTES = 16;
const long DIGESTSIZE = NUMBER_OF_ROUND_CONSTANTS_N*ELEMENTSIZE_IN_BYTES;

ZZ_p ZZ_p_from_bytes(const uint8_t *, long);
ZZ_pE ZZ_pE_from_bytes(const uint8_t *, long);
template <class T> void initialize_round_constants(Vec<T> &, T (*convert)(const uint8_t *, long), const string &);
void test_ZZ_p();
void test_ZZ_pE();
