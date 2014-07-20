#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgproc/imgproc.hpp>

#include "export2video.h"

using namespace cv;
using namespace std;

exportedVideo::exportedVideo ( ) {
	videoFilename = NULL;
	fps = NULL;
	compression = NULL;
	on = false;
	videoFile = false;
}

exportedVideo::exportedVideo ( char * filename, int compression_option, int Fps, int resolution_option ){
	videoFilename = filename;
	fps = (double) Fps;
	compression = compression_option;
	setComp ();
	resolution = resolution_option;
	setRes ();

	on = true;
	videoFile = true;
	video.open( videoFilename, comp, fps, res, true);
	if (!video.isOpened()){
        printf ("\nNao foi possivel abrir o video %s para gravacao", videoFilename);
		videoFile = false;        
		on = false;
    }


}

void exportedVideo::setComp ( ) {
	switch (compression) {
		case 0:
			comp = 0;
			break;
		case 1:
			comp = CV_FOURCC('P','I','M','1');
			break;
		case 2:
			comp = CV_FOURCC('M','P','4','2'); //MPEG-4.2 codec 
			break;
		case 3:
			comp = CV_FOURCC('D','I','V','3'); //MPEG-4.3 codec
			break;
		case 4:
			comp = CV_FOURCC('D','I','V','X'); //MPEG-4 codec 
			break;
		case 5:
			comp = CV_FOURCC('U','2','6','3'); //H263 codec
			break;
		case 6:
			comp = CV_FOURCC('I','2','6','3'); //H263I codec 
			break;
		case 7:
			comp = CV_FOURCC('F','L','V','1'); //FLV1 codec
			break;
	}
}
void exportedVideo::setRes ( ) {
	switch (resolution) {
		case 0:
			res.width = 640;
			res.height = 480;
			break;
		case 1:
			res.width = 800;
			res.height = 448;
			break;
		case 2:
			res.width = 960;
			res.height = 544;
			break;
		case 3:
			res.width = 1280;
			res.height = 720;
			break;
	}
}
void printMenu ( ){
	printf ("\n\n "
		"[Class] export2video::\n "	
		"\n (char * filename, int compression_option, int Fps, int resolution_option) \n"
		"\n -fps            frame rate (per second) (default = 30)"
		"\n -compression    0 - raw (default)"
		"\n                 1 - MPEG-1 codec" 
		"\n                 2 - MPEG-4.2 codec"
		"\n                 3 - MPEG-4.3 codec" 
		"\n                 4 - MPEG-4 codec"
		"\n                 5 - H263 codec" 
		"\n                 6 - H263I codec"
		"\n                 7 - FLV1 codec"
		"\n"
		"\n -resolution     0 - 640 x 480 (default)"
		"\n                 1 - 800 x 448"
		"\n                 2 - 960 x 544"
		"\n                 3 - 1280 x 720"
		"\n\n");
}