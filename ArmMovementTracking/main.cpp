#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>

#include "entryOptions.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "export2video.h"

using namespace cv;
using namespace std;

class Partes {
public:
	int n_partes;
	vector<vector<cv::Point>> divContours; //pontos_do_contornos[parte]
	vector<float> dprops; //proporção de cada parte (cumulativo) (fixo)
	vector<cv::Point> dpoints; // pontos de divisão entre as partes (dinâmico)
	vector<cv::Scalar> cores;
	vector<vector<int>> tamanho; // comprimento cumulativo das partes (fixo * comp.Maximo)
	float comprimento_fator;
	vector<Vec4f> fitL;
	vector<RotatedRect> boxes;

	char buffer[10];
	std::string text;

	RNG rng;

	Partes ( int quantidade ) {
		n_partes = quantidade;
		
		dprops.resize ( n_partes + 1 );
		dpoints.resize ( n_partes + 1 );

		divContours.resize( n_partes);

		cores.resize( n_partes ); 
		
		for (int i = 0; i < n_partes; i++ )
			cores[i] = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		
		tamanho.resize( n_partes + 1 );
		for (int i = 0; i < n_partes+1; i++ )
			tamanho[i].resize( 2 );

		fitL.resize ( n_partes );

		boxes.resize ( n_partes );

	}
	void setProporcoes ( std::vector<float> * props ) {
		dprops[0] = 0;
		int i = 0;
		for (i = 0; i < (int) props->size(); i++) 
			dprops[i+1] = (*props)[i];
		dprops[i+1] = 1;
		std::sort (dprops.begin(), dprops.end());
	}
	void setComprimentos ( int maxd, float fator ) {
		comprimento_fator = fator;
		for (int i = 0; i < (int) tamanho.size(); i++) {
			tamanho[i][1] = (int) ( dprops[i] * comprimento_fator * maxd );				
		}		
	}
	void desenharContornos ( Mat * frame ) {
		for (int i = 0; i < (int) divContours.size(); i++) {
			for (int j = 0; j < (int) divContours[i].size(); j++) {
				circle ( *frame, divContours[i][j], 1, cores[i], 1,8,0);
			}		
		}
	}
	void limparContornos ( ){
		for (int i = 0; i < (int) divContours.size(); i++)
			divContours[i].clear();		
	}
	void estimarBoxes( ) {
		for ( int i = 0; i < (int) divContours.size(); i++ ) {
			if ( (int) divContours[i].size() > 0 )
				boxes[i] = minAreaRect ( divContours[i] );
		}		
	}
	void desenharBoxes ( Mat * frame ) {
		Point2f rect_points[4]; 
		for (int i = 0; i < (int) divContours.size(); i++){ 
			if ( (int) divContours[i].size() > 0 ) {
				circle ( * frame, boxes[i].center, 1, cores[i], 2,8,0);
				text = _itoa ( i, buffer, 10);
				putText ( * frame, text, boxes[i].center, FONT_HERSHEY_SCRIPT_SIMPLEX, 2, Scalar(255,200,200), 3, 8 );
				boxes[i].points ( rect_points );
				for( int j = 0; j < 4; j++ )
					line( * frame, rect_points[j], rect_points[(j+1)%4], cores[i], 1, 8 );
			}						
		}
	}
	void imprimirPontos ( ) {
		for (int i = 0; i < (int) divContours.size(); i++){ 
			if ( (int) divContours[i].size() > 0 ) {
				printf ("%f; %f; %f; %f;", boxes[i].center.x, boxes[i].center.y,
					boxes[i].size.height, boxes[i].angle); 
			}
		}
		printf ("\n");
	}
};
	
#define MIN_AREA 25
#define MIN_PROP_PARTE 0.350

void selectContours ( std::vector<std::vector<cv::Point>> * contours, int minArea );
int maxY ( std::vector<std::vector<cv::Point>> * contours );
int distanciaMaxima ( entryOptions * options, WStuff * ms );
void dividirPartes ( entryOptions * options, WStuff * ms, Partes * partes );
int composicao ( int y, Partes * partes );
void calcular_partes ( cv::Rect boundRect, int index_parte, Partes * partes );
void dividir_contorno ( std::vector<cv::Point> * contours, int n, Partes * partes );
void desen_partes ( Mat * frame, int index_partes, Partes * partes );


RNG rng(12345);
int main ( int argc, char ** argv ) {

	entryOptions options;
	setOptions ( &options, argc, argv );
	
	WStuff ms;
	ms.setWStuff ( &options );
		
	exportedVideo exv ( "export.avi", 4, ms.fps, 2);
	
	/* Análise */
	Partes partes (3);
	vector<float> p (2);
	p[0] = (float) 0.265625;
	p[1] = (float) 0.656250;
	partes.setProporcoes ( &p );
	partes.rng(12345);

	// Capturar distância máxima para o cálculo das proporções das partes 
	int maxd = distanciaMaxima ( &options, &ms );
	partes.setComprimentos ( maxd, (float) 0.90 );
		
	
	for (;;) {
		ms.vfile >> ms.frame;
		if ( ms.frame.empty() )
			break;
		
		if ( (int) ms.frame.channels( ) > 1 )
			ms.getBinaryFrame ( "simple" );
			
		findContours ( ms.binFrame, ms.contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
		ms.binFrame.setTo ( 0, noArray( ) );

		dividirPartes ( &options, &ms, &partes );

		imshow ( ms.windowName, ms.frame );
		ms.contours.clear();

		char key = cv::waitKey(1000/ms.fps);
		if ( key == 27 ) break;	
		else if ( key == 32 ) waitKey(0);
		
		exv.video << ms.frame;
	}
	destroyWindow ( ms.windowName );
	printf ("\n");
	return 0;
}

void dividirPartes ( entryOptions * options, WStuff * ms, Partes * partes ) {
	
	if ( (int) ms->contours.size( ) > 0 ) {
		// eliminar objetos com area < 50
		selectContours ( &(ms->contours), MIN_AREA );
		//drawContours ( ms->frame, ms->contours, -1, cv::Scalar(0,0,255), 1);
		
		vector<Rect> boundRect( ms->contours.size() );
		for ( int i = 0; i < (int) ms->contours.size(); i++ ) { 
			boundRect[i] = boundingRect( ms->contours[i] );

			//calcular quais as de partes para a composição
			int n = composicao ( boundRect[i].br().y, partes );
			dividir_contorno ( &ms->contours[i], n, partes );
			//partes->desenharContornos ( &ms->frame );
			partes->estimarBoxes();
			partes->desenharBoxes( &ms->frame );
			partes->imprimirPontos( );
			//char key = cv::waitKey(0);

			/* Fit line
			for (int j = 0; j < (int) partes->divContours.size(); j++) {
				fitLine ( partes->divContours[j], partes->fitL[j], CV_DIST_L2, 0, 0.01, 0.01 );
			}*/
		}		
	}
	partes->limparContornos();
	ms->contours.clear();
}

int composicao ( int y, Partes * partes ) {
	int index_parte = 0;
	for (int i = 0; i < (int) partes->tamanho.size(); i++) {
		if ( y >=  partes->tamanho[i][1] * (1+MIN_PROP_PARTE) ) 
			index_parte++;			
	}
	if ( index_parte > partes->n_partes )
		index_parte = partes->n_partes;
	
	return index_parte;
}
void dividir_contorno ( std::vector<cv::Point> * contour, int n, Partes * partes ) {
	int T = (int) partes->tamanho.size();
	int C = (int) contour->size();
	for (int e = 0; e < C; e++){
		int index_local = 0;
		for (int i = 0; i < T; i++) {
			if ( (*contour)[e].y >= partes->tamanho[i][1] ) 
				index_local++;
		}
		if ( index_local > n )
			index_local = n;

		partes->divContours[n - index_local].push_back ( (*contour)[e] );
	}	
}


void selectContours ( std::vector<std::vector<cv::Point>> * contours, int minArea ) {
	int e, total = (int) contours->size( );
	for ( e = 0; e < total; e++ ) {
		int area = (int) contourArea ( (*contours)[e] );
		if ( area <= minArea ) {
			contours->erase (contours->begin()+e );
			total--;
			e--;
		}
	}
}
int distanciaMaxima ( entryOptions * options, WStuff * ms  ) {
	int maxd = 0;
	for (;;) {
		ms->vfile >> ms->frame;
		if ( ms->frame.empty() )
			break;
		
		if ( (int) ms->frame.channels( ) > 1 )
			ms->getBinaryFrame ( "simple" );
			
		findContours ( ms->binFrame, ms->contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
		ms->binFrame.setTo ( 0, noArray( ) );

		if ( (int) ms->contours.size( ) > 0 ) {
			selectContours ( &ms->contours, MIN_AREA );
			int d = maxY ( &ms->contours );
			if (d >= maxd)
				maxd = d;

		}
	}
	ms->vfile.release();
	ms->openVideo ( options->videoFilename );
	return maxd;
}
int maxY ( std::vector<std::vector<cv::Point>> * contours ) {
	int e, total = (int) contours->size( );
	int maxY = 0;
	for ( e = 0; e < total; e++ ) {
		int cpoints = (*contours)[e].size();
		for ( int c = 0; c < cpoints; c++ ) {
			if ( maxY <= (*contours)[e][c].y ) {
				maxY = (*contours)[e][c].y;				
			}
		}
	}
	return maxY;
}



