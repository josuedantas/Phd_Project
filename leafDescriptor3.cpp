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
         mediaR1 = (img.at<double>(p.x+7,p.y+1)-img.at<double>(p.x+7,p.y)-(img.at<double>(p.x-1,p.y+1)-
         img.at<double>(p.x-1,p.y)))/8;

         mediaR2 = (img.at<double>(p.x+7,p.y)-img.at<double>(p.x+7,p.y-1)-(img.at<double>(p.x-1,p.y)-
         img.at<double>(p.x-1,p.y-1)))/8;
    return mediaR1 - mediaR2;
       
}

double getTemplate2(Mat& img,Point p){
    double mediaR1 = 0,mediaR2 = 0;
         mediaR1 = (img.at<double>(p.x,p.y+7)-img.at<double>(p.x-1,p.y+7)-(img.at<double>(p.x,p.y-1)-
         img.at<double>(p.x-1,p.y-1)))/8;

         mediaR2 = (img.at<double>(p.x+1,p.y+7)-img.at<double>(p.x,p.y+7)-(img.at<double>(p.x+1,p.y-1)-
         img.at<double>(p.x,p.y-1)))/8;
    return mediaR1 - mediaR2;
}

double getTemplate3(Mat& img,Point p){
    double mediaR1 = 0,mediaR2 = 0;
       /*   mediaR1 = (img.at<double>(p.x+2,p.y+2)-img.at<double>(p.x+1,p.y+3)-img.at<double>(p.x-1,p.y-1)+
         img.at<double>(p.x-2,p.y))/8; // Branco

         mediaR2 = (img.at<double>(p.x+3,p.y+1)-img.at<double>(p.x+2,p.y+2)-img.at<double>(p.x,p.y-2)+
         img.at<double>(p.x-1,p.y-1))/8; // Preto */

         mediaR1 = (img.at<double>(p.x+4,p.y+4)-img.at<double>(p.x+3,p.y+5)-img.at<double>(p.x-1,p.y-1)+
         img.at<double>(p.x-2,p.y))/12; // Branco 

         mediaR2 = (img.at<double>(p.x+5,p.y+3)-img.at<double>(p.x+4,p.y+4)-img.at<double>(p.x,p.y-2)+
         img.at<double>(p.x-1,p.y-1))/12; // Preto 

    return mediaR1 - mediaR2;
}

double getTemplate4(Mat& img,Point p){
    double mediaR1 = 0,mediaR2 = 0;
            /* mediaR1 = (img.at<double>(p.x+2,p.y)-img.at<double>(p.x+1,p.y-1)-img.at<double>(p.x-1,p.y+3)+
            img.at<double>(p.x-2,p.y+2))/8; // Branco

            mediaR2 = (img.at<double>(p.x+4,p.y)-img.at<double>(p.x+3,p.y-1)-img.at<double>(p.x+1,p.y+3)+
            img.at<double>(p.x,p.y+2))/8; // Preto */

            mediaR1 = (img.at<double>(p.x+4,p.y-2)-img.at<double>(p.x+3,p.y-3)-img.at<double>(p.x-1,p.y+3)+
            img.at<double>(p.x-2,p.y+2))/12; // Branco

            mediaR2 = (img.at<double>(p.x+6,p.y-2)-img.at<double>(p.x+5,p.y-3)-img.at<double>(p.x+1,p.y+3)+
            img.at<double>(p.x,p.y+2))/12; // Preto

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
 Mat image = imread("./Leaves/1001.jpg");
 //Mat image = imread("./Leaves/1002.jpg");
 //Mat image = imread("./Leaves/1060.jpg");

 resize(image,image,Size(),0.45,0.45,CV_INTER_AREA);

 Mat hsv_image;
 cvtColor( image, hsv_image, CV_BGR2HSV );

 //imwrite("hsv.png",hsv_image);

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

 imwrite("hsv2.png",hsv_image);
    t3 = clock();
 float diff2 = ((float)t3-(float)t2);
 float seconds2 = diff2 / CLOCKS_PER_SEC;
 cout << seconds2<<"\n";

 //resize(out,out,Size(),0.45,0.45,CV_INTER_LINEAR);

 Canny( out, edge, 20, 40, 3);


//imwrite("canny.png",edge);

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

 Mat roi(edge, Rect(esq,cima,dir-esq+1,baixo-cima+1));

 Mat roiborda(out, Rect(esq,cima,dir-esq+1,baixo-cima+1));

 Mat edgeB;

 Canny( roiborda, edgeB, 100, 200, 3);

 

 for(int i = 0; i < roi.rows;i++){
      for(int j = 0; j < roi.cols; j++){
          if((int)edgeB.at<uchar>(i,j)== 255){
            roi.at<uchar>(i,j) = 0;
          }
      }
  }

 
 /* Mat tst = cv::Mat::zeros(roi.size(),CV_8UC1);
 tst = 0;
 vector<vector<Point> > contornos;
 findContours( roi, contornos,RETR_TREE, CHAIN_APPROX_SIMPLE);
 for(int i = 0; i < contornos.size();i++){
     vector<Point> pontos = contornos.at(i);
     for(int j =0; j < pontos.size(); j++){
         Point p = pontos.at(j);
         tst.at<int>(p.y,p.x) = 255;
     }
 } */

 Mat saida(roi.size(),CV_8UC1);
    double mediaR1 = 0,mediaR2 = 0, dist = 0;
    int contP1 = 0, contP2 = 0, contP3 = 0, contP4 = 0;
 bool sem = false;

 Mat imgIntegral, imgInt2,imgInt45;
    integral(roi,imgIntegral,imgInt2,imgInt45,CV_64F);

 // se não encontrou em uma parte da imagem não há motivos para buscar no todo
int contV = 0, contE = 0;
bool sem1 = false;

 /* for(int i = 1; i < roi.rows - 6; i++){
     sem = false;
     for(int j = 1; j < roi.cols - 2; j++){  */
     for(int j = 1; j < roi.cols - 2; j++){
         for(int i = 1; i < roi.rows - 6; i++){
       if((int)roi.at<uchar>(i+1,j+1) == 0 && (int)roi.at<uchar>(i+2,j+1) == 0){
           continue;
       }
         if ((getTemplate1(imgIntegral,Point(i,j))) > 200){
             /* cout << "P0 "<<roi.at<int>(i,j+1)<<"\n";
             cout <<"P1 "<< roi.at<int>(i-1,j)<<"\n";
             cout <<"P2 "<< roi.at<int>(i,j)<<"\n"; */
             if(vec1.size()==0){
                    vec1.push_back(vector<Point>());
                    vec1[vec1.size()-1].push_back(Point(i,j+1));
             }else{
                 //Point aux = vec1[vec1.size()-1].back();
                 for(int k = vec1.size() - 1; k >=0; k--){
                 /* sem1 = false;
                 Point aux = vec1[k].back();
                 int x1 = 0 , y1 = 0, x2 = 0, y2 = 0;
                 x1 = aux.y;
                 y1 = aux.x;
                 x2 = j+1;
                 y2 = i;
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2));
                 if(dist <= 6){
                     sem1 = true;
                     //vec1[k].pop_back();
                     vec1[k].push_back(Point(i,j+1));
                     break;
                 } */
                 /* int x1 = 0 , y1 = 0, x2 = 0, y2 = 0;
                 x1 = aux.y;
                 y1 = aux.x;
                 x2 = j+1;
                 y2 = i;
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2));
                 if(dist <= 6){
                     vec1[vec1.size()-1].push_back(Point(i,j+1));
                 } */
                Point aux = vec1[k].back();
                  
                int x1 = 0 , y1 = 0, x2 = 0, y2 = 0;
                 x1 = 0;
                 y1 = aux.x -7;
                 x2 = j - 1 - (aux.y - 1);
                 y2 = i - 7;
                 double ang = 0;
                 ang = acos(((x1*x2)+(y1*y2))/(sqrt((pow(x1,2)+pow(y1,2)))*sqrt(pow(x2,2)+pow(y2,2))));
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2));
                 if((ang*180/PI)>0 && (ang*180/PI) < 10 && dist < 20){
                     sem1 = true;
                     vec1[k].push_back(Point(i,j+1));
                     break;
                 }
                 }
                 if(sem1 == false){
                     contV++;
                     vec1.push_back(vector<Point>());
                     vec1[vec1.size()-1].push_back(Point(i,j+1));
                 }
            
             }
           // cout << mediaR1 - mediaR2 <<"\n";
           // dir1.push_back(Point(i,j));
        saida.at<uchar>(i,j) = 255;
        contP1++;
        i += 6;
             
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
         if((int)roi.at<uchar>(i+1,j+1) == 0 && (int)roi.at<uchar>(i+1,j+2) == 0){
           continue;
       }
         if ((getTemplate2(imgIntegral,Point(i,j))) > 200){
           // cout << mediaR1 - mediaR2 <<"\n";
           // dir2.push_back(Point(i,j));
           if(vec2.size()==0){
                 vec2.push_back(vector<Point>());
                 vec2[contV].push_back(Point(i,j+5));
             }else{
                 for(int k = vec2.size()-1; k >= 0; k--){
                 sem1 = false;
                 Point aux = vec2[k].back();
                 int x1 = 0 , y1 = 0, x2 = 0, y2 = 0;
                 /* x1 = aux.y -4;
                 y1 = 0;
                 x2 = j - 4;
                 y2 = i - 1 - (aux.x - 1); */

                 x1 = 0;
                 y1 = aux.x -7;
                 x2 = j - 1 - (aux.y - 1);
                 y2 = i - 7;
                 double ang = 0;
                 /* x1 = aux.y;
                 y1 = aux.x;
                 x2 = j;
                 y2 = i;
                 double ang = 0; */
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2));
                 ang = acos(((x1*x2)+(y1*y2))/(sqrt((pow(x1,2)+pow(y1,2)))*sqrt(pow(x2,2)+pow(y2,2))));
                 /* if(dist <= 6){
                     sem1 = true;
                     //vec2[k].pop_back();
                     vec2[k].push_back(Point(i,j+5));
                     //cout<< "Vetor "<<k<<" Ponto "<<i<<" "<<j<<"\n";
                     break;
                 } */
                 if((ang*180/PI)>0 && (ang*180/PI) < 10 && dist < 20){
                     sem1 = true;
                     vec2[k].push_back(Point(i,j+5));
                     break;
                 }
                 }
                 if(sem1 == false){
                     contV++;
                     vec2.push_back(vector<Point>());
                     vec2[vec2.size()-1].push_back(Point(i,j+5));
                 }
         }
    saida.at<uchar>(i,j) = 255;
    contP2++;
    j += 6;
     }
 }
 }

 contV = 0;
 sem1 = false;
 double mediaR3 = 0;
 double d3 = 0;

/*  for(int i = 2; i < roi.rows - 4; i++){
     sem = false;
     for(int j = 2; j < roi.cols - 4; j++){
 */
 int sent = 0, aux = 2;
  for(int j = roi.cols - 4; j >= 2; j--){
      sent = j;
      if(j==2 && aux < roi.rows - 4){
          j++;
          aux++;
      }
      for(int i = aux; (i < roi.rows - 4) && (sent <= roi.cols -4); i++){
       /*  if(roi.at<int>(i+1,j+1) == 0 && roi.at<int>(i+2,j+2) == 0 && roi.at<int>(i+3,j+3) == 0){
           continue;
       } */

         if ((getTemplate3(imgInt45,Point(i,sent))) > 90){
           // cout << mediaR1 - mediaR2 <<"\n";
            // dir3.push_back(Point(i,j));
             saida.at<uchar>(i,sent) = 255;
             contP3++;
             if(vec3.size()==0){
                vec3.push_back(vector<Point>());
                vec3[contV].push_back(Point(i+4,sent+4));
             }else{
                 for(int k = vec3.size()-1; k >= 0; k--){
                 sem1 = false;
                 Point aux = vec3[k].back();
                 int x1 = 0 , y1 = 0, x2 = 0, y2 = 0, descX = 0, descY=0;
                 descX = aux.y - 4;
                 descY = aux.x - 4;
                 x1 = aux.y - descX;
                 y1 = aux.x - descY;
                 x2 = sent - descX;
                 y2 = i - descY;
                 double ang = 0;
           /*       x1 = aux.y;
                 y1 = aux.x;
                 x2 = sent;
                 y2 = i;*/
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2)); 
                 ang = acos(((x1*x2)+(y1*y2))/(sqrt((pow(x1,2)+pow(y1,2)))*sqrt(pow(x2,2)+pow(y2,2))));
                if((ang*180/PI)>-10 && (ang*180/PI) < 10 && dist < 20){
                //if(dist <= 6){
                     sem1 = true;
                     vec3[k].push_back(Point(i+4,sent+4));
                     break;
                 }
                /*  if(dist <= 6){
                     sem1 = true;
                     //vec3[k].pop_back();
                     vec3[k].push_back(Point(i+4,sent-4));
                     break;
                 } */
                 }
                 if(sem1 == false){
                     contV++;
                     vec3.push_back(vector<Point>());
                     vec3[contV].push_back(Point(i+4,sent+4));
                 }
             }
             i +=4;
             sent +=4;
         }
         sent++;
     }
 }
/* for(int i = 2; i < roi.rows - 4; i++){
     sem = false;
     for(int j = 2; j < roi.cols - 4; j++){
 */
 aux = 2;
 for(int j = 2; j < roi.cols - 4; j++){
     sem = false;
     sent = j;
      if((j== roi.cols -5) && (aux < roi.rows - 4)){
          j--;
          aux++;
      }
     for(int i = aux; (i < roi.rows - 4) && (sent >= 2); i++){
       /*  if(roi.at<int>(i+1,j+1) == 0 && roi.at<int>(i,j) == 0 && roi.at<int>(i-1,j-1) == 0){
           continue;
       }
 */
   if ((getTemplate4(imgInt45,Point(i,sent))) > 90){
           // cout << mediaR1 - mediaR2 <<"\n";
            // dir4.push_back(Point(i,j));
            if(vec4.size()==0){
                vec4.push_back(vector<Point>());
                vec4[0].push_back(Point(i+2,sent));
             }else{
                 for(int k = vec4.size() - 1; k >=0; k--){
                 sem1 = false;
                 Point aux = vec4[k].back();
                 int x1 = 0 , y1 = 0, x2 = 0, y2 = 0, descX = 0, descY = 0;
                 descX = aux.y + 4;
                 descY = aux.x - 4;
                 x1 = aux.y - descX;
                 y1 = aux.x - descY;
                 x2 = sent - descX;
                 y2 = i - descY+2;
                 double ang = 0;
/*                  x1 = aux.y;
                 y1 = aux.x;
                 x2 = sent;
                 y2 = i;*/
                 dist = sqrt(pow(x2 - x1,2)+pow(y2 - y1,2)); 
                 ang = acos(((x1*x2)+(y1*y2))/(sqrt((pow(x1,2)+pow(y1,2)))*sqrt(pow(x2,2)+pow(y2,2))));
                if((ang*180/PI)>0 && (ang*180/PI) < 10 && dist < 20){
                     sem1 = true;
                     vec4[k].push_back(Point(i+2,sent));
                     break;
                 }
                 /* if(dist <= 6){
                     sem1 = true;
                     //vec4[k].pop_back();
                     vec4[k].push_back(Point(i+4,sent+2));
                     break;
                 } */
                 }
                 if(sem1 == false){
                    // contV++;
                     vec4.push_back(vector<Point>());
                     vec4[vec4.size()-1].push_back(Point(i+2,sent));
                 }
             }
             i += 4;
             sent -=4;
             saida.at<uchar>(i,sent) = 255;
             contP4++;
             //sem = true;
         }
         sent--;
     }
}

 Mat saida1(roi.rows,roi.cols,CV_8UC1);
double distance = 0;

 for(int i = 0; i < vec1.size();i++){
     if(vec1[i].size()>1){
     Point inicio = vec1[i].front();
     Point fim = vec1[i].back();
     line(saida1,Point(inicio.y,inicio.x),Point(fim.y,fim.x),255,1,8,0);
     }
 }
 for(int i = 0; i < vec2.size();i++){
     if(vec2[i].size()>1){
     Point inicio = vec2[i].front();
     Point fim = vec2[i].back();
    // line(saida1,inicio,fim,255,1,8,0);
    line(saida1,Point(inicio.y,inicio.x),Point(fim.y,fim.x),255,1,8,0);
     //cout<< "Inicio "<<inicio.x<<" "<<inicio.y<<" Fim "<< fim.x<<" "<<fim.y<<"\n";
     }
 }
 for(int i = 0; i < vec3.size();i++){
     if(vec3[i].size()>1){
     Point inicio = vec3[i].front();
     Point fim = vec3[i].back();
     //line(saida1,inicio,fim,255,1,8,0);
     line(saida1,Point(inicio.y,inicio.x),Point(fim.y,fim.x),255,1,8,0);
     //cout<<"Vetor "<<i<<" inicio y "<<inicio.y<<" inicio x "<< inicio.x<<" fim y "<< fim.y<<" fim x "<< fim.x<<"\n";
     }
 }
 for(int i = 0; i < vec4.size();i++){
     if(vec4[i].size()>1){
     Point inicio = vec4[i].front();
     Point fim = vec4[i].back();
     //line(saida1,inicio,fim,255,1,8,0);
     line(saida1,Point(inicio.y,inicio.x),Point(fim.y,fim.x),255,1,8,0);
     //cout<<"Vetor "<<i<<" inicio y "<<inicio.y<<" inicio x "<< inicio.x<<" fim y "<< fim.y<<" fim x "<< fim.x<<"\n";
     }
 }
 cout << "Padrao 1 "<< contP1<< " Padrão 2 "<< contP2 << " Padrao 3 "<< contP3 << " Padrao 4 "<< contP4<<"\n";
 cout << "Padrao 1 "<< vec1.size()<< " Padrão 2 "<< vec2.size() << " Padrao 3 "<< vec3.size() << " Padrao 4 "<< vec4.size()<<"\n";

 t4 = clock();
 float diff3 = ((float)t4-(float)t1);
 float seconds3 = diff3 / CLOCKS_PER_SEC;
 cout << seconds3<<"\n";

 //imwrite("saida0.png",saida1);
 //imwrite("saida.png",saida);

 //edge.convertTo(draw, CV_8UC1);

 String windowName = "Folha"; //Name of the window

 namedWindow(windowName,WINDOW_AUTOSIZE); // Create a window

 imshow(windowName, roi); // Show our image inside the created window.
 //imshow(windowName, saida);

 waitKey(0); // Wait for any keystroke in the window

 destroyWindow(windowName); //destroy the created window

 return 0;
}
 

  