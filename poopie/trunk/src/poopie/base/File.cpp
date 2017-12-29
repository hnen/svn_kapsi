
#include "poopie/defs.h"
#include "poopie/base/File.h"


namespace poopie {

void File::p_readData() {
	fseek( m_chandle, 0, SEEK_SET );

	if ( m_data ) {
		p_freeData();
	}
	m_data = new char[m_fileSize];

	fread( m_data, 1, m_fileSize, m_chandle );
}

void File::p_freeData() {
	if ( !m_data ) {
		delete [] m_data;
	}
}

void File::p_open( const String & filename ) {
	POOP_INTERNAL_LOG( String("Opening file ") + filename );
	m_chandle = fopen( filename, "rb" );
	
	if ( !m_chandle ) {
		String nname = String( "data/" ) + filename;
		m_chandle = fopen( nname, "rb" );
		if ( !m_chandle ) {
			POOP_ERROR( (String( "Couldn't open file: " ) + filename).getCStr() );
		}
	}

	fseek( m_chandle, 0, SEEK_END );
	m_fileSize = ftell( m_chandle );
}

void File::p_close() {
	fclose( m_chandle );
	m_chandle = NULL;
}

}

