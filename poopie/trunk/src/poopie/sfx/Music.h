
#ifndef __POOPIE_SFX_MUSIC
#define __POOPIE_SFX_MUSIC

namespace poopie {
namespace sfx {

	class Music {
	public:

		//Music( const void * data, int dataSize );

		static Music * create( void * data, int dataSize );
		virtual void play() = 0;
		virtual int getTime() = 0;
		virtual void setTime( unsigned int t ) = 0;
		virtual float * getSpectrum( int size ) = 0;

	};

}
}

#endif
