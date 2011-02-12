#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QtGui/QImage>
#include <QtGui/QVector2D>
#include <QDebug>

const int levels = 2;

const int size [] = {5,5};
double P [5][5][2];
double Pn[(4)*2^levels+1][(4)*2^levels+1][2];

double subdivide_grid();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
	
	const int width = 1920;
	const int height = 1080;

	QImage im(width, height, QImage::Format_ARGB32);
	
	for (int x = 0; x < size[0]; x++){
		for (int y = 0; y < size[1]; y++){
			P[x][y][0] = double(x * 50);
			P[x][y][1] = double(y * 50);
		}
	}
	
	subdivide_grid();
	
	exit(0);
    return a.exec();
}

double subdivide_grid(){
	int divs = 2^levels;
	
	
	for (int x = 0; x < size[0]; x++){
		for (int y = 0; y < size[1]; y++){
			Pn[x*divs][y*divs][0] = P[x][y][0];
			Pn[x*divs][y*divs][1] = P[x][y][1];
			//qDebug() << Pn[x*divs][y*divs][0] << Pn[x*divs][y*divs][1];
		}
	}
	
	int cur_level = 0;
	
	for (int j = 0; j < levels; j++){
		int hdivs = divs / 2;
		
		for (int y = 0; y < size[1]*divs; y += divs){
			double Ps [size[0]][2];
			double Psn [size[0]*2-1][2];
			double right [2] = {1,0};
			for (int i = 0; i < size[0]; i++){
				Ps[i][0] = Pn[i*divs][y][0];
				Ps[i][0] = Pn[i*divs][y][1];
			}
			interpolate_strip(Ps, Psn, right)
		}
	}
}

int interpolate_strip(& ){
	
}
