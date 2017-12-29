/*
 * File.h
 *
 *  Created on: 25.5.2010
 *      Author: hth
 */

#ifndef FILE_H_
#define FILE_H_

#include <stdio.h>

#include "containers/String.h"

class File {

	FILE * m_chandle;
	char * m_data;

	int m_fileSize;

	void p_open( const String & filename );
	void p_close();
	void p_readData();
	void p_freeData();

public:

	File( const String & filename ) : m_chandle(NULL), m_data(NULL) { p_open( filename ); p_readData(); };
	~File() { p_close(); p_freeData(); }

	const char * getData() const { return m_data; };
	int getDataSize() const { return m_fileSize; };

	bool isOpen() const { return m_chandle != 0; }

};


#endif /* FILE_H_ */
