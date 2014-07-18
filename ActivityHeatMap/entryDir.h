#ifndef ENTRYDIR_H
#define ENTRYDIR_H

class entryDir {
public:
		std::string dir;
		std::vector<std::string> files;
	    
		entryDir ( int argc, char ** argv );
		void selectFilesByType ( char * type );
		void print ();
		
};

void getDir (const char* d, std::vector<std::string> & f);


#endif