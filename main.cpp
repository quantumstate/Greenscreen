#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QtGui/QImage>
#include <QtGui/QVector2D>
#include <QDebug>
#include <stack>
#include <iostream>
using namespace std;


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

struct pt{
	int x;
	int y;
};

struct fpt{
	double x;
	double y;
};

void expand(col* r, uint* w, int amount, int width, int height);
void shrink(col* r, uint* w, int amount, int width, int height);
void mask(uint* im, uint* mask, int width, int height);
void yelshrink(col* r, uint* w, int amount, int width, int height);
int flood_fill(col* r_im, uint* w_im, int x, int y, fpt *dcentre, int width);
void find_regions(col* r_im, uint* w_im, int width, int height, int region_size, bool max);

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
	QImage out = QImage(width, height, QImage::Format_RGB32);
	
	im.load("im-0043.png");
	//QImage im(width, height, QImage::Format_ARGB32);
	
	//uint* p = (uint*)im.bits();
	col* r_p = (col*)im.bits();
	uint* w_p = (uint*)im.bits();
	col* r_gmask = (col*)gmask.bits();
	uint* w_gmask = (uint*)gmask.bits();
	col* r_rmask = (col*)rmask.bits();
	uint* w_rmask = (uint*)rmask.bits();
	col* r_ymask = (col*)ymask.bits();
	uint* w_ymask = (uint*)ymask.bits();
	col* r_mmask = (col*)mmask.bits();
	uint* w_mmask = (uint*)mmask.bits();
	col* r_cmask = (col*)cmask.bits();
	uint* w_cmask = (uint*)cmask.bits();
	uint* w_out = (uint*)out.bits();
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
	
	yelshrink(r_ymask, w_ymask, 2, width, height);
	yelshrink(r_mmask, w_mmask, 2, width, height);
	yelshrink(r_rmask, w_rmask, 2, width, height);
	yelshrink(r_cmask, w_cmask, 2, width, height);
	expand(r_rmask, w_rmask, 4, width, height);
	find_regions(r_ymask, w_ymask, width, height, 1, true);
	find_regions(r_mmask, w_mmask, width, height, 1, true);
	find_regions(r_cmask, w_cmask, width, height, 1, true);
	find_regions(r_rmask, w_rmask, width, height, 20, false);
	
	
	//im.save("im-0008-new.png", "PNG");
	//gmask.save("im-0008-new-mask.png", "PNG");
	//mmask.save("im-0008-mmask.png", "PNG");
	//rmask.save("im-0008-rmask.png", "PNG");
	//ymask.save("im-0008-ymask.png", "PNG");
	//cmask.save("im-0008-cmask.png", "PNG");
	
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

void find_regions(col* r_im, uint* w_im, int width, int height, int region_size, bool max){
	for (int x = 0; x < width; x++){
		w_im[x] = 0;
		w_im[x+width*(height-1)] = 0;
	}
	for (int y = 0; y < height; y++){
		w_im[y*width] = 0;
		w_im[width-1+width*y] = 0;
	}
	
	fpt max_centre;
	int max_size=0;
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			if (r_im[x+y*width].R ==255){
				fpt dcentre;
				int count = flood_fill(r_im, w_im, x, y, &dcentre, width);
				if (count >= region_size){
					//w_out[int(dcentre.x) + width * int(dcentre.y)] = col;
					if (max){
						if (count >= max_size){
							max_size = count;
							max_centre = dcentre;
						}
					}else{
						cout << dcentre.x << " " <<dcentre.y << "\n";
						//qDebug() << dcentre.x << dcentre.y;
					}
				}
			}
		}
	}
	if (max){
		cout << max_centre.x << " " << max_centre.y << "\n";
		//qDebug() << max_centre.x << max_centre.y;
	}
}

int flood_fill(col* r_im, uint* w_im, int x, int y, fpt *dcentre, int width){
	int count = 1;
	pt centre;
	centre.x = x;
	centre.y = y;
	
	int pos [8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
	
	stack<pt> pts;
	pts.push(centre);
	
	while (!pts.empty()){
		pt cur = pts.top();
		pts.pop();
		for (int i = 0; i < 8; i++){
			if (r_im[(cur.x+pos[i][0]) + width*(cur.y+pos[i][1])].R == 255){
				r_im[(cur.x+pos[i][0]) + width*(cur.y+pos[i][1])].R = 0;
				count++;
				pt tmp;
				tmp.x = cur.x+pos[i][0];
				tmp.y = cur.y+pos[i][1];
				pts.push(tmp);
				centre.x += cur.x+pos[i][0];
				centre.y += cur.y+pos[i][1];
			}
		}
	}
	dcentre->x = centre.x / float(count);
	dcentre->y = centre.y / float(count);
	
	return count;
	
}
