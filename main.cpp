#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QtGui/QImage>
#include <QtGui/QVector2D>
#include <QDebug>

const uint R = 256*256;
const uint G = 256;
const uint B = 1;
const uint A = 256*256*256;

struct col{
	uchar B;
	uchar G;
	uchar R;
	uchar A;
};

void expand(col* r, uint* w, int amount, int width, int height);
void shrink(col* r, uint* w, int amount, int width, int height);
void mask(uint* im, uint* mask, int width, int height);


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
	
	const int width = 1920;
	const int height = 1080;
	QImage im = QImage(width, height,QImage::Format_ARGB32);
	QImage gmask = QImage(width, height, QImage::Format_RGB32);
	QImage rmask = QImage(width, height, QImage::Format_RGB32);
	QImage ymask = QImage(width, height, QImage::Format_RGB32);
	QImage mmask = QImage(width, height, QImage::Format_RGB32);
	QImage cmask = QImage(width, height, QImage::Format_RGB32);
	
	im.load("im-0043.png");
	//QImage im(width, height, QImage::Format_ARGB32);
	
	//uint* p = (uint*)im.bits();
	col* r_p = (col*)im.bits();
	uint* w_p = (uint*)im.bits();
	col* r_gmask = (col*)gmask.bits();
	uint* w_gmask = (uint*)gmask.bits();
	col* r_rmask = (col*)gmask.bits();
	uint* w_rmask = (uint*)gmask.bits();
	col* r_ymask = (col*)gmask.bits();
	uint* w_ymask = (uint*)gmask.bits();
	col* r_mmask = (col*)gmask.bits();
	uint* w_mmask = (uint*)gmask.bits();
	col* r_cmask = (col*)gmask.bits();
	uint* w_cmask = (uint*)gmask.bits();
	//qDebug() << sizeof(uint);
	
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){	
			col q = r_p[x+width*y];
			if ((q.G > q.R * 1.4) && (q.G > q.B * 2)){
				w_gmask[x+width*y] = R*255+G*255+B*255;
			}
			
			//p[x+width*y] = 255*G + 100*A;
		}
	}
	shrink(r_gmask, w_gmask, 10, width, height);
	expand(r_gmask, w_gmask, 40, width, height);
	shrink(r_gmask, w_gmask, 30, width, height);
	mask(w_p, w_gmask, width, height);
	
	
	
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){	
			col q = r_p[x+width*y];
			//do magenta first so it doesn't get done by red
			if ((q.R > q.G * 2) && (q.B > q.G * 2) && (q.R/q.B > 0.7) && (q.R/q.B < 8)){
				w_mmask[x+y*width] = 255*R+255*G+255*B;
				w_p[x+y*width] = 255*R+255*B;
			} else if ((q.R > q.G * 3) && (q.R > q.B * 3)){
				w_rmask[x+y*width] = 255*R+255*G+255*B;
				w_p[x+y*width] = 255*R;
			} else if ((q.R > q.B * 2) && (q.G > q.B * 2) && (q.R/q.G > 0.7) && (q.R/q.G < 1.3)){
				w_ymask[x+y*width] = 255*R+255*G+255*B;
				w_p[x+y*width] = 255*R+255*G;
			} else if ((q.B > 0) && (q.G > 0) && (q.G/q.B < 2)){
				w_cmask[x+y*width] = 255*R+255*G+255*B;
				w_p[x+y*width] = 255*B+255*G;
			}
			//w_p[x+y*width] = 255*B+255*G;
		}
	}
	
	yelshrink(r_gmask, w_gmask, 10, width, height);
	
	
	im.save("im-0008-new.png", "PNG");
	gmask.save("im-0008-new-mask.png", "PNG");
	
	exit(0);
    return a.exec();
}

void expand(col* r, uint* w, int amount, int width, int height){
	for (int x = amount; x < width-amount; x++){
		for (int y = amount+x%2; y < height-amount; y+=2){
			if (r[x+width*y].R==255){
				for (int i = -amount; i <= amount; i++){
					r[x+i+width*y].B = 255;
				}
				for (int j = -amount; j <= amount; j++){
					r[x+width*(y+j)].B = 255;
				}
			}
		}
	}
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			if (w[x+width*y] == 255){
				w[x+width*y] = 255*R+255*G+255*B;
			}
		}
	}
}

void shrink(col* r, uint* w, int amount, int width, int height){
	for (int x = amount; x < width-amount; x++){
		for (int y = amount+(x%2); y < height-amount; y+=2){
			if (r[x+width*y].R==0){
				for (int i = -amount; i <= amount; i++){
					r[x+i+width*y].B = 0;
				}
				for (int j = -amount; j <= amount; j++){
					r[x+width*(y+j)].B = 0;
				}
			}
		}
	}
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			if (r[x+width*y].B == 0){
				w[x+width*y] = 0;
			}
		}
	}
}

void yelshrink(col* r, uint* w, int amount, int width, int height){
	for (int x = amount; x < width-amount; x++){
		for (int y = amount; y < height-amount; y++){
			if (r[x+width*y].R==255){
				if ((r[x+amount+width*y].B == 0) || (r[x-amount+width*y].B == 0)){
					r[x+width*y].G = 0;
				} else if ((r[x+width*(y+amount)].B == 0) || (r[x+width*(y-amount)].B == 0)){
					r[x+width*y].G = 0;
				}
			}
		}
	}
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			if (r[x+width*y].G == 0){
				w[x+width*y] = 0;
			}
		}
	}
}

void mask(uint* im, uint* mask, int width, int height){
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			if (mask[x+width*y] == 0){
				im[x+width*y] = 0;
			}
		}
	}
}

