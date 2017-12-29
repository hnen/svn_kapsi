
//#define FMOD_DISABLE

#include <fmod.hpp>
#include <fmod_errors.h>
#include <Windows.h>

#include "poopie/sfx/Music.h"
#include "poopie/defs.h"
#include "poopie/base/String.h"

namespace poopie {
namespace sfx {
namespace fmod {

	FMOD::System * system = NULL;

	void initSystem() {
#ifndef FMOD_DISABLE
		if ( !system ) {
			if ( FMOD::System_Create(&system) != FMOD_OK ) {
				POOP_ERROR( "Error initializing FMod :[" );
			}

			FMOD_RESULT rslt;
			if ( (rslt = system->init( 100, FMOD_INIT_NORMAL, 0 )) != FMOD_OK ) {
				//POOP_ERROR( (String( "Error initializing fmod: " ) + String(FMOD_ErrorString(rslt))).getCStr() );
				system = NULL;
			}
		}
#endif
	}

	class FmodMusic : public poopie::sfx::Music {
	public:
		FmodMusic( void * data, int dataSize ) {
#ifndef FMOD_DISABLE
			initSystem();

			if ( !system ) return;

			m_fmodsound = 0;

			FMOD_CREATESOUNDEXINFO info;
			memset( &info, 0, sizeof(info) );
			info.length = dataSize;
			info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
			
			FMOD_RESULT reslt;
			if ( (reslt = system->createStream( (char*)data, FMOD_DEFAULT | FMOD_OPENMEMORY, &info, &m_fmodsound )) != FMOD_OK ) {
				POOP_ERROR( (String("Error opening music, reason: ") + String(FMOD_ErrorString(reslt))).getCStr() );
			}
#endif
		}

		void play() {
#ifndef FMOD_DISABLE
			if ( system ) {
				m_channel = 0;
				system->playSound( FMOD_CHANNEL_FREE, m_fmodsound, false, &m_channel );
			} else {
				m_startTime = timeGetTime();
			}
#else
			m_startTime = timeGetTime();
#endif
		}

		float * getSpectrum( int size ) { 
			float * arr = new float[size];
			m_channel->getSpectrum( arr, size, 0, FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS );
			return arr;
		}

		int getTime() {
#ifndef FMOD_DISABLE
			if ( system ) {
				unsigned int t;
				m_channel->getPosition( &t, FMOD_TIMEUNIT_MS );
				return t;
			} else {
				return timeGetTime() - m_startTime;
			}
#else
			return timeGetTime() - m_startTime;
#endif
		}

		void setTime( unsigned int t ) {
#ifndef FMOD_DISABLE
			if ( system ) {
				m_channel->setPosition( t, FMOD_TIMEUNIT_MS );
			} else {
				m_startTime = timeGetTime() - t;
			}
#else
			m_startTime = timeGetTime() - t;
#endif
		}
	
	private:
		FMOD::Channel * m_channel;
		FMOD::Sound * m_fmodsound;
		unsigned int m_startTime;
	};


}

	Music * Music::create( void * data, int dataSize ) {
		return new fmod::FmodMusic( data, dataSize );
	}

}
}