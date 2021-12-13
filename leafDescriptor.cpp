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

//vector < vector<Point> > retangulos;
struct Regiao{
    Point inicio;
    int largura;
    int altura;
};

struct outliers{
    double dist;
    int ind;
};

vector<Regiao> regioes; 

struct No{
    int pos;
    No *quad1;
    No *quad2;
    No *quad3;
    No *quad4;
} *arvore;
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

void calcTemplate1(cv::Mat imgInt, cv::Mat &out){
int contV = 0, contE = 0, qtd = 0;
bool sem1 = false;
int fator = 0, jfinal = 0, ifinal = 0, tam = 5;
double dist = 0;
       for(int j = 1; j < imgInt.cols - 2; j++){
         for(int i = 1; i < imgInt.rows - tam; i++){
         if ((getTemplate1(imgInt,Point(i,j),tam)) > 200){
        out.at<uchar>(i,j) = 255;
        i += tam;
         }
     }
 }
}

void calcTemplate2(cv::Mat imgInt, Mat &out){
int contV = 0, qtd = 0;
bool sem1 = false;
int fator = 0, jfinal = 0, ifinal = 0, tam = 5;
double dist = 0;
std::vector< vector<Point> > vec;

 for(int i = 1; i < imgInt.rows - 2; i++){
     for(int j = 1; j < imgInt.cols - tam; j++){
         if ((getTemplate2(imgInt,Point(i,j),tam)) > 200){
    out.at<uchar>(i,j) = 255;
    j += tam;
     }
 }
 }
}

void calcTemplate3(cv::Mat imgInt, Mat &out){
int contV = 0, qtd = 0;
bool sem1 = false;
int fator = 0, jfinal = 0, ifinal = 0, tam = 5;
double dist = 0;
std::vector< vector<Point> > vec;

 int sent = 0, aux = 2;
   for(int j = imgInt.cols - 4; j >= 2; j--){
      sent = j;

      for(int i = aux; (i < imgInt.rows - 4) && (sent <= imgInt.cols -4); i++){
     
         if ((getTemplate3(imgInt,Point(i,sent),tam)) > 90){
             out.at<uchar>(i,sent) = 255;
             i +=tam;
             sent +=tam;
         }
         sent++;
     }
 }
}

void calcTemplate4(cv::Mat imgInt, Mat &out){
int contV = 0, qtd = 0;
bool sem1 = false;
int fator = 0, jfinal = 0, ifinal = 0, aux = 6, sent = 0, tam = 5;
double dist = 0;
std::vector< vector<Point> > vec;

 for(int j = 2; j < imgInt.cols - 4; j++){
     sem1 = false;
     sent = j;

     for(int i = aux; (i < imgInt.rows - 4) && (sent >= 2); i++){

   if ((getTemplate4(imgInt,Point(i,sent),tam)) > 90){
   
             i += tam;
             sent -=tam;
             out.at<uchar>(i,sent) = 255;
         }
         sent--;
     }
}
}

void insertNo(No *&root, int p){
    
          if ( root == NULL ) {
                 // The tree is empty.  Set root to point to a new node containing
                 // the new item.  This becomes the only node in the tree.
             root = new No();
             root -> pos = p;
                     // NOTE:  The left and right subtrees of root
                     // are automatically set to NULL by the constructor.
                     // This is important!
          }
          else if ( p == 1) {
             insertNo( root->quad1, 1 );
          } 
          else if ( p == 2) {
             insertNo( root->quad2, 2 );
          }
           else if ( p == 3) {
             insertNo( root->quad3, 3 );
          }
           else if ( p == 4) {
             insertNo( root->quad4, 4);
          }
}

double checkQuad(Mat img,Point ini, int lin, int col){
    bool result = false;
    double qtd_pontos = 0;
    qtd_pontos = (img.at<double>(ini.x+lin-1,ini.y+col-1) - img.at<double>(ini.x+lin-1,ini.y-1)-
    img.at<double>(ini.x-1,ini.y+col-1) + img.at<double>(ini.x-1,ini.y-1))/255.0;

    return qtd_pontos;
}

cv::Mat calculaJ(Mat input){
vector<cv::Mat> channel(3);
split(input,channel);
cv::Mat b = channel[0];
cv::Mat g = channel[1];
cv::Mat r = channel[2];
channel.clear();
//cv::Mat exg = Mat::zeros(Size(b.rows,b.cols),CV_8UC1);
//cv::Mat exr = Mat::zeros(Size(b.rows,b.cols),CV_8UC1);
cv::Mat j = Mat::zeros(Size(input.cols,input.rows),CV_8UC1);
for(int m = 0; m < b.rows; m++){
    for(int n =0; n < b.cols; n++){
        j.at<uchar>(m,n) = ((2*g.at<uchar>(m,n))-b.at<uchar>(m,n)-r.at<uchar>(m,n))-((2*r.at<uchar>(m,n))-b.at<uchar>(m,n)-g.at<uchar>(m,n));
    }
}
return j;
}

void findDico(Mat temp1, Mat temp2, Mat temp3, Mat temp4,Point ini, int lin,int col){
    double total1 = 0, total2 = 0, total3 = 0, total4 = 0, coef = 0, maior = 0, soma = 0;
    int cont = 0;
    total1  = checkQuad(temp1,ini,lin,col);
    total2 = checkQuad(temp2,ini,lin, col);
    total3 = checkQuad(temp3,ini,lin, col);
    total4 = checkQuad(temp4,ini,lin, col);

    if(total1 >= 1)
    cont++;
    if(total2 >= 1)
    cont++;
    if(total3 >= 1)
    cont++;
    if(total4 >= 1)
    cont++;
  
    if(cont >2){
        int lfac = lin /2, cfac = col /2;
        regioes.push_back({ini,lin,col});
        findDico(temp1,temp2,temp3,temp4,ini,lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x,ini.y+cfac),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y+cfac),lfac,cfac);
    }
}

cv::Mat getVenation(string path){
int total1 = 0, total2 = 0, total3 = 0, total4 = 0;
double media = 0,var = 0, dp = 0,coef = 0;
int res = 0;
cv::Mat image = cv::imread(path);
cv::Mat reduzida;
resize(image,reduzida,Size(),0.25,0.25,CV_INTER_AREA); 
cv::Mat hsv1;
cvtColor(reduzida, hsv1, CV_BGR2HSV);
int hbins = 30, sbins = 32;
int histSize[] = {hbins, sbins};
// hue varies from 0 to 179, see cvtColor
float hranges[] = { 0, 180 };
// saturation varies from 0 (black-gray-white) to
// 255 (pure spectrum color)
float sranges[] = { 0, 256 };
const float* ranges[] = { hranges, sranges };
MatND hist;
// we compute the histogram from the 0-th and 1-st channels
int channels[] = {0, 1};

calcHist( &hsv1, 1, channels, Mat(), // do not use mask
        hist, 2, histSize, ranges,
        true, // the histogram is uniform
        false );
//cout<<"Histograma  linhas "<<hist.rows<<" "<<hist.cols<<"\n";
double maxVal=0, thrS = 0, thrH = 0;
Point maxV;
minMaxLoc(hist, 0, &maxVal, &maxV, 0);
thrH = (180/30)*maxV.x;
thrS = (255/32)*maxV.y;

vector<Mat> channels1;
split(hsv1,channels1);
for(int i = 0; i < hsv1.rows; i++){
    for(int j =0;j < hsv1.cols; j++){
        if(channels1[0].at<uchar>(i,j)< thrH || channels1[0].at<uchar>(i,j)> (thrH+(180/30))){
            channels1[0].at<uchar>(i,j) = 179;
        }else{
            channels1[0].at<uchar>(i,j) = 0;
        }

        if(channels1[1].at<uchar>(i,j)< thrS || channels1[1].at<uchar>(i,j)> (thrS+(255/32))){
            channels1[1].at<uchar>(i,j) = 255;
        }else{
            channels1[1].at<uchar>(i,j) = 0;
        }
    }
}

merge(channels1, hsv1);
imshow("Hsv modificado", hsv1);

cout<<"Ponto valor máximo "<<maxV.x<<" "<<maxV.y<<"\n";



/* vector<Mat> bgr_planes;
split( reduzida, bgr_planes );
float range[] = { 0, 256 }; //the upper boundary is exclusive
const float* histRange = { range };
bool uniform = true, accumulate = false;
Mat b_hist, g_hist, r_hist;
calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
int hist_w = 512, hist_h = 400;
int bin_w = cvRound( (double) hist_w/histSize );
Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() ); */


/* cv::Mat ycbcr;
cv::cvtColor(reduzida, ycbcr, cv::COLOR_BGR2YCrCb);
vector<Mat> channel(3);
split(ycbcr, channel);
cv::Mat y = channel[0];
for(int i = 0; i < y.rows; i++){
    for(int j =0; j < y.cols; j++){
        if(y.at<uchar>(i,j)<170){
            y.at<uchar>(i,j)= 255;
        }else{
            y.at<uchar>(i,j)= 0;
        }
    }
}
imshow("y",y); */
//image.release();
cv::Mat j = calculaJ(reduzida);
cv::Mat outj = Mat::zeros(Size(j.cols,j.rows),CV_8UC3);
//cout<<"Tamanho j"<<j.rows<<" "<<j.cols<<"\n";
double t=threshold(j, j, 0, 255, THRESH_BINARY | THRESH_OTSU);
for(int i =0; i < outj.rows; i++){
    for(int l =0; l< outj.cols; l++){
        if(j.at<uchar>(Point(l,i))>t){
            outj.at<Vec3b>(i,l) = Vec3b(0,0,0);
        }else{
            outj.at<Vec3b>(i,l) = reduzida.at<Vec3b>(i,l);
        }
    }
}
//cout<<"Tamanho j"<<reduzida.rows<<" "<<reduzida.cols<<"\n";
//imshow("Teste", j);
imshow("Imagem J",outj);
/* cv::Mat hsv_image, edge, edgeB;
 cvtColor( reduzida, hsv_image, CV_BGR2HSV ); */

 
 /* std::vector<Mat> channels;
 split(hsv_image,channels);
 cv::Mat h = channels[0];
 cv::Mat s = channels[1];
 cv::Mat v = channels[2]; */
 cv::Mat edge, edgeB;
 Mat3b hsv;
 cv::cvtColor(outj, hsv, COLOR_BGR2HSV);
 cv::Mat out (hsv.size(),CV_8UC1);
 /* Vec3b hsvPixel;
 for(int i = 0; i < hsv.rows; i++){
     for(int j = 0; j < hsv.cols; j++){
         hsvPixel = hsv.at<Vec3b>(i,j);
         out.at<uchar>(i,j) = (((hsvPixel.val[0]+90)%360)/360+1-hsvPixel.val[2])/2;
         //cout<<"Matiz "<< (int)hsvPixel.val[0]<<"\n";
         //out.at<uchar>(i,j) = (((hsvPixel.val[0]- 60)/60+(1-hsvPixel.val[1]))+(1-(int)hsvPixel.val[2]))/3;
     }
 } */
//String windowName = "Folha"; //Name of the window

 //namedWindow(windowName,WINDOW_AUTOSIZE); // Create a window

 //imshow(windowName, out); // Show our image inside the created window.
 //imshow(windowName, saida);

 //waitKey(0); // Wait for any keystroke in the window

 //destroyWindow(windowName); //destroy the created window */
 //double t1=threshold(hsv, hsv, 0, 255, THRESH_BINARY | THRESH_OTSU);

 Canny( hsv1, edge, 150, 200, 3);
 imshow("Temp", edge);
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
/* cv::Mat roiborda(out, Rect(esq,cima,dir-esq+1,baixo-cima+1));
//double t2=threshold(roiborda, roiborda, 0, 255, THRESH_BINARY | THRESH_OTSU);
//Canny( roiborda, edgeB, t2, 255, 3);
 for(int i = 0; i < roi.rows;i++){
      for(int j = 0; j < roi.cols; j++){
          if((int)edgeB.at<uchar>(i,j)== 255){
            roi.at<uchar>(i,j) = 0;
          }
      }
  } */
cv::Mat imgIntegral, imgInt2,imgInt45, imgIntT1, imgIntT2, imgIntT3, imgIntT4;
integral(roi,imgIntegral,imgInt2,imgInt45,CV_64F);
cv::Mat temp1 = Mat::zeros(roi.rows,roi.cols,CV_8UC1);
cv::Mat temp2 = Mat::zeros(roi.rows,roi.cols,CV_8UC1);
cv::Mat temp3 = Mat::zeros(roi.rows,roi.cols,CV_8UC1);
cv::Mat temp4 = Mat::zeros(roi.rows,roi.cols,CV_8UC1);
cv::Mat alltemp = Mat::zeros(roi.rows,roi.cols,CV_8UC1);

calcTemplate1(imgIntegral,temp1);
calcTemplate2(imgIntegral,temp2);
calcTemplate3(imgInt45,temp3);
calcTemplate4(imgInt45,temp4);


for(int i =0; i < alltemp.rows; i++){
    for(int j =0; j < alltemp.cols; j++){
        if(temp1.at<uchar>(i,j) ==255 || temp2.at<uchar>(i,j) ==255 || temp3.at<uchar>(i,j) ==255 || temp4.at<uchar>(i,j) ==255){
            alltemp.at<uchar>(i,j) = 255;
        }
    }
}

/*imshow("Temp", alltemp);  
imshow("Temp 2", temp2); 
imshow("Temp 3", temp3); 
imshow("Temp 4", temp4);  */

integral(temp1,imgIntT1,CV_64F);
integral(temp2,imgIntT2,CV_64F);
integral(temp3,imgIntT3,CV_64F);
integral(temp4,imgIntT4,CV_64F);

findDico(imgIntT1,imgIntT2,imgIntT3,imgIntT4,Point(1,1),roi.rows,roi.cols);

int qtd_temp1 = 0, qtd_temp2 = 0, qtd_temp3 = 0, qtd_temp4 = 0, somaT = 0;


return roi;
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
//std::string path = "./LeavesSelection/Editadas/06.jpg";
std::string path = "./dataset_UFRA/DSCN0003.JPG";
std::string line, first;
int tipo, altura = 0, largura = 0, menor1 = 0,menor2 = 0,menor3 = 0;;
std::ifstream myfile(path);
double media = 0,result = 0, media_distancias = 0, desvio_padrao = 0;
cv::Mat output;
vector<Regiao> pontos_finais;
vector<Point2f> points_entrada, centers;
vector<double> dist2center;
double compactness = 0;

output = getVenation(path);

for(int i = 0; i < regioes.size();i++){
    Point p;
    int l = 0, c = 0;
    p = regioes[i].inicio;
    l = regioes[i].largura;
    c = regioes[i].altura;
    for(int j = 0; j < regioes.size();j++){
        Point p2;
        p2 = regioes[j].inicio;
        if(p2.x >= p.x && p2.x<= p.x+l && p2.y >= p.y && p2.y<= p.y+c && regioes[i].largura > regioes[j].largura
        && regioes[i].altura > regioes[j].largura){
            regioes.erase(regioes.begin()+i);
        }else if(p.x >= p2.x && p.x<= p2.x+regioes[j].largura && p.y >= p2.y && p.y<= p2.y+ regioes[j].largura
                 && regioes[i].largura > regioes[j].largura && regioes[i].altura > regioes[j].largura){
            regioes.erase(regioes.begin()+j);
        }
    }
}


for(int i = 0; i < regioes.size();i++){
        points_entrada.push_back(regioes.at(i).inicio);
       // cout<<regioes.at(i).inicio.x<<"\t"<<regioes.at(i).inicio.y<<"\n";
        //circle(output,Point(regioes.at(i).inicio.x,regioes.at(i).inicio.y),20,255,1,8,0);    
} 
Mat labels;
double anterior = 0, k = 1, atual = 0, taxa = 0;
do{
        anterior = atual;
        compactness = kmeans(points_entrada, k, labels,
            TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers);
            compactness = compactness / points_entrada.size();
            taxa = ((abs((anterior - compactness))*100)/anterior);
            cout << "Centroide "<<compactness<<" diferenca anterior "<<((abs((anterior - compactness))*100)/anterior)<<"\n";
            atual = compactness;
            k++;
}while(k<=2 || taxa >=100);
//while((abs(anterior - compactness)) > 1e+06);
compactness = kmeans(points_entrada, k-2, labels,
            TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers);

/*  for(int i = 0; i < centers.size();i++){
    circle(output,Point(centers[i].y,centers[i].x),20,255,1,8,0);
}  */

cout << "Quantidade "<<regioes.size()<<"\n";

/* for(int i = 0; i < points_entrada.size();i++){
    circle(output,Point(points_entrada[i].y,points_entrada[i].x),5,255,1,8,0);
}
 */

 t4 = clock();
 float diff3 = ((float)t4-(float)t1);
 float seconds3 = diff3 / CLOCKS_PER_SEC;
 cout << seconds3<<"\n";
 cout << "Tempo processamento " << seconds3<<"\n";

 imwrite("out01.png",output);
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