#include "main.h"

/**
 * Convert n bytes into a ZZ_p x, where x = sum(b[i]*256^i, i=0..n-1).
 *
 * @param bytes Pointer to a contiguous sequence of bytes. 
 * @param n The number of bytes of the sequence that we consider.
 * @return A ZZ_p object 
 */
ZZ_p ZZ_p_from_bytes(const uint8_t *bytes, long n)
{
	return to_ZZ_p(ZZFromBytes(bytes, n));
}

/**
 * Convert n bytes into a ZZ_pE p, where p = sum(b[i,j]x^{8*i+j}, i=0..n-1, j=0..7).
 *
 * @param bytes Pointer to a contiguous sequence of bytes. 
 * @param n The number of bytes of the sequence that we consider.
 * @return A ZZ_pE object 
 */
ZZ_pE ZZ_pE_from_bytes(const uint8_t *bytes, long n)
{
	Vec<ZZ_p> coefficientVector;
	coefficientVector.SetLength(8*n);
	// Convert byte sequence to coefficent vector over ZZ_p. 
	for (long i = 0; i < n; i++) {
		for (long j = 0; j < 8; j++) {
			if (bytes[i] & (1 << j)) {
				coefficientVector[8*i+j] = 1;
			}	
		}	
	}
	// Convert coefficient vector to a reduced polynomial.
	ZZ_pX p = to_ZZ_pX(coefficientVector);
	ZZ_pE P = to_ZZ_pE(p);

	return P;
}

/**
 * Generate the round constants by applying SHAKE256 to msg and converting the
 * resulting bytes into an object of type T (either ZZ_p or ZZ_pE).
 *
 * @tparam T Type to work with. Either ZZ_p or ZZ_pE.
 * @param rc The vector which contains the round constants upon completion. 
 * @param convert A pointer to a function to convert the bytes into objects of type T.
 * @param msg The input message to SHAKE256.
 */
template <class T>
void initialize_round_constants(Vec<T> &rc, T (*convert)(const uint8_t *, long), const string &msg)
{
	// Generate byte sequence 
	uint8_t digest[DIGESTSIZE];
	FIPS202_SHAKE256((const uint8_t *) msg.c_str(), msg.size(), digest, DIGESTSIZE);

	// Convert bytes to elements of T and store them into rc.
	for (long i = 0; i < NUMBER_OF_ROUND_CONSTANTS_N; i++) {
		T c = (*convert)(digest + i*ELEMENTSIZE_IN_BYTES, ELEMENTSIZE_IN_BYTES);
		// IMPORTANT: all of the round constants we found were non-zero and not
		// equal to one, hence we did not need to filter anything.
		rc[i] = c;
	}
}

/**
 * Example code showing how to use the cipher in the case of GF(p).
 */
void test_ZZ_p()
{
	// We generated a random prime of approximately 128 bits.
	string prime = "258439831533290445326983084816294483837";
	// Define GF(prime)
	ZZ_p::init(conv<ZZ>((const char *) prime.c_str()));

	// Generate round constants
	Vec<ZZ_p> rcs_n;
	rcs_n.SetLength(NUMBER_OF_ROUND_CONSTANTS_N);
	string msg = "GF(" + prime + ")";
	initialize_round_constants<ZZ_p>(rcs_n, &ZZ_p_from_bytes, msg);
	Vec<ZZ_p> rcs_r;
	rcs_r.SetLength(NUMBER_OF_ROUND_CONSTANTS_R);
	for (long i = 0; i < NUMBER_OF_ROUND_CONSTANTS_R; i++) {
		rcs_r[i] = rcs_n[START_R + i];
	}

	// Instantiate the permutations, rolling function, and farfalle-like construction
	IteratedTransformation<ZZ_p> p_n(NUMBER_OF_ROUNDS_N, rcs_n);
	IteratedTransformation<ZZ_p> p_r(NUMBER_OF_ROUNDS_R, rcs_r);
	RollingFunction<ZZ_p> roll;
	FarfalleLike<ZZ_p> authenc(p_n, p_r, roll);

	// Create the master key	
	Vec<ZZ_p> MK;
	MK.SetLength(2);
	MK[0] = 0L;
	MK[1] = 0L;

	// Create the nonce
	ZZ_p nonce = ZZ_p(1L);

	// Create message sequence
	Vec<ZZ_p> Mseq;
	Mseq.SetLength(2);
	Mseq[0] = 0L;
	Mseq[1] = 1L;
	
	// Compute ciphertexts and tag
	Vec<ZZ_p> Cseq; 
	Cseq.SetLength(2);
	ZZ_p tag;
	tie(Cseq, tag) = authenc(MK, nonce, Mseq);

	// Print outputs to stdout
	cout << "The case GF(p):\n";	
	cout << "Round constants for p_n:\n";
	for (long i = 0; i < rcs_n.length(); i++) {
		int round_number = i/4;
		int round_index = i%4;
		cout << "(" << round_number << "," << round_index << ") " << rcs_n[i] << "\n";
	}
	cout << "Round constants for p_r:\n";
	for (long i = 0; i < rcs_r.length(); i++) {
		int round_number = i/4;
		int round_index = i%4;
		cout << "(" << round_number << "," << round_index << ") " << rcs_r[i] << "\n";
	}
	cout << "Master key pair: (" << MK[0] << "," << MK[1] << ")\n";
	cout << "Nonce: " << nonce << "\n";
	cout << "Message sequence:\n\t" << Mseq[0] << ",\n\t" << Mseq[1] << "\n";
	cout << "Ciphertext sequence:\n\t" << Cseq[0] << ",\n\t" << Cseq[1] << "\n";
	cout << "Tag:\n\t" << tag << "\n" << endl;
}

/**
 * Example code showing how to use the cipher in the case of GF(2^n).
 */
void test_ZZ_pE()
{
	// Define GF(2)
	ZZ_p::init(ZZ(2));

	// Create irreducible polynomial P := x^128+x^7+x^2+x+1
	// Note that the vector [P_{0}, P_{1}, ... , P_{n}] encodes the 
	// polynomial P_{0} + P_{1}X + \cdots + P_{n}X^n 
	ZZ_pX P;
	P.SetLength(129);
	P[0] = 1;
	P[1] = 1;
	P[2] = 1;
	P[7] = 1;
	P[128] = 1;

	// Define GF(2^128) = GF(2)[X]/(P)
	ZZ_pE::init(P);

	// Build message for round constant generation
	// The hex encoding of P is 1[0]{30}87 (1, followed by 30 zeros, followed by 87)
	string msg = "GF(2)[X]/";
	msg.append("1"); 
	msg.append(string(30, '0'));
	msg.append("87");

	// Generate round constants
	Vec<ZZ_pE> rcs_n;
	rcs_n.SetLength(NUMBER_OF_ROUND_CONSTANTS_N);
	initialize_round_constants<ZZ_pE>(rcs_n, &ZZ_pE_from_bytes, msg);
	Vec<ZZ_pE> rcs_r;
	rcs_r.SetLength(NUMBER_OF_ROUND_CONSTANTS_R);
	for (long i = 0; i < NUMBER_OF_ROUND_CONSTANTS_R; i++) {
		rcs_r[i] = rcs_n[START_R + i];
	}

	// Instantiate the permutations, rolling function, and farfalle-like construction
	IteratedTransformation<ZZ_pE> p_n(NUMBER_OF_ROUNDS_N, rcs_n);
	IteratedTransformation<ZZ_pE> p_r(NUMBER_OF_ROUNDS_R, rcs_r);
	RollingFunction<ZZ_pE> roll;
	FarfalleLike<ZZ_pE> authenc(p_n, p_r, roll);

	// Create master key	
	Vec<ZZ_pE> MK;
	MK.SetLength(2);
	MK[0] = 0UL;
	MK[1] = 0UL;

	// Create nonce
	ZZ_pE nonce = ZZ_pE(1UL);

	// Create message sequence
	Vec<ZZ_pE> Mseq;
	Mseq.SetLength(2);
	Mseq[0] = 0L;
	Mseq[1] = 1L;

	// Compute ciphertexts and tag
	Vec<ZZ_pE> Cseq; 
	Cseq.SetLength(2);
	ZZ_pE tag;
	tie(Cseq, tag) = authenc(MK, nonce, Mseq);

	// Print outputs to stdout
	cout << "The case GF(2^n):\n";
	cout << "Round constants for p_n:\n";
	for (long i = 0; i < rcs_n.length(); i++) {
		int round_number = i/4;
		int round_index = i%4;
		cout << "(" << round_number << "," << round_index << ") " << rcs_n[i] << "\n";
	}
	cout << "Round constants for p_r:\n";
	for (long i = 0; i < rcs_r.length(); i++) {
		int round_number = i/4;
		int round_index = i%4;
		cout << "(" << round_number << "," << round_index << ") " << rcs_r[i] << "\n";
	}
	cout << "Master key pair: (" << MK[0] << "," << MK[1] << ")\n";
	cout << "Nonce: " << nonce << "\n";
	cout << "Message sequence:\n\t" << Mseq[0] << ",\n\t" << Mseq[1] << "\n";
	cout << "Ciphertext sequence:\n\t" << Cseq[0] << ",\n\t" << Cseq[1] << "\n";
	cout << "Tag:\n\t" << tag << "\n" << endl;
}

int main()
{
	test_ZZ_p();
	test_ZZ_pE();

	return 0;
}

