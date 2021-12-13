#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/tracking.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <bitset>


#define PI 3.14159265

using namespace cv;
using namespace std;

RNG rng(12345);

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

double getTemplate1(cv::Mat &img,cv::Point p, int tam){ //horizontal
    double mediaR1 = 0,mediaR2 = 0;
         mediaR1 = (img.at<double>(p.y+(tam -1),p.x+1)-img.at<double>(p.y+(tam -1),p.x)-img.at<double>(p.y-1,p.x+1)+
         img.at<double>(p.y-1,p.x))/tam;
         //cout <<" Media 1 template 1 "<<mediaR1<<"\n";

         mediaR2 = (img.at<double>(p.y+(tam -1),p.x)-img.at<double>(p.y+(tam -1),p.x-1)-img.at<double>(p.y-1,p.x)+
         img.at<double>(p.y-1,p.x-1))/tam;
        // cout <<" Media 2 template 2 "<<mediaR2<<"\n";
    return mediaR1 - mediaR2;
       
}

double getTemplate2(cv::Mat &img,cv::Point p,int tam){
    double mediaR1 = 0,mediaR2 = 0;
         mediaR1 = (img.at<double>(p.y,p.x+(tam -1))-img.at<double>(p.y-1,p.x+(tam -1))-img.at<double>(p.y,p.x-1)+
         img.at<double>(p.y-1,p.x-1))/tam;

         mediaR2 = (img.at<double>(p.y+1,p.x+(tam -1))-img.at<double>(p.y,p.x+(tam -1))-img.at<double>(p.y+1,p.x-1)+
         img.at<double>(p.y,p.x-1))/tam;
    return mediaR1 - mediaR2;
}

double getTemplate3(cv::Mat &img,cv::Point p, int tam){
    double mediaR1 = 0,mediaR2 = 0;

         mediaR1 = (img.at<double>(p.y+tam-1,p.x+tam-1)-img.at<double>(p.y+(tam -2),p.x+tam)-
         img.at<double>(p.y-1,p.x-1)+img.at<double>(p.y-2,p.x))/(tam); // Branco 

         mediaR2 = (img.at<double>(p.y+tam,p.x+tam-2)-img.at<double>(p.y+tam-1,p.x+tam-1)-img.at<double>(p.y,p.x-2)+
         img.at<double>(p.y-1,p.x-1))/(tam); // Preto 

    return mediaR1 - mediaR2;
}

double getTemplate4(cv::Mat &img,cv::Point p, int tam){
    double mediaR1 = 0,mediaR2 = 0;

            mediaR1 = (img.at<double>(p.y+tam,p.x)-img.at<double>(p.y+tam-1,p.x-1)-
            img.at<double>(p.y-1,p.x+tam+1)+img.at<double>(p.y-2,p.x+tam))/(tam); // Branco

            mediaR2 = (img.at<double>(p.y+tam+1,p.x)-img.at<double>(p.y+tam,p.x-1)-img.at<double>(p.y,p.x+tam+1)+
            img.at<double>(p.y-1,p.x+tam))/(tam); // Preto

    return mediaR1 - mediaR2;
}

bitset<32> calcJanLBPH(Mat &imgIntegral, Mat &imgInt45, int row, int col, int tamJan, int tamTemplate){
    std::bitset<32> lbph (std::string("0000000000000000000000000000000000000000"));
    std::bitset<32> lbphAux (std::string("0000000000000000000000000000000000000000"));
    std::bitset<32> lbph1 (std::string("0000000000000000000000000000000000000000")); 
    std::bitset<32> lbph2 (std::string("0000000000000000000000000000000000000000")); 
    std::bitset<32> lbph3 (std::string("0000000000000000000000000000000000000000")); 
    std::bitset<32> lbph4 (std::string("0000000000000000000000000000000000000000")); 
    for(int l = 0; l < tamJan; l++){
                if(getTemplate1(imgIntegral,Point(col, row+l),tamTemplate)>150){
                   lbph1<<=1;
                   lbph1.set(0,1);
                }else{
                    lbph1<<=1;
                   lbph1.set(0,0);
                }
                if(getTemplate2(imgIntegral,Point(col, row+l),tamTemplate)>150){
                   lbph2<<=1;
                   lbph2.set(0,1);
                }else{
                    lbph2<<=1;
                   lbph2.set(0,0);
                }
                if(getTemplate3(imgInt45,Point(col, row+l),tamTemplate)>150){
                   lbph3<<=1;
                   lbph3.set(0,1);
                }else{
                    lbph3<<=1;
                   lbph3.set(0,0);
                }
                if(getTemplate4(imgInt45,Point(col, row+l),tamTemplate)>150){
                   lbph4<<=1;
                   lbph4.set(0,1);
                }else{
                    lbph4<<=1;
                   lbph4.set(0,0);
                }
            }
            lbphAux = lbphAux | lbph1;
            lbphAux <<= 8;
            lbphAux = lbphAux | lbph2;
            lbphAux <<= 8;
            lbphAux = lbphAux | lbph3;
            lbphAux <<= 8;
            lbphAux = lbphAux | lbph4;

            lbph = lbph |= lbphAux;
    return lbph;
}

vector<Point> calcLBPH(Mat &m){
    std::bitset<32> lbph (std::string("0000000000000000000000000000000000000000"));
    vector<Point> pontos;
    cv::Mat imgIntegral, imgInt2,imgInt45;
   /*  std::bitset<24> lbphAux (std::string("00000000000000000000000000000000"));
    std::bitset<24> lbph1 (std::string("00000000000000000000000000000000")); 
    std::bitset<24> lbph2 (std::string("00000000000000000000000000000000")); 
    std::bitset<24> lbph3 (std::string("00000000000000000000000000000000")); 
    std::bitset<24> lbph4 (std::string("00000000000000000000000000000000")); 
    int cont = 0;
    cv::Mat imgIntegral, imgInt2,imgInt45;
    vector<Point> pontos;
    integral(m,imgIntegral,imgInt2,imgInt45,CV_64F); */
    Mat temp1 = Mat::zeros(Size(m.cols,m.rows),CV_8UC1);
    /* Mat temp2 = Mat::zeros(Size(m.cols,m.rows),CV_8UC1);
    Mat temp3 = Mat::zeros(Size(m.cols,m.rows),CV_8UC1);
    Mat temp4 = Mat::zeros(Size(m.cols,m.rows),CV_8UC1); */
    integral(m,imgIntegral,imgInt2,imgInt45,CV_64F);

    /* for (int i = 0; i < m.rows-10; i++){
        for(int j = 0; j < m.cols-10; j++){
            if(getTemplate1(imgIntegral,Point(j, i),10)>150){
                temp1.at<uchar>(i,j) = 255;
            }
             if(getTemplate2(imgIntegral,Point(j, i),10)>150){
                temp1.at<uchar>(i,j) = 255;
            }
             if(getTemplate3(imgInt45,Point(j, i),10)>150){
                temp1.at<uchar>(i,j) = 255;
            }
             if(getTemplate4(imgInt45,Point(j, i),10)>150){
                temp1.at<uchar>(i,j) = 255;
            }
        }
    }
    imshow("temp1", temp1); */
    for (int i = 10; i < m.rows-10; i+=10){
        for(int j =10; j< m.cols-10; j+=10){
            lbph.reset();
            lbph = lbph |= calcJanLBPH(imgIntegral, imgInt45, i, j,10,10);
            lbph = lbph |= calcJanLBPH(imgIntegral, imgInt45, i+10, j,10,10);
            lbph = lbph |= calcJanLBPH(imgIntegral, imgInt45, i, j+10,10,10);
            lbph = lbph |= calcJanLBPH(imgIntegral, imgInt45, i+10, j+10,10,10);
            //cout <<lbph<<"\n";
            if(lbph.count()>=13){
                //cout <<lbph<<"\n";
                    pontos.push_back(Point(j+10,i+10));
            }
            /* lbph1.reset();
            lbph2.reset();
            lbph3.reset();
            lbph4.reset();
            lbphAux.reset();
            for(int l = 0; l < 10; l++){
                if(getTemplate1(imgIntegral,Point(i+l,j),10)>200){
                   lbph1<<=1;
                   lbph1.set(0,1);
                }else{
                    lbph1<<=1;
                   lbph1.set(0,0);
                }
                if(getTemplate2(imgIntegral,Point(i+l,j),10)>200){
                   lbph2<<=1;
                   lbph2.set(0,1);
                }else{
                    lbph2<<=1;
                   lbph2.set(0,0);
                }
                if(getTemplate3(imgInt45,Point(i+l,j),10)>150){
                   lbph3<<=1;
                   lbph3.set(0,1);
                }else{
                    lbph3<<=1;
                   lbph3.set(0,0);
                }
                if(getTemplate4(imgInt45,Point(i+l,j),10)>150){
                   lbph4<<=1;
                   lbph4.set(0,1);
                }else{
                    lbph4<<=1;
                   lbph4.set(0,0);
                }
            }
            lbphAux = lbphAux | lbph1;
            lbphAux <<= 4;
            lbphAux = lbphAux | lbph2;
            lbphAux <<= 4;
            lbphAux = lbphAux | lbph3;
            lbphAux <<= 4;
            lbphAux = lbphAux | lbph4;

            lbph = lbph |= lbphAux;
            
            cont++;
            if(cont%4==0){
                if(lbph.count()>=16){
                    pontos.push_back(Point(i-8,j-6));
                }
                //cout<<lbph<<"\n";
                lbph.reset();
                cont = 0;
            } */
          /*   for(int l = 0; l < 6; l++){
                if(getTemplate2(imgIntegral,Point(i,j+l),6)>200){
                    lbph += pow(2.0,cont);
                    cont++;
                    break;
                }
            }
            for(int l = 0; l < 6; l++){
                if(getTemplate3(imgInt45,Point(i,j+l),6)>90){
                    lbph += pow(2.0,cont);
                    cont++;
                    break;
                }
            }
            for(int l = 0; l < 6; l++){
                if(getTemplate4(imgInt45,Point(i,j+l),6)>90){
                    lbph += pow(2.0,cont);
                    cont++;
                    break;
                }
                }
            if(lbph >= 3){
                pontos.push_back(Point(i,j));
            }
            cout<<"lpbh "<<lbph<<"\n"; */
        }
    }
    return pontos;
}

cv::Mat calculaJ(Mat &input){
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

cv::Mat getVenation(string path){
    int total1 = 0, total2 = 0, total3 = 0, total4 = 0;
    double media = 0,var = 0, dp = 0,coef = 0;
    int res = 0;
    cv::Mat image = cv::imread(path);
    cv::Mat mask;
    cv::Mat reduzida,bgdModel, fgdModel;
   
    resize(image,reduzida,Size(),0.20,0.20,CV_INTER_AREA);

    //cvtColor(reduzida, reduzida, COLOR_RGBA2RGB, 0);

    /*  Rect rect(50,50,reduzida.cols-50,reduzida.rows-50);

    cv::grabCut(reduzida, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);

    cv::compare(mask,cv::GC_PR_FGD,mask,cv::CMP_EQ); */

    //cv::Mat foreground(reduzida.size(),CV_8UC3,cv::Scalar(255,255,255));
   // image.copyTo(foreground,mask);
    //imshow("output",mask);

 /*    for (int i = 0; i < reduzida.rows; i++) {
    for (int j = 0; j < reduzida.cols; j++) {
        if (mask.at<cv::Vec3b>(Point(i,j))[0] == 0) {
            reduzida.at<cv::Vec3b>(Point(i,j))[0] = 0;
            reduzida.at<cv::Vec3b>(Point(i,j))[1] = 0;
            reduzida.at<cv::Vec3b>(Point(i,j))[2] = 0;
        }
    }
} */

    //cv::Mat j = calculaJ(reduzida);
   
    cv::Mat outj = Mat::zeros(Size(reduzida.cols,reduzida.rows),CV_8SC1);
    //cout<<"Tamanho j"<<j.rows<<" "<<j.cols<<"\n";
    //cv::threshold(reduzida, outj, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    
    /* double t=threshold(j, j, 0, 255, THRESH_BINARY | THRESH_OTSU);
    for(int i =0; i < outj.rows; i++){
        for(int l =0; l< outj.cols; l++){
            if(j.at<uchar>(Point(l,i))>t){
                outj.at<Vec3b>(i,l) = Vec3b(0,0,0);
            }else{
                outj.at<Vec3b>(i,l) = reduzida.at<Vec3b>(i,l);
            }
        }
    } */

/*     for(int i =0; i < outj.rows; i++){
        for(int l =0; l< outj.cols; l++){
            if(outj.at<uchar>(Point(l,i))<255){
                reduzida.at<int>(l,i) = (int)Scalar(0,0,0);
        }
    }
    } */

     //imshow("J",outj);

    /* cv::Mat edge, edgeB;
    Mat3b hsv;
    Mat gray;
    cv::cvtColor(reduzida, hsv, COLOR_BGR2HSV);
    GaussianBlur( reduzida, reduzida, Size( 5, 5 ), 0, 0 );
    cv::cvtColor(reduzida, gray, COLOR_BGR2GRAY);

    cv::Mat out (hsv.size(),CV_8UC1);


    Mat grad_x, grad_y, grad;
    Mat abs_grad_x, abs_grad_y;

    Sobel(hsv, grad_x, CV_8UC1, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    Sobel(hsv, grad_y, CV_8UC1, 0, 1, 3, 1, 0, BORDER_DEFAULT);


    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);

    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad); */

    //imshow("Sobel", grad);
    /* cv::Mat hsv_image;
    cvtColor(reduzida,hsv_image, COLOR_BGR2HSV);
    vector<cv::Mat> hsvPixel;
    cv::split(hsv_image, hsvPixel);
    for(int i = 0; i < hsv_image.rows; i++){
        for(int j = 0; j < hsv_image.cols; j++){
            outj.at<schar>(i,j) = (((hsvPixel[0].at<schar>(i,j)+90)%360)/360+1-hsvPixel[2].at<schar>(i,j))/2;
        }
    }
    
    outj.convertTo(outj,CV_8UC1); */
    
    // imshow("Filtro", outj);

    //double t1=threshold(outj, outj, 0, 255, THRESH_BINARY | THRESH_OTSU);
   
     /* Mat gray;
    GaussianBlur( reduzida, reduzida, Size( 5, 5 ), 0, 0 );
    cv::cvtColor(reduzida, gray, COLOR_BGR2GRAY); */
     

    cv::Mat edge, edgeB, mediaImg, gausImg;
    Mat3b hsv;
    Mat gray1, gray2;
    //cv::GaussianBlur( reduzida, gausImg, Size( 5, 5 ), 0, 0 );
    //cv::medianBlur( reduzida, mediaImg, 13);
    cv::cvtColor(reduzida, gray2, COLOR_BGR2GRAY);
    //cv::cvtColor(outj, gray2, COLOR_BGR2GRAY);
    //cv::cvtColor(mediaImg, gray1, COLOR_BGR2GRAY);
    
    //Canny( gray1, edge, 100, 150, 3);

    Canny( gray2, edgeB, 50, 250, 3);

   /*  int contBorda = 0;
    Point aux;
    cv::Mat test = Mat::zeros(Size(edge.cols,edge.rows),CV_8UC1);
    for(int i = 0; i < edge.rows-3; i++){
        for(int j = 0; j < edge.cols-3; j++){
            if(edge.at<uchar>(i,j)==255 && edgeB.at<uchar>(i,j)==255){
               edgeB.at<uchar>(i,j)= 0;
               Point p = Point(i+1,j);
               contBorda = 1;
               while(contBorda >0){
                contBorda = 0;
                for(int l = p.x; l < p.x +3; l++){
                    for(int m = p.y; m < p.y+3; m++){
                        if(edgeB.at<uchar>(l,m)==255){
                            contBorda++;
                            edgeB.at<uchar>(l,m)= 0;
                            test.at<uchar>(l,m)= 255;
                            aux = Point(l+1,m);
                        }
                    }
                }
                p = aux;
               }

            }
        }
    } */

    //imshow("Contornos",edgeB);
    //imshow("Temp", test);

    //cv::inRange(hsv, Scalar(17, 75, 80), Scalar(360/2, 255, 255), edgeB);

    //imshow("Contornos",edgeB);
    //imshow("Temp", edge);

     /* vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours( edge, contours,hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0) );

    Mat drawing = Mat::zeros( edge.size(), CV_8UC1 );
    Vec4i auxH; */
    /*  for( int i = 0; i< hierarchy.size(); i++ ){
       auxH = hierarchy.at(i);
       if(auxH[3]==-1)
       contours.erase(contours.begin()+i);
     } */
     /* vector<Point> contornos;
      for( int i = 0; i< contours.size(); i++ ){
        contornos = contours.at(i);
        for(int l = 0; l < contornos.size(); l++){
            Point p = contornos.at(l);
            drawing.at<uchar>(p) = 255;
        }
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, Scalar(255), 1, 8,hierarchy, 0, Point() );
     } */

/*     for(int i = 0; i < grad_x.rows; i++){
        for(int j = 0; j < grad_x.cols; j++){
            if((abs_grad_x.at<int>(i,j)== 0 && abs_grad_x.at<int>(i+1,j)> 0)){
                edge.at<uchar>(i+1,j) = 0;
            }
            if(abs_grad_x.at<int>(i,j)== 0 && abs_grad_x.at<int>(i,j+1)> 0){
                edge.at<uchar>(i,j+1) = 0;
            }
        }
    } */
    //imshow("Temp", drawing);
   // Mat img_xor = Mat::zeros( edge.size(), CV_8UC1 );
    //cv::bitwise_xor(edge,drawing,img_xor);
    //imshow("Xor",img_xor);
    int esq=edgeB.cols, dir=0, cima=edgeB.rows, baixo=0; // primeiro pixel diferente de 255.
    for (int l=0; l<edgeB.rows; l++)
        for (int c=0; c<edgeB.cols; c++) {
            if ((int)edgeB.at<uchar>(l,c)==255) {
            if (c<esq) esq=c;
            if (dir<c) dir=c;
            if (l<cima) cima=l;
            if (baixo<l) baixo=l;
            }
    }
     cv::Mat roi(edgeB, Rect(esq,cima,dir-esq+1,baixo-cima+1));
    
     //imshow("Teste 10", drawing);

    vector<Point> pts;
    //Mat outp1 = Mat::zeros(Size(roi.cols,roi.rows),CV_8UC1);
    pts = calcLBPH(roi);
    for(int i = 0; i < pts.size();i++){
        Point p = pts.at(i);
        //outp1.at<uchar>(p.x,p.y)=255;
        circle(roi,Point(p.x, p.y),20,255,1,8,0);
    } 
    //imshow("LBPH",outp1);
   return roi;
}



int main(int argc, char** argv)
{
clock_t t1,t2,t3,t4;
t1=clock();
std::string path = "./LeavesSelection/Editadas/sofolha.JPG";
//std::string path = "./LeavesSelection/Editadas/semfundo2.JPG";
//std::string path = "./dataset_UFRA/DSCN0003.JPG";
cv::Mat output;
vector<Point2f> centers;

output = getVenation(path);


/*  for(int i = 0; i < centers.size();i++){
    circle(output,Point(centers[i].y,centers[i].x),20,255,1,8,0);
} */ 


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