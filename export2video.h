#ifndef EXPORT2VIDEO_H
#define EXPORT2VIDEO_H

class exportedVideo {
	public:
		//estruturais
		char * videoFilename;
		cv::VideoWriter video;
		
		//opções com valores default
		double fps;
		int compression;
		int resolution;
		cv::Size res;
		int comp;

		bool videoFile;
		bool on;

		exportedVideo ( char * filename, int compression_option, int Fps, int resolution_option  );
		exportedVideo ( );
		void setRes ( );
		void setComp ( );
		void printMenu( );
		
};

#endif