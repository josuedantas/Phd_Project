#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/tracking.hpp>
#include <iostream>
#include <vector>
#include <math.h> 

#define PI 3.14159265

using namespace cv;
using namespace std;

double getTemplate1(Mat& img,Point p){
    double mediaR1 = 0,mediaR2 = 0;
         mediaR1 = (img.at<double>(p.x+5,p.y+1)-img.at<double>(p.x+5,p.y)-(img.at<double>(p.x-1,p.y+1)-
         img.at<double>(p.x-1,p.y)))/6;

         mediaR2 = (img.at<double>(p.x+5,p.y)-img.at<double>(p.x+5,p.y-1)-(img.at<double>(p.x-1,p.y)-
         img.at<double>(p.x-1,p.y-1)))/6;
    return mediaR1 - mediaR2;
       
}

double getTemplate2(Mat& img,Point p){
    double mediaR1 = 0,mediaR2 = 0;
         mediaR1 = (img.at<double>(p.x,p.y+5)-img.at<double>(p.x-1,p.y+5)-(img.at<double>(p.x,p.y-1)-
         img.at<double>(p.x-1,p.y-1)))/6;

         mediaR2 = (img.at<double>(p.x+1,p.y+5)-img.at<double>(p.x,p.y+5)-(img.at<double>(p.x+1,p.y-1)-
         img.at<double>(p.x,p.y-1)))/6;
    return mediaR1 - mediaR2;
}

double getTemplate3(Mat& img,Point p){
    double mediaR1 = 0,mediaR2 = 0;
         mediaR1 = (img.at<double>(p.x+2,p.y+2)-img.at<double>(p.x+1,p.y+3)-img.at<double>(p.x-1,p.y-1)+
         img.at<double>(p.x-2,p.y))/8; // Branco

         mediaR2 = (img.at<double>(p.x+3,p.y+1)-img.at<double>(p.x+2,p.y+2)-img.at<double>(p.x,p.y-2)+
         img.at<double>(p.x-1,p.y-1))/8; // Preto
    return mediaR1 - mediaR2;
}

double getTemplate4(Mat& img,Point p){
    double mediaR1 = 0,mediaR2 = 0;
            mediaR1 = (img.at<double>(p.x+2,p.y)-img.at<double>(p.x+1,p.y-1)-img.at<double>(p.x-1,p.y+3)+
            img.at<double>(p.x-2,p.y+2))/8; // Branco

            mediaR2 = (img.at<double>(p.x+4,p.y)-img.at<double>(p.x+3,p.y-1)-img.at<double>(p.x+1,p.y+3)+
            img.at<double>(p.x,p.y+2))/8; // Preto
    return mediaR1 - mediaR2;
}

int main(int argc, char** argv)
{
//vector<Point> dir1, dir2, dir3, dir4;
vector< vector<Point> > vec1, vec2, vec3, vec4;
vector <vector<double>> dist1, dist2, dist3, dist4;
clock_t t1,t2,t3,t4;
t1=clock();
 // Read the image file
 //Mat image = imread("./Leaves/1001.jpg");
 Mat image = imread("./Leaves/1060.jpg");

 resize(image,image,Size(),0.5,0.5,CV_INTER_LINEAR);

 Mat hsv_image;
 cvtColor( image, hsv_image, CV_BGR2HSV );

 t2 = clock();
 float diff = ((float)t2-(float)t1);
 float seconds = diff / CLOCKS_PER_SEC;
 cout << seconds<<"\n";

 Mat out (hsv_image.size(),CV_8UC1);
 Mat edge;
 vector<Mat> channels;
 split(hsv_image,channels);
 Mat h = channels[0];
 Mat v = channels[2];

 
 for(int i = 0; i < hsv_image.rows; i++){
     for(int j = 0; j < hsv_image.cols; j++){
         out.at<uchar>(i,j) = (((h.at<uchar>(i,j)+90)%360)/360+1-v.at<uchar>(i,j))/2;
     }
 }
    t3 = clock();
 float diff2 = ((float)t3-(float)t2);
 float seconds2 = diff2 / CLOCKS_PER_SEC;
 cout << seconds2<<"\n";

 //resize(out,out,Size(),0.45,0.45,CV_INTER_LINEAR);

 Canny( out, edge, 20, 40, 3);

 int esq=edge.cols, dir=0, cima=edge.rows, baixo=0; // primeiro pixel diferente de 255.
 for (int l=0; l<edge.rows; l++)
    for (int c=0; c<edge.cols; c++) {
        if (edge.at<int>(l,c)==255) {
        if (c<esq) esq=c;
        if (dir<c) dir=c;
        if (l<cima) cima=l;
        if (baixo<l) baixo=l;
        }
 }

 Mat roi(edge, Rect(esq,cima,dir-esq+1,baixo-cima+1));
 
  //double t=threshold(out, edge, 0, 255, THRESH_BINARY);
 

 Mat saida(roi.size(),CV_8UC1);
    double mediaR1 = 0,mediaR2 = 0, dist = 0;
    int contP1 = 0, contP2 = 0, contP3 = 0, contP4 = 0;
 bool sem = false;

 Mat imgIntegral, imgInt2,imgInt45;
    integral(roi,imgIntegral,imgInt2,imgInt45,CV_64F);

/* for(int i = edge.rows/2; i <(edge.rows/2)+18; i++){
    for(int j = edge.cols/2; j < (edge.cols/2)+16; j++){
        cout << (int)edge.at<uchar>(i,j) <<" ";
    }
        cout << "\n";
}
cout <<"\n";
for(int i = imgIntegral.rows/2; i <(imgIntegral.rows/2)+18; i++){
    for(int j = imgIntegral.cols/2; j < (imgIntegral.cols/2)+16; j++){
        cout << imgIntegral.at<int>(i,j) <<" ";
    }
        cout << "\n";
} */
 // se não encontrou em uma parte da imagem não há motivos para buscar no todo
int contV = 0;
bool sem1 = false;

 for(int i = 1; i < roi.rows - 6; i++){
     sem = false;
     for(int j = 1; j < roi.cols - 2; j++){
         /* mediaR1 = (imgIntegral.at<double>(i+5,j+1)-imgIntegral.at<double>(i+5,j)-(imgIntegral.at<double>(i-1,j+1)-
         imgIntegral.at<double>(i-1,j)))/6;

         mediaR2 = (imgIntegral.at<double>(i+5,j)-imgIntegral.at<double>(i+5,j-1)-(imgIntegral.at<double>(i-1,j)-
         imgIntegral.at<double>(i-1,j-1)))/6; */
        /*  mediaR1 = (edge.at<uchar>(i,j+1)+edge.at<uchar>(i+1,j+1)+edge.at<uchar>(i+2,j+1)+
         edge.at<uchar>(i+3,j+1)+edge.at<uchar>(i+4,j+1)+edge.at<uchar>(i+5,j+1)+
         edge.at<uchar>(i+6,j+1)+edge.at<uchar>(i+7,j+1))/8;

         mediaR2 = (edge.at<uchar>(i,j)+edge.at<uchar>(i+1,j)+edge.at<uchar>(i+2,j)+
         edge.at<uchar>(i+3,j)+edge.at<uchar>(i+4,j)+edge.at<uchar>(i+5,j)+
         edge.at<uchar>(i+6,j)+edge.at<uchar>(i+7,j))/8; */
         if ((getTemplate1(imgIntegral,Point(i,j))) > 200){
             if(vec1.size()==0){
                 vec1.push_back(vector<Point>());
                 vec1[contV].push_back(Point(i,j+1));
             }else{
                 for(int k = 0; k < vec1.size(); k++){
                 sem1 = false;
                 Point aux = vec1[k].back();
                 int x1 = 0 , y1 = 0, x2 = 0, y2 = 0;
                 /* x1 = 0;
                 y1 = aux.x -7;
                 x2 = j - 1 - (aux.y - 1);
                 y2 = i - 7;
                 double ang = 0; */
                 x1 = aux.y;
                 y1 = aux.x;
                 x2 = j+1;
                 y2 = i;
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2));
                // ang = acos(((x1*x2)+(y1*y2))/(sqrt((pow(x1,2)+pow(y1,2)))*sqrt(pow(x2,2)+pow(y2,2))));
                 if(dist <= 6){
                     sem1 = true;
                     //vec1[k].pop_back();
                     vec1[k].push_back(Point(i,j+1));
                     break;
                 }
                 }
                 if(sem1 == false){
                     contV++;
                     vec1.push_back(vector<Point>());
                     vec1[contV].push_back(Point(i,j+1));
                 }
            
             }
           // cout << mediaR1 - mediaR2 <<"\n";
           // dir1.push_back(Point(i,j));
        saida.at<uchar>(i,j) = 255;
        contP1++;
             
             //sem = true;
         }
        /*  mediaR1 = (edge.at<uchar>(i,j)+edge.at<uchar>(i,j+1)+edge.at<uchar>(i,j+2)+
        edge.at<uchar>(i,j)+edge.at<uchar>(i,j+1)+edge.at<uchar>(i,j+2) )/3;
         mediaR2 = (edge.at<uchar>(i,j+1)+edge.at<uchar>(i+1,j+1)+edge.at<uchar>(i+2,j+1))/3;
         if ((mediaR1 - mediaR2) > 140){
           // cout << mediaR1 - mediaR2 <<"\n";
             saida.at<uchar>(i,j) = 255;
             //sem = true;
         } */
     }
 }
 contV = 0;
 sem1 = false;

 for(int i = 1; i < roi.rows - 2; i++){
     sem = false;
     for(int j = 1; j < roi.cols - 6; j++){
         /* mediaR1 = (imgIntegral.at<double>(i,j+5)-imgIntegral.at<double>(i-1,j+5)-(imgIntegral.at<double>(i,j-1)-
         imgIntegral.at<double>(i-1,j-1)))/6;

         mediaR2 = (imgIntegral.at<double>(i+1,j+5)-imgIntegral.at<double>(i,j+5)-(imgIntegral.at<double>(i+1,j-1)-
         imgIntegral.at<double>(i,j-1)))/6; */
        /*  mediaR1 = (edge.at<uchar>(i,j)+edge.at<uchar>(i,j+1)+edge.at<uchar>(i,j+2)+
         edge.at<uchar>(i,j+3)+edge.at<uchar>(i,j+4)+edge.at<uchar>(i,j+5)+
         edge.at<uchar>(i,j+6)+edge.at<uchar>(i,j+7))/8;
         
         mediaR2 = (edge.at<uchar>(i+1,j)+edge.at<uchar>(i+1,j+1)+edge.at<uchar>(i+1,j+2)+
         edge.at<uchar>(i+1,j+3)+edge.at<uchar>(i+1,j+4)+edge.at<uchar>(i+1,j+5)+
         edge.at<uchar>(i+1,j+6)+edge.at<uchar>(i+1,j+7))/8; */
         if ((getTemplate2(imgIntegral,Point(i,j))) > 200){
           // cout << mediaR1 - mediaR2 <<"\n";
           // dir2.push_back(Point(i,j));
           if(vec2.size()==0){
                 vec2.push_back(vector<Point>());
                 vec2[contV].push_back(Point(i,j+5));
             }else{
                 for(int k = 0; k < vec2.size(); k++){
                 sem1 = false;
                 Point aux = vec2[k].back();
                 int x1 = 0 , y1 = 0, x2 = 0, y2 = 0;
                 /* x1 = aux.y -4;
                 y1 = 0;
                 x2 = j - 4;
                 y2 = i - 1 - (aux.x - 1); */
                 x1 = aux.y;
                 y1 = aux.x;
                 x2 = j;
                 y2 = i;
                 double ang = 0;
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2));
                // ang = acos(((x1*x2)+(y1*y2))/(sqrt((pow(x1,2)+pow(y1,2)))*sqrt(pow(x2,2)+pow(y2,2))));
                 if(dist <= 6){
                     sem1 = true;
                     //vec2[k].pop_back();
                     vec2[k].push_back(Point(i,j+5));
                     //cout<< "Vetor "<<k<<" Ponto "<<i<<" "<<j<<"\n";
                     break;
                 }
                 }
                 if(sem1 == false){
                     contV++;
                     vec2.push_back(vector<Point>());
                     vec2[contV].push_back(Point(i,j+5));
                 }
         }
    saida.at<uchar>(i,j) = 255;
    contP2++;
     }
 }
 }

 contV = 0;
 sem1 = false;
 double mediaR3 = 0;
 double d3 = 0;

 for(int i = 2; i < roi.rows - 4; i++){
     sem = false;
     for(int j = 2; j < roi.cols - 4; j++){
/* 
         mediaR3 = (imgInt45.at<double>(i+1,j+3)-imgInt45.at<double>(i,j+4)-imgInt45.at<double>(i-2,j)+
         imgInt45.at<double>(i-3,j+1))/8; // Preto */
        /*  mediaR1 = (imgInt45.at<double>(i+2,j+2)-imgInt45.at<double>(i+1,j+3)-imgInt45.at<double>(i-1,j-1)+
         imgInt45.at<double>(i-2,j))/8; // Branco

         mediaR2 = (imgInt45.at<double>(i+3,j+1)-imgInt45.at<double>(i+2,j+2)-imgInt45.at<double>(i,j-2)+
         imgInt45.at<double>(i-1,j-1))/8; // Preto */

         /* mediaR1 = (edge.at<uchar>(i,j+1)+edge.at<uchar>(i+1,j+2)+edge.at<uchar>(i+2,j+3)+
         edge.at<uchar>(i+3,j+4)+edge.at<uchar>(i+4,j+5)+edge.at<uchar>(i+5,j+6))/6;

         mediaR2 = (edge.at<uchar>(i,j)+edge.at<uchar>(i+1,j+1)+edge.at<uchar>(i+2,j+2)+
         edge.at<uchar>(i+3,j+3)+edge.at<uchar>(i+4,j+4)+edge.at<uchar>(i+5,j+5))/6; */
         if ((getTemplate3(imgInt45,Point(i,j))) > 90){
           // cout << mediaR1 - mediaR2 <<"\n";
            // dir3.push_back(Point(i,j));
             saida.at<uchar>(i,j) = 255;
             contP3++;
             if(vec3.size()==0){
                vec3.push_back(vector<Point>());
                vec3[contV].push_back(Point(i+2,j+2));
             }else{
                 for(int k = 0; k < vec3.size(); k++){
                 sem1 = false;
                 Point aux = vec3[k].back();
                 int x1 = 0 , y1 = 0, x2 = 0, y2 = 0, descX = 0, descY=0;
                 /* descX = aux.y - 2;
                 descY = aux.x - 2;
                 x1 = aux.y - descX;
                 y1 = aux.x - descY;
                 x2 = j - descX;
                 y2 = i - descY;
                 double ang = 0; */
                 x1 = aux.y;
                 y1 = aux.x;
                 x2 = j;
                 y2 = i;
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2));
                // ang = acos(((x1*x2)+(y1*y2))/(sqrt((pow(x1,2)+pow(y1,2)))*sqrt(pow(x2,2)+pow(y2,2))));
                 if(dist <= 6){
                     sem1 = true;
                     //vec3[k].pop_back();
                     vec3[k].push_back(Point(i+2,j+2));
                     break;
                 }
                 }
                 if(sem1 == false){
                     contV++;
                     vec3.push_back(vector<Point>());
                     vec3[contV].push_back(Point(i+2,j+2));
                 }
             }
         }
             //sem = true;
          /*   mediaR3 = (imgInt45.at<double>(i,j)-imgInt45.at<double>(i-1,j-1)-imgInt45.at<double>(i-3,j+3)+
            imgInt45.at<double>(i-4,j+2))/8; // Preto

            mediaR1 = (imgInt45.at<double>(i+2,j)-imgInt45.at<double>(i+1,j-1)-imgInt45.at<double>(i-1,j+3)+
            imgInt45.at<double>(i-2,j+2))/8; // Branco

            mediaR2 = (imgInt45.at<double>(i+4,j)-imgInt45.at<double>(i+3,j-1)-imgInt45.at<double>(i+1,j+3)+
            imgInt45.at<double>(i,j+2))/8; // Preto */

         
         /* mediaR1 = (edge.at<uchar>(i,j+5)+edge.at<uchar>(i+1,j+4)+edge.at<uchar>(i+2,j+3)+
         edge.at<uchar>(i+3,j+2)+edge.at<uchar>(i+4,j+1)+edge.at<uchar>(i+5,j))/6;

         mediaR2 = (edge.at<uchar>(i,j+6)+edge.at<uchar>(i+1,j+5)+edge.at<uchar>(i+2,j+4)+
         edge.at<uchar>(i+3,j+3)+edge.at<uchar>(i+4,j+2)+edge.at<uchar>(i+5,j+1))/6; */
         if ((getTemplate4(imgInt45,Point(i,j))) > 90){
           // cout << mediaR1 - mediaR2 <<"\n";
            // dir4.push_back(Point(i,j));
            if(vec4.size()==0){
                vec4.push_back(vector<Point>());
                vec4[0].push_back(Point(i+2,j));
             }else{
                 for(int k = 0; k < vec4.size(); k++){
                 sem1 = false;
                 Point aux = vec4[k].back();
                 int x1 = 0 , y1 = 0, x2 = 0, y2 = 0, descX = 0, descY = 0;
                 /* descX = aux.y + 2;
                 descY = aux.x - 2;
                 x1 = aux.y - descX;
                 y1 = aux.x - descY;
                 x2 = j - descX;
                 y2 = i - descY+2;
                 double ang = 0; */
                 x1 = aux.y;
                 y1 = aux.x;
                 x2 = j;
                 y2 = i;
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2));
                // ang = acos(((x1*x2)+(y1*y2))/(sqrt((pow(x1,2)+pow(y1,2)))*sqrt(pow(x2,2)+pow(y2,2))));
                 if(dist <= 6){
                     sem1 = true;
                     //vec4[k].pop_back();
                     vec4[k].push_back(Point(i+2,j));
                     break;
                 }
                 }
                 if(sem1 == false){
                    // contV++;
                     vec4.push_back(vector<Point>());
                     vec4[vec4.size()-1].push_back(Point(i+2,j));
                 }
             }
             saida.at<uchar>(i,j) = 255;
             contP4++;
             //sem = true;
         }
     }
 }

 Mat saida1(roi.rows,roi.cols,CV_8UC1);
/*  Mat saida2(out.size(),CV_8UC1);
 line(saida2,Point(243,63),Point(313,613),255,1,8,0); */
 //line(saida2,Point(2,3),Point(3,2),255,1,8,0);

double distance = 0;
/* for(int i = 0; i < vec1.size();i++){
    dist1.push_back(vector<double>());
    for(int j = 0; j < vec1[i].size()-1;j++){
    Point p1 = vec1[i].at(j);
    Point p2 = vec1[i].at(j+1);
    distance = sqrt(pow(p2.x-p1.x,2)+pow(p2.y-p1.y,2));
    dist1[i].push_back(distance);
    }
}
double desvP1 = 0, med1 = 0;
for(int i = 0; i < dist1.size();i++){
    for(int j = 0; j < dist1[i].size();j++){
        med1 = med1 +dist1[i].at(j);
    }
    med1 = med1 / dist1[i].size();
    for(int k = 0; k < dist1[i].size(); k++){
        desvP1 = desvP1 + (pow(dist1[i].at(k) - med1,2));
    }
    desvP1 = sqrt(desvP1/dist1[i].size());
    for(int m = 0; m < dist1[i].size(); m++){
        if(dist1[i].at(m)< desvP1*0.5 || dist1[i].at(m) > desvP1*1.5 )
         vec1[i].erase(vec1[i].begin()+(m+1));
    }
} */

/* for(int i = 0; i < vec2.size();i++){
    dist2.push_back(vector<double>());
    for(int j = 0; j < vec2[i].size()-1;j++){
    Point p1 = vec2[i].at(j);
    Point p2 = vec2[i].at(j+1);
    distance = sqrt(pow(p2.x-p1.x,2)+pow(p2.y-p1.y,2));
    dist2[i].push_back(distance);
    }
}
double desvP2 = 0, med2 = 0;
for(int i = 0; i < dist2.size();i++){
    for(int j = 0; j < dist2[i].size();j++){
        med2 = med2 +dist2[i].at(j);
    }
    med2 = med2 / dist2[i].size();
    for(int k = 0; k < dist2[i].size(); k++){
        desvP2 = desvP2 + (pow(dist2[i].at(k) - med2,2));
    }
    desvP2 = sqrt(desvP2/dist2[i].size());
    for(int m = 0; m < dist2[i].size(); m++){
        if(dist2[i].at(m) > desvP2*1.5 )
         vec2[i].erase(vec1[i].begin()+(m+1));
    }
}
 */
/* for(int i = 0; i < vec3.size();i++){
    dist3.push_back(vector<double>());
    for(int j = 0; j < vec3[i].size()-1;j++){
    Point p1 = vec3[i].at(j);
    Point p2 = vec3[i].at(j+1);
    distance = sqrt(pow(p2.x-p1.x,2)+pow(p2.y-p1.y,2));
    dist3[i].push_back(distance);
    }
}
double desvP3 = 0, med3 = 0;
for(int i = 0; i < dist3.size();i++){
    for(int j = 0; j < dist3[i].size();j++){
        med3 = med3 +dist3[i].at(j);
    }
    med3 = med3 / dist3[i].size();
    for(int k = 0; k < dist3[i].size(); k++){
        desvP3 = desvP3 + (pow(dist3[i].at(k) - med3,2));
    }
    desvP3 = sqrt(desvP3/dist3[i].size());
    for(int m = 0; m < dist3[i].size(); m++){
        if( dist3[i].at(m) > (med3*1.2) )
         vec3[i].erase(vec3[i].begin()+(m+1));
    }
} */

/* for(int i = 0; i < vec4.size();i++){
    dist4.push_back(vector<double>());
    for(int j = 0; j < vec4[i].size()-1;j++){
    Point p1 = vec4[i].at(j);
    Point p2 = vec4[i].at(j+1);
    distance = sqrt(pow(p2.x-p1.x,2)+pow(p2.y-p1.y,2));
    dist4[i].push_back(distance);
    }
}
double desvP4 = 0, med4 = 0;
for(int i = 0; i < dist4.size();i++){
    for(int j = 0; j < dist4[i].size();j++){
        med4 = med4 +dist4[i].at(j);
    }
    med4 = med4 / dist4[i].size();
   /*  for(int k = 0; k < dist4[i].size(); k++){
        desvP4 = desvP4 + (pow(dist4[i].at(k) - med4,2));
    }
    desvP4 = sqrt(desvP4/dist4[i].size()); */
   /* for(int m = 0; m < dist4[i].size(); m++){
        if(dist4[i].at(m) > med4){
        vec4[i].erase(vec4[i].begin()+(m+1));
        }
    }
} */
//line(saida1,Point(30,20),Point(200,20),255,1,8,0);
 for(int i = 0; i < vec1.size();i++){
     if(vec1[i].size()>5){
     Point inicio = vec1[i].front();
     Point fim = vec1[i].back();
     line(saida1,Point(inicio.y,inicio.x),Point(fim.y,fim.x),255,1,8,0);
     }
 }
 for(int i = 0; i < vec2.size();i++){
     if(vec2[i].size()>5){
     Point inicio = vec2[i].front();
     Point fim = vec2[i].back();
    // line(saida1,inicio,fim,255,1,8,0);
    line(saida1,Point(inicio.y,inicio.x),Point(fim.y,fim.x),255,1,8,0);
     //cout<< "Inicio "<<inicio.x<<" "<<inicio.y<<" Fim "<< fim.x<<" "<<fim.y<<"\n";
     }
 }
 for(int i = 0; i < vec3.size();i++){
     if(vec3[i].size()>5){
     Point inicio = vec3[i].front();
     Point fim = vec3[i].back();
     //line(saida1,inicio,fim,255,1,8,0);
     line(saida1,Point(inicio.y,inicio.x),Point(fim.y,fim.x),255,1,8,0);
     //cout<<"Vetor "<<i<<" inicio y "<<inicio.y<<" inicio x "<< inicio.x<<" fim y "<< fim.y<<" fim x "<< fim.x<<"\n";
     }
 }
 for(int i = 0; i < vec4.size();i++){
     if(vec4[i].size()>5){
     Point inicio = vec4[i].front();
     Point fim = vec4[i].back();
     //line(saida1,inicio,fim,255,1,8,0);
     line(saida1,Point(inicio.y,inicio.x),Point(fim.y,fim.x),255,1,8,0);
     }
 }
 cout << "Padrao 1 "<< contP1<< " Padrão 2 "<< contP2 << " Padrao 3 "<< contP3 << " Padrao 4 "<< contP4<<"\n";
 cout << "Padrao 1 "<< vec1.size()<< " Padrão 2 "<< vec2.size() << " Padrao 3 "<< vec3.size() << " Padrao 4 "<< vec4.size()<<"\n";

 t4 = clock();
 float diff3 = ((float)t4-(float)t1);
 float seconds3 = diff3 / CLOCKS_PER_SEC;
 cout << seconds3<<"\n";
 imwrite("edge.png",edge);

 //edge.convertTo(draw, CV_8UC1);

 String windowName = "Folha"; //Name of the window

 namedWindow(windowName,WINDOW_AUTOSIZE); // Create a window

 imshow(windowName, saida1); // Show our image inside the created window.
 //imshow(windowName, saida);

 waitKey(0); // Wait for any keystroke in the window

 destroyWindow(windowName); //destroy the created window

 return 0;
}