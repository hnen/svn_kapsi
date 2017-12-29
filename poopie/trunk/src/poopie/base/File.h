#ifndef __POOPIE_FILE_H_
#define __POOPIE_FILE_H_

#include <stdio.h>
#include "poopie/base/String.h"


namespace poopie {

class File {
public:

	File( const String & filename ) : m_chandle(NULL), m_data(NULL) { p_open( filename ); p_readData(); };
	~File() { p_close(); p_freeData(); }

	//const char * getData() const { return m_data; };
	char * getData() const { return m_data; };
	int getDataSize() const { return m_fileSize; };

	bool isOpen() const { return m_chandle != 0; }

private:

	FILE * m_chandle;
	char * m_data;

	int m_fileSize;

	void p_open( const String & filename );
	void p_close();
	void p_readData();
	void p_freeData();

};

}

#endif /* FILE_H_ */
