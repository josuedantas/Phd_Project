#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/tracking.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>


#define PI 3.14159265

using namespace cv;
using namespace std;

//calcula a quantidade pixels que pertence a folha em si
int calcAreaLeaf(Mat input){
    int total = 0;
    for(int i = 0; i < input.rows;i++){
        for(int j = 0; j < input.cols;j++){
            if((int)input.at<uchar>(i,j)>0){
                total++;
            }
        }
    }
    return total;
}

double getTemplateP(cv::Mat img,cv::Point p){
    double mediaR = 0;
         mediaR = (img.at<double>(p.x+99,p.y+99)-img.at<double>(p.x+99,p.y-1)-(img.at<double>(p.x-1,p.y+99)+
         img.at<double>(p.x-1,p.y-1)))/10000;
    return mediaR;
       
}

double getTemplate1(cv::Mat img,cv::Point p, int tam){
    double mediaR1 = 0,mediaR2 = 0;
         mediaR1 = (img.at<double>(p.x+(tam -1),p.y+1)-img.at<double>(p.x+(tam -1),p.y)-img.at<double>(p.x-1,p.y+1)+
         img.at<double>(p.x-1,p.y))/tam;
         //cout <<" Media 1 template 1 "<<mediaR1<<"\n";

         mediaR2 = (img.at<double>(p.x+(tam -1),p.y)-img.at<double>(p.x+(tam -1),p.y-1)-img.at<double>(p.x-1,p.y)+
         img.at<double>(p.x-1,p.y-1))/tam;
        // cout <<" Media 2 template 2 "<<mediaR2<<"\n";
    return mediaR1 - mediaR2;
       
}

double getTemplate2(cv::Mat img,cv::Point p,int tam){
    double mediaR1 = 0,mediaR2 = 0;
         mediaR1 = (img.at<double>(p.x,p.y+(tam -1))-img.at<double>(p.x-1,p.y+(tam -1))-img.at<double>(p.x,p.y-1)+
         img.at<double>(p.x-1,p.y-1))/tam;

         mediaR2 = (img.at<double>(p.x+1,p.y+(tam -1))-img.at<double>(p.x,p.y+(tam -1))-img.at<double>(p.x+1,p.y-1)+
         img.at<double>(p.x,p.y-1))/tam;
    return mediaR1 - mediaR2;
}

double getTemplate3(cv::Mat img,cv::Point p, int tam){
    double mediaR1 = 0,mediaR2 = 0;

         mediaR1 = (img.at<double>(p.x+tam-1,p.y+tam-1)-img.at<double>(p.x+(tam -2),p.y+tam)-
         img.at<double>(p.x-1,p.y-1)+img.at<double>(p.x-2,p.y))/(2*tam); // Branco 

         mediaR2 = (img.at<double>(p.x+tam,p.y+tam-2)-img.at<double>(p.x+tam-1,p.y+tam-1)-img.at<double>(p.x,p.y-2)+
         img.at<double>(p.x-1,p.y-1))/(2*tam); // Preto 

    return mediaR1 - mediaR2;
}

double getTemplate4(cv::Mat img,cv::Point p, int tam){
    double mediaR1 = 0,mediaR2 = 0;

            mediaR1 = (img.at<double>(p.x+tam,p.y)-img.at<double>(p.x+tam-1,p.y-1)-
            img.at<double>(p.x,p.y+tam)+img.at<double>(p.x-1,p.y+tam-1))/(2*tam); // Branco

            mediaR2 = (img.at<double>(p.x+tam+1,p.y+1)-img.at<double>(p.x+tam,p.y)-img.at<double>(p.x+1,p.y+tam+1)+
            img.at<double>(p.x,p.y+tam))/(2*tam); // Preto

    return mediaR1 - mediaR2;
}

void calcTemplate1(cv::Mat imgInt, cv::Mat& out){
int contV = 0, contE = 0, qtd = 0;
bool sem1 = false;
int fator = 0, jfinal = 0, ifinal = 0, tam = 3;
double dist = 0;
//std::vector< vector<Point> > vec;

       for(int j = 1; j < imgInt.cols - 2; j++){
         /*   if(j > jfinal){
               fator = 0;
           } */
         for(int i = 1; i < imgInt.rows - tam; i++){
      /*  if(getTemplateP(imgInt,Point(i,j)) ==0){
           i = i+100;
           fator = i - 1;
           jfinal = j +100;
           continue;
       } */
         if ((getTemplate1(imgInt,Point(i,j),tam)) > 200){
        //qtd++;
        out.at<uchar>(i,j) = 255;
        i += tam;
         }
     }
 }
}

void calcTemplate2(cv::Mat imgInt, Mat& out){
int contV = 0, qtd = 0;
bool sem1 = false;
int fator = 0, jfinal = 0, ifinal = 0, tam = 3;
double dist = 0;
std::vector< vector<Point> > vec;

 for(int i = 1; i < imgInt.rows - 2; i++){
     /* sem1 = false;
     if(i > ifinal){
               fator = 0;
           } */
     for(int j = 1; j < imgInt.cols - tam; j++){
         
         /* if(getTemplateP(imgInt,Point(i,j)) == 0){
           j = j+100;
           fator = j - 1;
           ifinal = i + 100;
           continue;
       } */
         if ((getTemplate2(imgInt,Point(i,j),tam)) > 200){
    //qtd++;
    out.at<uchar>(i,j) = 255;
    j += tam;
     }
 }
 }
}

void calcTemplate3(cv::Mat imgInt, Mat& out){
int contV = 0, qtd = 0;
bool sem1 = false;
int fator = 0, jfinal = 0, ifinal = 0, tam = 3;
double dist = 0;
std::vector< vector<Point> > vec;

 int sent = 0, aux = 2;
   for(int j = imgInt.cols - 4; j >= 2; j--){
      sent = j;

      for(int i = aux; (i < imgInt.rows - 4) && (sent <= imgInt.cols -4); i++){
     
         if ((getTemplate3(imgInt,Point(i,sent),tam)) > 90){
            // saida.at<uchar>(i,sent) = 255;
             //qtd++;
             out.at<uchar>(i,sent) = 255;
             i +=tam;
             sent +=tam;
         }
         sent++;
     }
 }
}

void calcTemplate4(cv::Mat imgInt, Mat& out){
int contV = 0, qtd = 0;
bool sem1 = false;
int fator = 0, jfinal = 0, ifinal = 0, aux = 6, sent = 0, tam = 3;
double dist = 0;
std::vector< vector<Point> > vec;

 for(int j = 2; j < imgInt.cols - 4; j++){
     sem1 = false;
     sent = j;

     for(int i = aux; (i < imgInt.rows - 4) && (sent >= 2); i++){

   if ((getTemplate4(imgInt,Point(i,sent),tam)) > 90){
   
             i += tam;
             sent -=tam;
             //saida.at<uchar>(i,sent) = 255;
             //qtd++;
             out.at<uchar>(i,sent) = 255;
         }
         sent--;
     }
}
}

Mat getVenation(string path){
int total1 = 0, total2 = 0, total3 = 0, total4 = 0;
double media = 0,var = 0, dp = 0,coef = 0;
int res = 0;
cv::Mat imgIntegral, imgInt2,imgInt45;
cv::Mat image = cv::imread(path);
cv::Mat reduzida;
resize(image,reduzida,Size(),0.25,0.25,CV_INTER_AREA); 
cv::Mat hsv_image, edge, edgeB;
 cvtColor( reduzida, hsv_image, CV_BGR2HSV );

 cv::Mat out (hsv_image.size(),CV_8UC1);
 std::vector<Mat> channels;
 split(hsv_image,channels);
 cv::Mat h = channels[0];
 cv::Mat v = channels[2];
 for(int i = 0; i < hsv_image.rows; i++){
     for(int j = 0; j < hsv_image.cols; j++){
         out.at<uchar>(i,j) = (((h.at<uchar>(i,j)+90)%360)/360+1-v.at<uchar>(i,j))/2;
     }
 }
 Canny( out, edge, 20, 40, 3);
 
 int esq=edge.cols, dir=0, cima=edge.rows, baixo=0; // primeiro pixel diferente de 255.
 for (int l=0; l<edge.rows; l++)
    for (int c=0; c<edge.cols; c++) {
        if ((int)edge.at<uchar>(l,c)==255) {
        if (c<esq) esq=c;
        if (dir<c) dir=c;
        if (l<cima) cima=l;
        if (baixo<l) baixo=l;
        }
 }
cv::Mat roi(edge, Rect(esq,cima,dir-esq+1,baixo-cima+1));

cv::Mat roiborda(out, Rect(esq,cima,dir-esq+1,baixo-cima+1));
Canny( roiborda, edgeB, 100, 200, 3);
 for(int i = 0; i < roi.rows;i++){
      for(int j = 0; j < roi.cols; j++){
          if((int)edgeB.at<uchar>(i,j)== 255){
            roi.at<uchar>(i,j) = 0;
          }
      }
  }
imwrite("ed03.png",roi);
integral(roi,imgIntegral,imgInt2,imgInt45,CV_64F);
Mat saida = Mat::zeros(roi.rows,roi.cols,CV_8UC1);
//saida.create(roi.rows,roi.cols,CV_8UC1);
    //double distance = 0;
calcTemplate1(imgIntegral,saida);
calcTemplate2(imgIntegral,saida);
calcTemplate3(imgInt45,saida);
calcTemplate4(imgInt45,saida);

/* media = (total1+total2+total3+total4)/4;
var = pow(total1-media,2);
var = var + pow(total2-media,2);
var = var + pow(total3-media,2);
var = var + pow(total4-media,2);
var = var / 4;
dp = sqrt(var); */

//cout<< path<<" "<<total1<<" "<<total2<<" "<<total3<<" "<<total4<<"\n";

//media = (total1+total2+total3+total4)/4;
/* if(total1 >= 50)
res++;
if(total2 >= 50)
res++;
if(total3 >= 50)
res++;
if(total4 >= 50)
res++; */

/* if(total1 > total2 && total1 > total3 && total1 > total4){
    coef = total1 / (total2 + total3 + total4);
}else if(total2 > total3 && total2 > total4){
   coef = total2 / (total1 + total3 + total4);
}else if(total3 > total4){
    coef = total3 / (total1 + total2 + total4);
}else{
    coef = total4 / (total1 + total2 + total3);
} */

/* if(tp == 0 && res >= 3){
    imwrite(path.substr(path.size()-8,path.size()-1)+".png",edge);
}else if(tp == 1 && res < 3){
    imwrite(path.substr(path.size()-8,path.size()-1)+".png",edge);
} */

return saida;
}

void splitLine(string l,string &num,int &tipo){
stringstream input(l);
input >> num;
input  >> tipo;
}

int main(int argc, char** argv)
{
int totalfolhas = 0, acertos = 0,fp = 0, fn = 0;

clock_t t1,t2,t3,t4;
t1=clock();
std::string path = "./LeavesSelection/1034.jpg";
std::string line, first;
int tipo;
std::ifstream myfile(path);
double media = 0,result = 0;
Mat output;

output = getVenation(path);
   /*  if(result <= 0.65  && tipo == 1){
        acertos++;
    }else if(result > 0.65 && tipo == 0){
        acertos++;
    } else if(result <= 0.65 && tipo == 0){
        fn++;
        //cout << first <<" "<<result<<"\n";
    }else if(result > 0.65 && tipo == 1){
        fp++;
    }
    totalfolhas++; */

 t4 = clock();
 float diff3 = ((float)t4-(float)t1);
 float seconds3 = diff3 / CLOCKS_PER_SEC;
 cout << seconds3<<"\n";
 cout << "Tempo processamento " << seconds3<<"\n";

 /* cout << "Total de acertos " << acertos<<"\n";
 cout << "Erros "<< totalfolhas-acertos<<"\n";
 cout << "Falsos positivos "<<fp<<"\n";
 cout << "Falsos negativos "<<fn<<"\n";

 cout << "Taxa de acerto "<< (acertos*100/totalfolhas)<<"%\n"; */

 imwrite("out03.png",output);
 //imwrite("saida.png",saida);

 //edge.convertTo(draw, CV_8UC1);

 String windowName = "Folha"; //Name of the window

 namedWindow(windowName,WINDOW_AUTOSIZE); // Create a window

 imshow(windowName, output); // Show our image inside the created window.
 //imshow(windowName, saida);

 waitKey(0); // Wait for any keystroke in the window

 destroyWindow(windowName); //destroy the created window */

 return 0;
}