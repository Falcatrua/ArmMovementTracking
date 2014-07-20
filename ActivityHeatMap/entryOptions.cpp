#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include "entryOptions.h"

using namespace cv;
using namespace std;

entryOptions::entryOptions ( ) {
	videoFilename = NULL;
	ArenaMaskFilename = NULL;
	resultsFilename = NULL;
	
	videoFile = NULL;
	printResults = false;
	ArenaMaskFile = false;

	int fps = NULL;
	int totalFrames = NULL;
}
void entryOptions::setVideoFilename ( char * filename ) {
	videoFilename = filename;
	videoFile = true;
}
void entryOptions::setArenaMaskFilename ( char * mask_filename ) {
	ArenaMaskFilename = mask_filename;
	ArenaMaskFile = true;
}
void entryOptions::setResultsFilename ( char * results_filename ) {
	resultsFilename = results_filename;
	printResults = true;
}

void setOptions ( entryOptions * Eoptions, int argc, char ** argv ) {
	/* set defaults */
	if ( argc < 2 ) {
		printMenu (argv[0]);
		exit ( 0 );
	}
	else {
		Eoptions->setVideoFilename ( argv[1] );
		int i;
		for (i = 2; i < argc; i++) {
			if (!(strcmp (argv[i], "-mask")))
				Eoptions->setArenaMaskFilename ( argv[i+1] );
			else if (!(strcmp (argv[i], "-results")))
				Eoptions->setResultsFilename ( argv[i+1] );
			
		}
	}
	
}
void entryOptions::printOptions ( )  {
	printf ("\n\n--- Entry Options ---" );
	printf ("\n  -input video: %s", videoFilename );
	if ( ArenaMaskFile )
		printf ("\n  -arena mask: %s", ArenaMaskFilename );
	if ( printResults )
		printf ("\n  -results output: %s", resultsFilename);
	else 
		printf ("\n Warning: no output file for results");

	printf ("\n---\n");
}

void printMenu ( char * programName ){
	printf ("\n\n %s \n"
		"\n    %s   <videoFilename> [[-option] [-option value]]"
		"\n\nOptions:"
		"\n -results        name of the output results file"
		"\n -mask           black and white image of the arena (analysis area)"
		"\n\n", programName, programName);
}
void vectorize_moments ( Moments * mo, double * vmo ) {
	vmo[0] = mo->m00;
	vmo[1] = mo->m10;
	vmo[2] = mo->m01;
	vmo[3] = mo->m20;
	vmo[4] = mo->m11;
    vmo[5] = mo->m02;
	vmo[6] = mo->m30;
	vmo[7] = mo->m21;
	vmo[8] = mo->m12;
	vmo[9] = mo->m03;
	vmo[10] = mo->mu20;
	vmo[11] = mo->mu11;
	vmo[12] = mo->mu02;
	vmo[13] = mo->mu30;
	vmo[14] = mo->mu21;
	vmo[15] = mo->mu12;
	vmo[16] = mo->mu03;
	vmo[17] = mo->nu20;
	vmo[18] = mo->nu11;
	vmo[19] = mo->nu02;
	vmo[20] = mo->nu30;
	vmo[21] = mo->nu21;
	vmo[22] = mo->nu12;
	vmo[23] = mo->nu03;
}


WStuff::WStuff ( ) {
	
	frame = NULL;
	ArenaMask = NULL;
	measurements = NULL;
		
	frameCount = 0;
	resultsFH = NULL;

	windowName = "Video Analysis";
	
}

void WStuff::setWStuff ( entryOptions * options ) {
	frame = NULL;
	ArenaMask = NULL;
	measurements = NULL;
		
	frameCount = 0;
	resultsFH = NULL;
	

	windowName = "Video Analysis";
	openVideo ( options->videoFilename );
	namedWindow ( windowName );
	binFrame.create (videoSize.height, videoSize.width, CV_8UC1);
		
	if ( options->ArenaMaskFile )
		setArenaMask ( options->ArenaMaskFilename );

	setArenaLimits ( options );


}
void WStuff::openVideo ( char * filename ) {
	vfile.open ( filename );
	if (!vfile.isOpened ( ) )
		printf ("\nERROR: could not open %s\n", filename);
	else {
		videoSize.height = (int) vfile.get ( CV_CAP_PROP_FRAME_HEIGHT );
		videoSize.width = (int) vfile.get ( CV_CAP_PROP_FRAME_WIDTH );
		fps = (int) vfile.get ( CV_CAP_PROP_FPS );
		totalFrames = (int) vfile.get ( CV_CAP_PROP_FRAME_COUNT );
	}
}
void WStuff::setArenaMask ( char * filename ) {
	ArenaMask = imread ( filename );
	threshold ( ArenaMask, ArenaMask, 125, 255, CV_THRESH_BINARY );
}
void WStuff::openResultsFile ( char * filename ) {
	 resultsFH = fopen (filename,"w");
	 if ( resultsFH == NULL ) {
		 printf ("\nERROR: could not open output results file \"%s\"", filename );
	 }
}
void WStuff::setArenaLimits ( entryOptions * options ) {
	if ( options->ArenaMaskFile ) {
		int nl= ArenaMask.rows; // number of lines
		int nc= ArenaMask.cols ; // number of columns
		ArenaLimits.x_ini = nc;
		ArenaLimits.x_end = 0;
		ArenaLimits.y_ini = nl;
		ArenaLimits.y_end = 0;
		int x, y;
		for (y = 0; y < nl; y++ ) {
			uchar * data = ArenaMask.ptr<uchar>(y);
			for (x = 0; x < nc; x++) {
				if ( data[x] == (uchar) 0 ) {
					if ( x > ArenaLimits.x_end )
						ArenaLimits.x_end = x;
					if ( x < ArenaLimits.x_ini )
						ArenaLimits.x_ini = x;
					if ( y > ArenaLimits.y_end )
						ArenaLimits.y_end = y;
					if ( y < ArenaLimits.y_ini )
						ArenaLimits.y_end = y;
				}
			}
		}
	}
	else {
		ArenaLimits.x_ini = 0;
		ArenaLimits.y_ini = 0;
		ArenaLimits.x_end = videoSize.width;
		ArenaLimits.y_end = videoSize.height;
	}
}
void WStuff::getBinaryFrame ( char * method ) {
	if (!(strcmp (method, "simple"))) {
		threshold ( frame, binFrame, 50, 255, CV_THRESH_BINARY );
		std::vector<cv::Mat> planes;
		split (binFrame, planes);
		binFrame = planes[0];
	}
}
void WStuff::setMeasurements ( ) {
	int feats = (int) featVec_options.size( );
	features.resize (feats, vector<int>(3,0));
		
	// encoding features to get
	// Features -> opencv itseez documentation >> structural analysis and shape descriptors
	//   (features types numbered in correspondence to the order in the doc site )
	// features = feats rows, 3 columns
	//   
	//    column 0: type
	//		1 = moments -> moments (...) (24 elements)
	//      2 = hu invariant -> HuMoments (...) (7 elements)
	//      3 = perimeter -> arcLength ( ... ) (single element)
	//      4 = area -> contourArea ( ... ) (single element)
	//      5 = minimum-area bounding rectangle (possibly rotated) -> minAreaRect ( ... )
	//           (3 elements: 0= width, 1= length, 2=angle)
	//      0 = position ->( calculated based on moments x =(m10 /m00), y =(m01/m00) );
	//			 (2 elements: 0= x, 1= y)
	//
	//    column 1: element
	//		example:
	//		   1 = (if type == 1) -> first element of moments >> m00
	//
	//    column 2: position index in featureVec_options
	//       example:
	//         if (featVec_options[f].compare ( "hu5" ) == 0) {
	//				features[f][0] = 2;
	//              features[f][1] = 5;
	//              features[f][2] = f;
	//			}
	for ( int f = 0; f < feats; f++ ) {
		features[f][2] = f;
		if ( featVec_options[f].compare ( 0, 7,"moment_") == 0 ) {
			features[f][0] = 1;
			if ( featVec_options[f].compare ( 7, 3,"m00") == 0 ) features[f][1] = 0;
			else if ( featVec_options[f].compare ( 7, 3,"m10") == 0 ) features[f][1] = 1;
			else if ( featVec_options[f].compare ( 7, 3,"m01") == 0 ) features[f][1] = 2;
			else if ( featVec_options[f].compare ( 7, 3,"m20") == 0 ) features[f][1] = 3;
			else if ( featVec_options[f].compare ( 7, 3,"m11") == 0 ) features[f][1] = 4;
			else if ( featVec_options[f].compare ( 7, 3,"m02") == 0 ) features[f][1] = 5;
			else if ( featVec_options[f].compare ( 7, 3,"m30") == 0 ) features[f][1] = 6;
			else if ( featVec_options[f].compare ( 7, 3,"m21") == 0 ) features[f][1] = 7;
			else if ( featVec_options[f].compare ( 7, 3,"m12") == 0 ) features[f][1] = 8;
			else if ( featVec_options[f].compare ( 7, 3,"m03") == 0 ) features[f][1] = 9;
			else if ( featVec_options[f].compare ( 7, 4,"mu20") == 0 ) features[f][1] = 10;
			else if ( featVec_options[f].compare ( 7, 4,"mu11") == 0 ) features[f][1] = 11;
			else if ( featVec_options[f].compare ( 7, 4,"mu02") == 0 ) features[f][1] = 12;
			else if ( featVec_options[f].compare ( 7, 4,"mu30") == 0 ) features[f][1] = 13;
			else if ( featVec_options[f].compare ( 7, 4,"mu21") == 0 ) features[f][1] = 14;
			else if ( featVec_options[f].compare ( 7, 4,"mu12") == 0 ) features[f][1] = 15;
			else if ( featVec_options[f].compare ( 7, 4,"mu03") == 0 ) features[f][1] = 16;
			else if ( featVec_options[f].compare ( 7, 4,"nu20") == 0 ) features[f][1] = 17;
			else if ( featVec_options[f].compare ( 7, 4,"nu11") == 0 ) features[f][1] = 18;
			else if ( featVec_options[f].compare ( 7, 4,"nu02") == 0 ) features[f][1] = 19;
			else if ( featVec_options[f].compare ( 7, 4,"nu30") == 0 ) features[f][1] = 20;
			else if ( featVec_options[f].compare ( 7, 4,"nu21") == 0 ) features[f][1] = 21;
			else if ( featVec_options[f].compare ( 7, 4,"nu12") == 0 ) features[f][1] = 22;
			else if ( featVec_options[f].compare ( 7, 4,"nu03") == 0 ) features[f][1] = 23;
		}
		else if ( featVec_options[f].compare ( 0, 2,"hu") == 0 ) {
			features[f][0] = 2;
			if ( featVec_options[f].compare ( 2, 1,"0") == 0 ) features[f][1] = 0;
			else if ( featVec_options[f].compare ( 2, 1,"1") == 0 ) features[f][1] = 1;
			else if ( featVec_options[f].compare ( 2, 1,"2") == 0 ) features[f][1] = 2;
			else if ( featVec_options[f].compare ( 2, 1,"3") == 0 ) features[f][1] = 3;
			else if ( featVec_options[f].compare ( 2, 1,"4") == 0 ) features[f][1] = 4;
			else if ( featVec_options[f].compare ( 2, 1,"5") == 0 ) features[f][1] = 5;
			else if ( featVec_options[f].compare ( 2, 1,"6") == 0 ) features[f][1] = 6;		
		}
		else if ( featVec_options[f].compare ("perimeter") == 0 )
			features[f][0] = 3;
		else if ( featVec_options[f].compare ("area") == 0 )
			features[f][0] = 4;
		else if ( featVec_options[f].compare ( 0, 5,"rect_") == 0 ) {
			features[f][0] = 5;
			if ( featVec_options[f].compare ( 5, 5,"width") == 0 ) features[f][1] = 0;
			else if ( featVec_options[f].compare ( 5, 6,"length") == 0 ) features[f][1] = 1;
			else if ( featVec_options[f].compare ( 5, 6,"angle") == 0 ) features[f][1] = 2;
		}
		else if ( featVec_options[f].compare ("x") == 0 ) {
			features[f][0] = 0;
			features[f][1] = 0;
		}
		else if ( featVec_options[f].compare ("y") == 0 ) {
			features[f][0] = 0;
			features[f][1] = 1;
		}
	}
	printf ("\n");
	printSTLMat_int ( features );
	m = 0;
}
void WStuff::getMeasurements ( ) {
	int feats = (int) featVec_options.size( );
	int conts = (int) contours.size ( );
	
	if ( conts > 0 ) {
		measurements.create ( conts, feats, CV_32FC1 );
		measurements.setTo ( 0, noArray() );

		Moments mo;
		double mo_v[24];
		double hu[7];
		double perimeter;
		double area;
		RotatedRect box;
	
		bool b_moments = false;
		bool b_hu = false;
		bool b_perimeter = false;
		bool b_area = false;
		bool b_rect = false;
	
		for (int f = 0; f < feats; f++) {
			if ( features[f][0] == 1||0||2 ) 
				b_moments = true;
			if ( features[f][0] == 2 )
				b_hu = true;
			if ( features[f][0] == 3 )
				b_perimeter = true;
			if ( features[f][0] == 4 )
				b_area = true;
			if ( features[f][0] == 5 )
				b_rect = true;
		}
		for (int c = 0; c < conts; c++) {
			if ( b_moments ) {
				mo = moments ( contours[c], false );
				vectorize_moments ( &mo, mo_v );
			}
			if ( b_hu )
				HuMoments ( mo, hu );
			if ( b_perimeter )
				perimeter = arcLength ( contours[c], true );
			if ( b_area )
				area =  contourArea ( contours[c], false );
			if ( b_rect )
				box = minAreaRect ( contours[c] );
			
			for (int f = 0; f < feats; f++){
				if (features[f][0] == 0) {
					if (features[f][1] == 0)
						measurements.at<float>(c, features[f][2]) = (float) (mo.m10 / mo.m00);
					else if (features[f][1] == 1)
						measurements.at<float>(c, features[f][2]) = (float) (mo.m01 / mo.m00);
				}
				else if (features[f][0] == 1) 
					measurements.at<float>(c, features[f][2]) = (float) mo_v[features[f][1]];
				else if (features[f][0] == 2)
					measurements.at<float>(c, features[f][2]) = (float) hu[features[f][1]];
				else if (features[f][0] == 3)
					measurements.at<float>(c, features[f][2]) = (float) perimeter;
				else if (features[f][0] == 4)
					measurements.at<float>(c, features[f][2]) = (float) area;
				else if (features[f][0] == 5) {
					if (features[f][1] == 0)
						measurements.at<float>(c, features[f][2]) = (float) box.size.width;
					else if (features[f][1] == 1)
						measurements.at<float>(c, features[f][2]) = (float) box.size.height;
					else if (features[f][1] == 2)
						measurements.at<float>(c, features[f][2]) = (float) box.angle;
				}
			}
		}
	}
	m = conts;

}