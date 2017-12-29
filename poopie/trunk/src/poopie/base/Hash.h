
#ifndef __POOPIE_BASE_HASH
#define __POOPIE_BASE_HASH

namespace poopie {

	class Hash {
	public:
		static unsigned int superFastHash( const void * data, int len );

		static float rnd( int n, float min = -1.0f, float max = 1.0f ) {
			unsigned int N = n * 304243;
			unsigned int r = superFastHash(&N, 4); 
			return ((float)(r % (unsigned int)((max-min) * 100000))) / 100000.0f + min;
		}


	};

	// Generalized hash function
	template<class T> unsigned int poopie_hash( const T & t) { return Hash::superFastHash( &t, sizeof(T) ); }
	//template<> unsigned int poopie_hash<int>( const int & i );

}

#endif

