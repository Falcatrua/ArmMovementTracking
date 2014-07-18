#ifndef VIBEMAT_H
#define VIBEMAT_H

class ViBe {
public:
        int n;
        int radius;
        int cmin;
        int phi;
        int bg;
        int fg;
        int width;
        int height;
        uchar *** data;

		ViBe ( int n, int radius, int cmin, int phi, int width, int height );
		void allocate ( );
		void iniModel ( cv::Mat * gray, int i );
		void timeToViBe ( cv::Mat * gray, cv::Mat * map );
		void printModelparam ( );
		int randNeighbourCoord ( int x );
		int unirand ( int a, int b );
		int euclideanDist ( uchar im_v, uchar s_v );
		void testModel ( );
		void initializeModel2 ( cv::Mat * frame0 );
		void initializeModel1 ( cv::Mat * frame, cv::Mat * gray, cv::VideoCapture capture );
		void initializeModel3 ( cv::Mat * frame0 );
		int ImageRandCoord ( int imgDimSize );
		void releaseVibe ( );
};

#endif