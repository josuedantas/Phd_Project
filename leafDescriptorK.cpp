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
int fator = 0, jfinal = 0, ifinal = 0, tam = 3;
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
int fator = 0, jfinal = 0, ifinal = 0, tam = 3;
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
int fator = 0, jfinal = 0, ifinal = 0, tam = 3;
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
/* 
int countNodes( No *root ) {
           // Count the nodes in the binary tree to which
           // root points, and return the answer.
        if ( root == NULL )
           return 0;  // The tree is empty.  It contains no nodes.
        else {
           int count = 1;   // Start by counting the root.
           count += countNodes(root->left);  // Add the number of nodes
                                            //     in the left subtree.
           count += countNodes(root->right); // Add the number of nodes
                                            //    in the right subtree.
           return count;  // Return the total.
        }
} */


/* bool checkQuad(Mat img,Point ini, int lin, int col){
    bool result = false;
    double qtd_pontos = 0;
    qtd_pontos = (img.at<double>(ini.x+lin-1,ini.y+col-1) - img.at<double>(ini.x+lin-1,ini.y-1)-
    img.at<double>(ini.x-1,ini.y+col-1) + img.at<double>(ini.x-1,ini.y-1))/255.0;
    //cout << "Inicio "<<ini<<" linhas "<<lin<<" colunas "<<col<<" pontos "<<qtd_pontos<<"\n";
    if(qtd_pontos > 70)
    result = true;
    return result;
} */

double checkQuad(Mat img,Point ini, int lin, int col){
    bool result = false;
    double qtd_pontos = 0;
    qtd_pontos = (img.at<double>(ini.x+lin-1,ini.y+col-1) - img.at<double>(ini.x+lin-1,ini.y-1)-
    img.at<double>(ini.x-1,ini.y+col-1) + img.at<double>(ini.x-1,ini.y-1))/255.0;
    //cout << "Inicio "<<ini<<" linhas "<<lin<<" colunas "<<col<<" pontos "<<qtd_pontos<<"\n";
    /* if(qtd_pontos > 70)
    result = true; */
    return qtd_pontos;
}
void findDico(Mat temp1, Mat temp2, Mat temp3, Mat temp4,Point ini, int lin,int col){
    double total1 = 0, total2 = 0, total3 = 0, total4 = 0, coef = 0, maior = 0, soma = 0;
    int cont = 0;
    total1  = checkQuad(temp1,ini,lin,col);
    total2 = checkQuad(temp2,ini,lin, col);
    total3 = checkQuad(temp3,ini,lin, col);
    total4 = checkQuad(temp4,ini,lin, col);

    /* cout << "Inicio "<<ini<<" linhas "<<lin<<" colunas "<<col<<" template 1 "<<total1<<
        " template 2 "<<total2<<" template 3 "<<total3<<" template 4 "<<total4<<"\n"; */
   /* if(total1 > total2 && total1 > total3 && total1 > total4){
        maior = total1;
        soma = total2 + total3 + total4;
    }else if(total2 > total3 && total2 > total4){
        maior = total2;
        soma = total1 + total3 + total4;
    } else if(total3 > total4){
        maior = total3;
        soma = total1 + total2 + total4;
    }else{
        maior = total4;
        soma = total1 + total2 + total3;
    }  */

    if(total1 >= 1)
    cont++;
    if(total2 >= 1)
    cont++;
    if(total3 >= 1)
    cont++;
    if(total4 >= 1)
    cont++;
    //if(lin > 40 && col > 40){
   // if((maior >= 5) && (maior*0.5 <= soma)){
    if(cont >2){
        //cout << "Inicio "<<ini<<" linhas "<<lin<<" colunas "<<col<<"\n";
        int lfac = lin /2, cfac = col /2;
        regioes.push_back({ini,lin,col});
        findDico(temp1,temp2,temp3,temp4,ini,lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x,ini.y+cfac),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y+cfac),lfac,cfac);
    }/* else if (soma > 3){
        int lfac = lin /2, cfac = col /2;
        findDico(temp1,temp2,temp3,temp4,ini,lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x,ini.y+cfac),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y+cfac),lfac,cfac);
    } */
    //}

   /*  cout << "Inicio "<<ini<<" linhas "<<lin<<" colunas "<<col<<" coeficiente "<<coef<<"\n";
    if((maior > 80) && (maior > soma)){
        //tamanho diminui
        //só a última ocorrência deve ser gravada
        //insertNo(arvore,pos);
        int lfac = lin /2, cfac = col /2;
        regioes.push_back({ini,lin,col});
        findDico(temp1,temp2,temp3,temp4,ini,lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x,ini.y+cfac),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y+cfac),lfac,cfac);
    }else if((lin > 20 && col > 20)){
         int lfac = lin /2, cfac = col /2;
        findDico(temp1,temp2,temp3,temp4,ini,lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x,ini.y+cfac),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y),lfac,cfac);
        findDico(temp1,temp2,temp3,temp4,cv::Point(ini.x+lfac,ini.y+cfac),lfac,cfac);
    } */
    /* if(checkQuad == false || tam < 8){
        //colocar o anterior que era true
    }else if(checkQuad(){
        //tamanho diminui
        //só a última ocorrência deve ser gravada
        findDico();
        findDico();
        findDico();
        findDico();
    } */
}

Mat getVenation(string path){
int total1 = 0, total2 = 0, total3 = 0, total4 = 0;
double media = 0,var = 0, dp = 0,coef = 0;
int res = 0;
cv::Mat imgIntegral, imgInt2,imgInt45, imgIntT1, imgIntT2, imgIntT3, imgIntT4;
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

integral(roi,imgIntegral,imgInt2,imgInt45,CV_64F);
Mat temp1 = Mat::zeros(roi.rows,roi.cols,CV_8UC1);
Mat temp2 = Mat::zeros(roi.rows,roi.cols,CV_8UC1);
Mat temp3 = Mat::zeros(roi.rows,roi.cols,CV_8UC1);
Mat temp4 = Mat::zeros(roi.rows,roi.cols,CV_8UC1);

calcTemplate1(imgIntegral,temp1);
calcTemplate2(imgIntegral,temp2);
calcTemplate3(imgInt45,temp3);
calcTemplate4(imgInt45,temp4);

integral(temp1,imgIntT1,CV_64F);
integral(temp2,imgIntT2,CV_64F);
integral(temp3,imgIntT3,CV_64F);
integral(temp4,imgIntT4,CV_64F);

findDico(imgIntT1,imgIntT2,imgIntT3,imgIntT4,Point(1,1),roi.rows,roi.cols);

int qtd_temp1 = 0, qtd_temp2 = 0, qtd_temp3 = 0, qtd_temp4 = 0, somaT = 0;

/* for(int i = 0; i < roi.rows - 64; i+=64){
    for(int j = 0; j < roi.cols - 64; j+=64){
        qtd_temp1 = 0;
        qtd_temp2 = 0;
        qtd_temp3 = 0;
        qtd_temp4 = 0;
        somaT = 0;
        for(int k = i; k < (i+64);k++)
            for(int l = j; l < (j+64);l++){
                if(temp1.at<uchar>(k,l)==255)
                    qtd_temp1++;
                if(temp2.at<uchar>(k,l)==255)
                    qtd_temp2++;
                if(temp3.at<uchar>(k,l)==255)
                    qtd_temp3++;
                if(temp4.at<uchar>(k,l)==255)
                    qtd_temp4++;
            }
        coef = 0;
        //cout << "Temp 1 "<< qtd_temp1<<" Temp 2 "<<qtd_temp2<<" Temp 3 "<<qtd_temp3<<" Temp 4 "<<qtd_temp4<<"\n";
        if(qtd_temp1 > qtd_temp2 && qtd_temp1 > qtd_temp3 && qtd_temp1 > qtd_temp4){
            somaT = qtd_temp2 + qtd_temp3 + qtd_temp4;
            if(somaT > 0)
            coef = qtd_temp1 / somaT;
        }else if(qtd_temp2 > qtd_temp3 && qtd_temp2 > qtd_temp4){
            somaT = qtd_temp1 + qtd_temp3 + qtd_temp4;
            if(somaT > 0)
            coef = qtd_temp2 / somaT;
        }else if(qtd_temp3 > qtd_temp4){
            somaT = qtd_temp1 + qtd_temp2 + qtd_temp4;
            if(somaT > 0)
            coef = qtd_temp3 / somaT;
        }else{
            somaT = qtd_temp1 + qtd_temp2 + qtd_temp3;
            if(somaT > 0)
            coef = qtd_temp4 / somaT;
        }
        //cout << coef << "\n";
        if(coef > 0 && coef < 0.9){
            rectangle(roi, Point(i-64,j-64), Point(i,j), 255, 1, 8, 0);
        }
    }
} */

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
std::string path = "./LeavesSelection/Editadas/05.jpg";
std::string line, first;
int tipo, altura = 0, largura = 0, menor1 = 0,menor2 = 0,menor3 = 0;;
std::ifstream myfile(path);
double media = 0,result = 0, media_distancias = 0, desvio_padrao = 0;
Mat output;
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

/* menor1 = regioes[0].largura*regioes[0].altura;

for(int i = 1; i < regioes.size();i++){
    int aux = 0;
    altura = regioes[i].largura;
    largura = regioes[i].altura;
    aux = altura*largura;
    if(menor1 > aux){
        menor3 = menor2;
        menor2 = menor1;
        menor1 = aux;
    }
    
}
 */
for(int i = 0; i < regioes.size();i++){
  /*   altura = regioes[i].largura;
    largura = regioes[i].altura;
    if((altura*largura)>=menor1 && menor3>=(altura*largura)){ */
        points_entrada.push_back(regioes.at(i).inicio);
   // }
    
}
Mat labels;
for (int i =1; i <= 10; i++){
compactness = kmeans(points_entrada, 6, labels,
            TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers);
            cout << "Centroide "<<compactness<<"\n";
}

//cout <<"Centroide "<<centers[0].x<<" "<<centers[0].y<<"\n";
/* 
cout <<"Tamanho "<<points_entrada.size()<<"\n";

for(int i = 0; i < points_entrada.size();i++){
    double dist = sqrt(pow(points_entrada.at(i).x-centers.at(0).x,2)+
                          pow(points_entrada.at(i).y-centers.at(0).y,2));

    dist2center.push_back(dist);
    //cout << dist2center.at(i);
    media_distancias = media_distancias + dist2center.at(i);
}
    media_distancias = media_distancias / points_entrada.size();

for(int i = 0; i < points_entrada.size();i++){
    desvio_padrao = desvio_padrao + pow(dist2center.at(i)-media_distancias,2);
}

desvio_padrao = sqrt(desvio_padrao/points_entrada.size());

cout<<"Desvio padrão "<<desvio_padrao<< " Media "<<media_distancias<<"\n";

for(int i = 0; i < points_entrada.size();i++){
    if(dist2center.at(i)>(media_distancias+desvio_padrao)||dist2center.at(i)<(media_distancias-desvio_padrao)){
    points_entrada.erase(points_entrada.begin()+i);
    }
}

compactness = kmeans(points_entrada, 1, labels,
            TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers); */

 for(int i = 0; i < centers.size();i++){
    //cout <<"Centroide "<<centers[i].x<<" "<<centers[i].y<<"\n";
    circle(output,Point(centers[i].y,centers[i].x),20,255,1,8,0);
} 

/* for(int i = 0; i < points_entrada.size();i++){
    //cout <<"Centroide "<<centers[i].x<<" "<<centers[i].y<<"\n";
    circle(output,points_entrada[i],10,255,1,8,0);
} */

//circle(output,Point(50,100),50,255,1,8,0);

/* for(int i = 0; i < regioes.size();i++){
    Point p;
    int l = 0, c = 0;
    p = regioes[i].inicio;
    l = regioes[i].largura;
    c = regioes[i].altura;
    for(int j = 0; j < regioes.size();j++){
        if(i == j)
        continue;
        Point p2;
        p2 = regioes[j].inicio;
       /*  if(p2.x >= p.x && p2.x<= p.x+l && p2.y >= p.y && p2.y<= p.y+c && regioes[i].largura > regioes[j].largura
        && regioes[i].altura > regioes[j].largura){
            regioes.erase(regioes.begin()+i);
        } */

        //ao identificar se há regiões com correspondência em áreas menores excluir a área maior
        /* if(p2.x >= p.x && p2.x< p.x+l && p2.y >= p.y && p2.y< p.y+c){
            regioes.erase(regioes.begin()+i);
        }
    }
} */
cout << "Quantidade "<<regioes.size()<<"\n";

for(int i = 0; i < points_entrada.size();i++){
    circle(output,Point(points_entrada[i].y,points_entrada[i].x),5,255,1,8,0);
   /*  Point p1,p2;
    p1 = regioes[i].inicio;
    p2 = Point(p1.x + regioes[i].largura,p1.y + regioes[i].altura); */
    /* Rect r;
    r.y = points_entrada[i].inicio.x;
    r.x = points_entrada[i].inicio.y;
    r.height = points_entrada[i].largura;
    r.width = points_entrada[i].altura;
    rectangle(output,r,255,1,8,0);
    cout << "Inicio "<<points_entrada[i].inicio<<" linhas "<<points_entrada[i].largura<<" colunas "<<points_entrada[i].altura<<"\n"; */
}

/* for(int i = 0; i < regioes.size();i++){
    Point p1,p2;
    p1 = regioes[i].inicio;
    p2 = Point(p1.x + regioes[i].largura,p1.y + regioes[i].altura);
    Rect r;
    r.y = regioes[i].inicio.x;
    r.x = regioes[i].inicio.y;
    r.height = regioes[i].largura;
    r.width = regioes[i].altura;
    rectangle(output,r,255,1,8,0);
    cout << "Inicio "<<regioes[i].inicio<<" linhas "<<regioes[i].largura<<" colunas "<<regioes[i].altura<<"\n";
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