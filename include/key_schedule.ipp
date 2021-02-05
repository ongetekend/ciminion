#pragma once

template <class T>
KeySchedule<T>::KeySchedule(IteratedTransformation<T> &p_n) : p_n(p_n) {}

template <class T>
Vec<T> KeySchedule<T>::operator()(const Vec<T> &key_state, const long n) const
{
	Vec<T> keys;
	keys.SetLength(n);
	Vec<T> key_state2 = key_state;
	for (long i = 0; i < n; i++) {
		p_n(key_state2);
		keys[i] = key_state2[0];
	}

	return keys;
}
