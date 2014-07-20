#ifndef ENTRYOPTIONS_H
#define ENTRYOPTIONS_H

class entryOptions {
public:
		char * videoFilename;
		char * ArenaMaskFilename;
		char * resultsFilename;
		
		bool videoFile;
		bool printResults;
		bool ArenaMaskFile;

		entryOptions ( );
		void setVideoFilename ( char * filename );
		void setArenaMaskFilename ( char * mask_filename );
		void setResultsFilename ( char * results_filename );
		void printOptions ( );
		
};

void setOptions ( entryOptions * Eoptions, int argc, char ** argv );
void printMenu ( char * programName );
void vectorize_moments ( cv::Moments * mo, double * vmo );

class WStuff {
public:
	cv::VideoCapture vfile;
	cv::Size videoSize;
	int fps;
	int totalFrames;

	cv::Mat frame;
	cv::Mat binFrame;
	cv::Mat ArenaMask;
	
	int frameCount;
	FILE * resultsFH;
	
	struct ArenaLimits {
		int x_ini;
		int x_end;
		int y_ini;
		int y_end;
	}ArenaLimits;

	std::vector <std::vector<cv::Point>> contours;
	std::vector <std::string> featVec_options;
	std::vector <std::vector <int>> features;
	std::vector<cv::Mat> planes;
	cv::Mat measurements;
	int m;
	char * windowName;
		
	WStuff ( );
	void setWStuff ( entryOptions * options );
	void openVideo ( char * filename );
	void setArenaMask ( char * filename );
	void openResultsFile ( char * filename );
	void setArenaLimits ( entryOptions * options );
	void getBinaryFrame ( char * method );
	void setMeasurements( );
	void getMeasurements( );
	
};

#endif


		