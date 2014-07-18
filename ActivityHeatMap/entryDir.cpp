#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <string>

#include <stdlib.h>
#include <stdio.h>

#include "entryDir.h"

using namespace cv;
using namespace std;


entryDir::entryDir ( int argc, char ** argv  ) {
	dir = argv[1];
	dir = "\""+ dir + "\"";
	getDir( dir.c_str(), files);
	for (int i = 0; i < (int) files.size(); i++ )
		files[i].pop_back();

}
void entryDir::selectFilesByType ( char * type ){
	int s = (int)strlen (type);
	for (int i = 0; i < (int) files.size(); i++ ) {
		if ( files[i].compare(files[i].length()-s, s, type) != 0 ) {
			//cout << files[i] << endl;
			files.erase (files.begin()+i);
			i = i-1;
		}
	}
}

void entryDir::print ( ) {
	printf ("\n\n\t entryDir::\n");
	printf ("\ndirectory: %s\n\n", dir.c_str());
	for (int i = 0; i < (int) files.size(); i++ )
		cout<<files[i]<<endl;
}

void getDir(const char* d, vector<string> & f)
{
	FILE* pipe =  NULL;
	string pCmd = "dir /B /S " + string(d);
	char buf[256];

	if( NULL == (pipe = _popen(pCmd.c_str(),"rt")))
	{
		cout<<"Shit"<<endl;
		return;
	}

	while (!feof(pipe))
	{
		if(fgets(buf,256,pipe) != NULL)
		{
			f.push_back(string(buf));
		}

	}

	_pclose(pipe);


}