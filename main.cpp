#include <QtGui/QApplication>
#include <QtCore/QCoreApplication>
#include "mainwindow.h"
#include <QtGui/QImage>
#include <QtGui/QVector2D>
#include <QDebug>
#include <stack>
#include <iostream>
#include <QFile>
#include <QStringList>

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

struct fcol{
	float B;
	float G;
	float R;
	float A;
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
	QCoreApplication coreapp(argc, argv);
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
	
	//im.load("im-0043.png");
	im.load(coreapp.arguments().at(1).toLocal8Bit().constData());
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
	
	
	
	QFile file("in.txt");
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	
	QTextStream fileRef(&file);
	//fileRef << 100.0 << 12.7;
	QString line;
	line = fileRef.readLine();
	line = fileRef.readLine();
	QStringList list1;
	float c[5][12];
	for (int i =0; i < 5; i++){
		line = fileRef.readLine();
		list1 = line.split(",");
		for (int j = 0; j < 12; j++){
			c[i][j] = list1.at(j).toFloat();
		}
	}
	
	
	line = fileRef.readLine();
	line = fileRef.readLine();
	list1 = line.split(",");
	int s1 = list1.at(0).toInt();
	int g1 = list1.at(1).toInt();
	int g2 = list1.at(2).toInt();
	int s2 = list1.at(3).toInt();
	int s3 = list1.at(4).toInt();
	file.close();
	
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){	
			col q = r_p[x+width*y];
			if (q.R>=c[0][0] && q.R<=c[0][1] && q.G>=c[0][2] && q.G<=c[0][3] && q.B>=c[0][4] && q.B<=c[0][5] &&
			  q.R/(q.G+1)>=c[0][6] && q.R/(q.G+1)<=c[0][7] && q.G/(q.B+1)>=c[0][8] && q.G/(q.B+1)<=c[0][9] && q.B/(q.R+1)>=c[0][10] && q.B/(q.R+1)<=c[0][11]){
				w_gmask[x+width*y] = R*255+G*255+B*255;
			}
			
			//p[x+width*y] = 255*G + 100*A;
		}
	}
	
	shrink(r_gmask, w_gmask, s1, width, height);
	expand(r_gmask, w_gmask, g1, width, height);
	expand(r_gmask, w_gmask, 1, width, height);
	expand(r_gmask, w_gmask, g2, width, height);
	expand(r_gmask, w_gmask, 1, width, height);
	shrink(r_gmask, w_gmask, s2, width, height);
	shrink(r_gmask, w_gmask, s3, width, height);
	mask(w_p, w_gmask, width, height);
	//cout << x;
	

	
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){	
			col q = r_p[x+width*y];
			fcol qf;
			qf.R = float(q.R);
			qf.G = float(q.G);
			qf.B = float(q.B);
			
			//do magenta first so it doesn't get done by red
			int n = 2;
			//cout << float(q.B)/float(q.R+1);
			if (qf.R>=c[n][0] && qf.R<=c[n][1] && qf.G>=c[n][2] && qf.G<=c[n][3] && qf.B>=c[n][4] && qf.B<=c[n][5] &&
			  qf.R/(qf.G+1)>=c[n][6] && qf.R/(qf.G+1)<=c[n][7] && qf.G/(qf.B+1)>=c[n][8] && qf.G/(qf.B+1)<=c[n][9] && qf.B/(qf.R+1)>=c[n][10] && qf.B/(qf.R+1)<=c[n][11]){
				w_mmask[x+y*width] = 255*R+255*G+255*B;
				w_p[x+y*width] = 255*R+255*B;
			} else if (n-- && qf.R>=c[n][0] && qf.R<=c[n][1] && qf.G>=c[n][2] && qf.G<=c[n][3] && qf.B>=c[n][4] && qf.B<=c[n][5] &&
					   qf.R/(qf.G+1)>=c[n][6] && qf.R/(qf.G+1)<=c[n][7] && qf.G/(qf.B+1)>=c[n][8] && qf.G/(qf.B+1)<=c[n][9] && qf.B/(qf.R+1)>=c[n][10] && qf.B/(qf.R+1)<=c[n][11]){ //red
				w_rmask[x+y*width] = 255*R+255*G+255*B;
				w_p[x+y*width] = 255*R;
			} else if (n++ && n++ && qf.R>=c[n][0] && qf.R<=c[n][1] && qf.G>=c[n][2] && qf.G<=c[n][3] && qf.B>=c[n][4] && qf.B<=c[n][5] &&
					   qf.R/(qf.G+1)>=c[n][6] && qf.R/(qf.G+1)<=c[n][7] && qf.G/(qf.B+1)>=c[n][8] && qf.G/(qf.B+1)<=c[n][9] && qf.B/(qf.R+1)>=c[n][10] && qf.B/(qf.R+1)<=c[n][11]){ //yellow
				w_ymask[x+y*width] = 255*R+255*G+255*B;
				w_p[x+y*width] = 255*R+255*G;
			} else if (n++ && qf.R>=c[n][0] && qf.R<=c[n][1] && qf.G>=c[n][2] && qf.G<=c[n][3] && qf.B>=c[n][4] && qf.B<=c[n][5] &&
					   qf.R/(qf.G+1)>=c[n][6] && qf.R/(qf.G+1)<=c[n][7] && qf.G/(qf.B+1)>=c[n][8] && qf.G/(qf.B+1)<=c[n][9] && qf.B/(qf.R+1)>=c[n][10] && qf.B/(qf.R+1)<=c[n][11]){ //cyan
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
	
	
	im.save("color-highlights.png", "PNG");
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
