#pragma once

template <class T>
FarfalleLike<T>::FarfalleLike(IteratedTransformation<T> &p_n, IteratedTransformation<T> &p_r, RollingFunction<T> &roll) : 
	p_n(p_n), 
	p_r(p_r), 
	roll(roll), 
	ks(KeySchedule<T>(p_n)) {}

template <class T>
tuple<Vec<T>, T> FarfalleLike<T>::operator()(const Vec<T> &MK, const T &nonce, const Vec<T> &Mseq) const
{

	if (Mseq.length() % 2 != 0) {
		throw "Expecting an even number of messages.";
	}
	
	long number_of_pairs = Mseq.length()/2;

	// Generate working keys
	Vec<T> key_state;
	key_state.SetLength(3);
	key_state[0] = KS_CHI;
	key_state[1] = MK[0];
	key_state[2] = MK[1];
	// We need 2 keys per pair, 2 keys for the call to p_n, and 1 key for tag creation. 
	Vec<T> working_keys = ks(key_state, 2*number_of_pairs+3);

	// Create initial state
	Vec<T> inner_upper_branches;
	inner_upper_branches.SetLength(3);
	inner_upper_branches[0] = nonce;
	inner_upper_branches[1] = working_keys[1];
	inner_upper_branches[2] = working_keys[2];

	// Inner call to P_N
	p_n(inner_upper_branches);
	// Branch duplication
	Vec<T> inner_lower_branches = inner_upper_branches;
	T accum;
	// Process ciphertexts
	Vec<T> Cseq;
	Cseq.SetLength(Mseq.length());
	for (long i = 0; i < number_of_pairs; i++) {
		inner_lower_branches[0] += working_keys[2*i+4];	
		inner_lower_branches[1] += working_keys[2*i+3];
		roll(inner_lower_branches);
		// Branch duplication
		Vec<T> outer_branches = inner_lower_branches;
		p_r(outer_branches);
		// Encrypt the plaintexts
		Cseq[2*i] = outer_branches[0] + Mseq[2*i];
		Cseq[2*i+1] = outer_branches[1] + Mseq[2*i+1];
		// Compute part of MAC 
		accum += Cseq[2*i];
		accum *= working_keys[0];
		accum += Cseq[2*i+1];
		accum *= working_keys[0];
	}
	
	// Compute tag
	p_r(inner_upper_branches);
	T tag = inner_upper_branches[0] + accum;

	return make_pair(Cseq, tag);
}
