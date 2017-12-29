/*
 * File.cpp
 *
 *  Created on: 25.5.2010
 *      Author: hth
 */

#include "DebugTools.h"
#include "File.h"

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
	m_chandle = fopen( filename, "rb" );
	
	if ( !m_chandle ) {
		m_chandle = fopen( String( "data/" ) + filename, "rb" );
		if ( !m_chandle ) {
			FERROR( (String( "Couldn't open file: " ) + filename).getCStr() );
		}
	}

	fseek( m_chandle, 0, SEEK_END );
	m_fileSize = ftell( m_chandle );
}

void File::p_close() {
	fclose( m_chandle );
	m_chandle = NULL;
}


