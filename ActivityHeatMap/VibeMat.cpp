#include <opencv2/core/core.hpp>        
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include "ViBeMat.h"

using namespace tbb;
using namespace cv;
using namespace std;

ViBe::ViBe ( int in_n, int in_radius, int in_cmin, int in_phi, int in_width, int in_height ) {
	 n = in_n;
     radius = in_radius;
     cmin = in_cmin;
     phi = in_phi;
     bg = 0;
     fg = 255;
     width = in_width;
     height = in_height;
     data = NULL;
}
	
void ViBe::timeToViBe ( cv::Mat * gray, cv::Mat * map ) {
	int Iwidth, Iheight, x, y;
	Iwidth = gray->cols;
    Iheight = gray->rows;
    uchar foreground = (uchar) fg;
    uchar background = (uchar) bg;
	
	for ( y = 0; y < Iheight; y++ ) {
		uchar * line_ptr = (uchar *) gray->ptr<uchar>(y);
		uchar * map_ptr = (uchar *) map->ptr<uchar>(y);
        //uchar * line_ptr = (uchar *) (gray->rowsimageData + y*gray->rowswidthStep);
        //uchar * map_ptr = (uchar *) (map->rowsimageData + y*map->rowswidthStep);
        for ( x = 0; x < Iwidth; x++ ) {    
            int count = 0;
            int index = 0;
            int dist = 0;
            while ((count < cmin) && (index < n)){
				int dist = euclideanDist ( line_ptr[x], data[y][x][index]);
				if ( dist < radius ) {
					count = count + 1;
				}
				index = index+1;
			}
			if (count >= cmin){
				map_ptr[x] = background;
				int rand = unirand (0, phi-1 );
				if (rand == 0){
					int r = unirand ( 0, n-1 );
					data[y][x][r] = line_ptr[x];
				}
				rand = unirand ( 0, phi-1 );
                if (rand == 0){					
				   int xng = -1;
                   int yng = -1;
                   while (xng < 0  || xng >= width )
						 xng = randNeighbourCoord ( x );
				   while (yng < 0 || yng >= height )
                         yng = randNeighbourCoord ( y );
                   int r = unirand ( 0, n-1 );
				   data[yng][xng][r] = line_ptr[x];
				}
             }
             else {
                  map_ptr[x] = foreground;
             }
         }
     }

}

void ViBe::iniModel ( cv::Mat * gray, int i ) {
	 int Iwidth, Iheight, x, y;
     Iwidth = gray->cols;
     Iheight = gray->rows;
      
     for ( y = 0; y < Iheight; y++ ) {
		 uchar * line_ptr = gray->ptr<uchar>(y);
         //uchar * line_ptr = (uchar *) (gray->rowsimageData + y*gray->rowswidthStep);
         for ( x = 0; x < Iwidth; x++ ) {
             data[y][x][i] = line_ptr[x];
         }
     }
    
}

void ViBe::printModelparam ( ) {
	 printf ("\n\nViBe model::");
     printf ("\n  n = %d", n);
     printf ("\n  radius = %d", radius);
     printf ("\n  cmin = %d", cmin);
     printf ("\n  phi = %d", phi);
     printf ("\n");
}

int ViBe::randNeighbourCoord ( int x ) {
	int xrand = unirand ( -1, 1 );
	return ( x + xrand );
}
int ViBe::unirand ( int a, int b ) {
	 return ( a+ (int) ((b-a)*((double)rand()/(double)RAND_MAX)) );
}
int ViBe::euclideanDist ( uchar im_v, uchar s_v ) {
	int value = (int) im_v;
    int sample = (int) s_v;
    return ( (value-sample)>=0?(value-sample):-(value-sample) );
}
void ViBe::testModel ( ) {
	 int x, y, i;
     uchar last = (uchar) 0;
     for ( y = 0; y < height; y++ ) {
         for ( x = 0; x < width; x++ ) {
             for ( i = 0; i < n; i++ ) {
                 euclideanDist ( data[y][x][i], last );
                 last = data[y][x][i];
             }
         }
     }
}
void ViBe::initializeModel2 ( cv::Mat * frame0 ) {
	int Iwidth, Iheight, x, y, k;
	Iwidth = frame0->cols;
    Iheight = frame0->rows;
    for ( y = 0; y < Iheight; y++ ) {
        for ( x = 0; x < Iwidth; x++ ) {
            for ( k = 0; k < n; k++ ) {
                int xng = -1;
                int yng = -1;
                while (xng < 0 || xng >= width )
                      xng = randNeighbourCoord ( x );
                while (yng < 0 || yng >= height )
                      yng = randNeighbourCoord ( y );
                uchar * line_ptr = frame0->ptr<uchar>(y);
				//uchar * line_ptr = (uchar *) (frame0->rowsimageData + yng*frame0->rowswidthStep);
                data[y][x][k] = line_ptr[xng];
            }
        }
    }
}
void ViBe::initializeModel1 ( cv::Mat * frame, cv::Mat * gray, cv::VideoCapture capture ) {
	int i;
    for (i = 0; i < n; i++) {
        capture.read ( *frame );
        if( !frame ) break;
        cvCvtColor ( frame, gray, CV_BGR2GRAY );
        iniModel ( gray, i );
    }
}

void ViBe::initializeModel3 ( cv::Mat * frame0 ) {
	int Iwidth, Iheight, x, y, k;
    Iwidth = frame0->cols;
    Iheight = frame0->rows;
	for ( y = 0; y < Iheight; y++ ) {
		for ( x = 0; x < Iwidth; x++ ) {
			for ( k = 0; k < n; k++ ) {
				int xng = -1;
                int yng = -1;
                while (xng < 0 || xng > width )
					xng = ImageRandCoord ( frame0->cols );				      
                while (yng < 0 || yng > height )
					yng = ImageRandCoord ( frame0->rows );
				//uchar * line_ptr = (uchar *) (frame0->rowsimageData + yng*frame0->rowswidthStep);
				uchar * line_ptr = frame0->ptr<uchar>(y);
                data[y][x][k] = line_ptr[xng];
            }
        }
    }
}
int ViBe::ImageRandCoord ( int imgDimSize ) {
	int xrand = unirand ( 0, imgDimSize );
	return ( xrand );
}

void ViBe::allocate ( ) {
	int i = 0, j = 0, k = 0;
    data = (uchar ***) malloc(height*sizeof(uchar **));
    for (i = 0; i < height; i++) {
        data[i] = (uchar**) malloc(width*sizeof(uchar**));
        for (j = 0; j < width; j++) {
            data[i][j] = (uchar*) malloc(n*sizeof(uchar));
            for ( k = 0;  k < n; k++) {
                data[i][j][k] = (uchar) 0;
            }
        }
    }
	
}

void ViBe::releaseVibe ( ) {
	size_t i, j;
	for (i = 0; i < height; i++) {
		if (data[i] != NULL) {
			for (j = 0; j < width; j++){
				free ( data[i][j] );
			}
			free (data[i]);
        }
    }
    free(data);
    data = NULL;
}