#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "entryDir.h"
#include "../entryOptions.h"
#include "ViBeMat.h"


using namespace cv;
using namespace std;

void getGray ( cv::Mat * frame, cv::Mat * gray, std::vector<cv::Mat> &layers );

int main ( int argc, char ** argv ) {
	
	char * vfile_type = "avi";
		
	//Handling directory (get all avi files in a list)
	entryDir directory (argc, argv);
	directory.selectFilesByType ( vfile_type );
	
	//Create and Set options for videos
	entryOptions options;
	argc = 2;
	
	//Create and Set structures of video analysis
	WStuff ms;
	ms.featVec_options.push_back ("x"); 
	ms.featVec_options.push_back ("y"); 
	ms.setMeasurements ( );

	
	//Create and Set ViBe (background substraction)
	//ViBe          ( n,  r,  c, p, width,height ) {
	ViBe vibe_model ( 20, 20, 8, 16, 640, 480 );
	vibe_model.allocate( );
	vibe_model.printModelparam ( );
	printf ("\nwidth = %d, height = %d", vibe_model.width, vibe_model.height);
	for (int i = 0; i < 480; i++)
		for (int j = 0; j < 640; j++) 
			for (int k = 0; k < 20; k++)
				vibe_model.data[i][j][k] = vibe_model.data[i][j][k]*2;


	Mat gray(ms.frame.cols, ms.frame.rows, CV_8UC1);		
		
	
	// Inner stuff
	bool frameRead;
	vector<Mat> layers;
	//         Mat (nrows, ncols, type)
	Mat videoFgSum ( 480, 640, CV_16UC1 ); // pixel frequency map
	videoFgSum.setTo ( 0 );
	Mat videoFg ( 480, 640, CV_16UC1 ); // frame in 16U int to 
			
	
	//Iterate video files through file list
	for (int ifile = 0; ifile < (int) directory.files.size(); ifile++) {
		argv[1] = (char *) directory.files[ifile].c_str();
		setOptions ( &options, argc, argv );
				
		cout <<"\nAnalyse File : "<< options.videoFilename <<endl;
		ms.setWStuff ( &options );

		// Primeira rodada para convergir o vibe
		for (;;) {
			frameRead = ms.vfile.read ( ms.frame );
			if (!frameRead)
				break;
		    			
		    getGray ( &ms.frame, &gray, layers );
			
			// mandar para o Vibe 
			if (ms.frameCount == 0)
				vibe_model.initializeModel3 ( &gray );
			else
				vibe_model.timeToViBe ( &gray, &ms.binFrame );
						
			imshow (ms.windowName, ms.binFrame);
			 
			char key = cv::waitKey(1);
			if ( key == 27 ) break;	
			else if ( key == 32 ) waitKey(0);

			ms.frameCount++;			
		}
		
		// resetar leitura
		ms.frameCount = 0;
		ms.vfile.release ( );
		ms.openVideo ( options.videoFilename );
		
		// Segunda Rodada, para análise
		for (;;) {
			frameRead = ms.vfile.read ( ms.frame );
			if (!frameRead)
				break;
		    
			
			getGray ( &ms.frame, &gray, layers );
			
			vibe_model.timeToViBe ( &gray, &ms.binFrame );
			
			imshow (ms.windowName, ms.binFrame);
			char key = cv::waitKey(1);
			if ( key == 27 ) break;	
			else if ( key == 32 ) waitKey(0);

			ms.frameCount++;

			threshold( ms.binFrame, ms.binFrame, 1, 1, THRESH_BINARY);
			ms.binFrame.convertTo ( videoFg, CV_16U );
			videoFgSum = videoFgSum + videoFg;
		}

		//Criar Arquivo de Resultados
		int s = (int)strlen ( vfile_type );
		directory.files[ifile].replace(directory.files[ifile].length()-s, s, "csv");
		cout <<"\nCreate Results File: "<< directory.files[ifile] <<endl;
		ms.openResultsFile ( (char *) directory.files[ifile].c_str() );
		for (int i = 0; i < videoFgSum.rows; i++){
			unsigned short * line_ptr = videoFgSum.ptr<unsigned short>(i);
			for (int j = 0; j < videoFgSum.cols; j++) {
				fprintf ( ms.resultsFH, "%d; ", line_ptr[j] );
			}
			fprintf ( ms.resultsFH, "\n");
		}
		
			
	
	} 
	
	vibe_model.releaseVibe( );
	ms.vfile.release( );
	
	
	printf ("\n\n");
	return 0;
}

void getGray ( cv::Mat * frame, cv::Mat * gray, std::vector<cv::Mat> &layers ) {
	cvtColor ( *frame, * frame, CV_BGR2Luv );
	split ( *frame, layers );
	divide ( layers[0], layers[1], *gray, 100, -1 );
}