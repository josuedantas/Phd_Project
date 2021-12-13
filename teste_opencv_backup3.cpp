//#include <cv.h>
//#include <highgui.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc/seeds.hpp>
#include <opencv2/ximgproc/slic.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/ml.hpp>

//#include <opencv2/tracking.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <bitset>
#include <queue>
#include <cmath>


#define PI 3.14159265

using namespace cv;
using namespace std;
using namespace cv::ximgproc;
using namespace cv::ml;

RNG rng(12345);

//vector < vector<Point> > retangulos;
struct TotalTemplates {
    int qtdTemp1;
    int qtdTemp2;
    int qtdTemp3;
    int qtdTemp4;
};

struct Retangulo {
    Point inicial;
    Point final;
    int qtd2Templates;
};

struct AVisitar {
    Point p;
    int id;
};

struct SuperPixel_features {
    int color = 0;
    int Y = 0;
    int Cb = 0;
    int Cr = 0;
    Point p_central = Point (0,0);
    Point inicio;
    Point fim;
    bool first = false;
};

struct ShadowsAreas {
    int index;
    Point p;
    bool shadowed;
};

struct ColorSpaceValues {
    float L = 0;
    float a = 0;
    float b = 0;
    int cont = 0;

};

struct GreenCircles {
    Point central;
    int raio = 0;
};

double getTemplate1(cv::Mat& img, cv::Point p, int tam) { //horizontal
    double mediaR1 = 0, mediaR2 = 0;
    mediaR1 = (img.at<double>(p.y + (tam - 1), p.x + 1) - img.at<double>(p.y + (tam - 1), p.x) - img.at<double>(p.y - 1, p.x + 1) +
        img.at<double>(p.y - 1, p.x)) / tam;
    //cout <<" Media 1 template 1 "<<mediaR1<<"\n";

    mediaR2 = (img.at<double>(p.y + (tam - 1), p.x) - img.at<double>(p.y + (tam - 1), p.x - 1) - img.at<double>(p.y - 1, p.x) +
        img.at<double>(p.y - 1, p.x - 1)) / tam;
    //cout <<" Media 2 template 2 "<<mediaR2<<"\n";
    return mediaR1 - mediaR2;

}

double getTemplate2(cv::Mat& img, cv::Point p, int tam) {//vertical
    double mediaR1 = 0, mediaR2 = 0;
    mediaR1 = (img.at<double>(p.y, p.x + (tam - 1)) - img.at<double>(p.y - 1, p.x + (tam - 1)) - img.at<double>(p.y, p.x - 1) +
        img.at<double>(p.y - 1, p.x - 1)) / tam;

    mediaR2 = (img.at<double>(p.y + 1, p.x + (tam - 1)) - img.at<double>(p.y, p.x + (tam - 1)) - img.at<double>(p.y + 1, p.x - 1) +
        img.at<double>(p.y, p.x - 1)) / tam;
    return mediaR1 - mediaR2;
}

double getTemplate3(cv::Mat& img, cv::Point p, int tam) {
    double mediaR1 = 0, mediaR2 = 0;

    mediaR1 = (img.at<double>(p.y + tam - 1, p.x + tam - 1) - img.at<double>(p.y + (tam - 2), p.x + tam) -
        img.at<double>(p.y - 1, p.x - 1) + img.at<double>(p.y - 2, p.x)) / (tam); // Branco 

    mediaR2 = (img.at<double>(p.y + tam, p.x + tam - 2) - img.at<double>(p.y + tam - 1, p.x + tam - 1) - img.at<double>(p.y, p.x - 2) +
        img.at<double>(p.y - 1, p.x - 1)) / (tam); // Preto 

    return mediaR1 - mediaR2;
}

double getTemplate4(cv::Mat& img, cv::Point p, int tam) {
    double mediaR1 = 0, mediaR2 = 0;

    mediaR1 = (img.at<double>(p.y + tam, p.x) - img.at<double>(p.y + tam - 1, p.x - 1) -
        img.at<double>(p.y - 1, p.x + tam + 1) + img.at<double>(p.y - 2, p.x + tam)) / (tam); // Branco

    mediaR2 = (img.at<double>(p.y + tam + 1, p.x) - img.at<double>(p.y + tam, p.x - 1) - img.at<double>(p.y, p.x + tam + 1) +
        img.at<double>(p.y - 1, p.x + tam)) / (tam); // Preto

    return mediaR1 - mediaR2;
}

/*bitset<32> calcJanLBPH(Mat& imgIntegral, Mat& imgInt45, int row, int col, int tamJan, int tamTemplate) {
    std::bitset<32> lbph(std::string("0000000000000000000000000000000000000000"));
    std::bitset<32> lbphAux(std::string("0000000000000000000000000000000000000000"));
    std::bitset<32> lbph1(std::string("0000000000000000000000000000000000000000"));
    std::bitset<32> lbph2(std::string("0000000000000000000000000000000000000000"));
    std::bitset<32> lbph3(std::string("0000000000000000000000000000000000000000"));
    std::bitset<32> lbph4(std::string("0000000000000000000000000000000000000000"));
    for (int l = 0; l < tamJan; l++) {
        if (getTemplate1(imgIntegral, Point(col, row + l), tamTemplate) > 240) {
            lbph1 <<= 1;
            lbph1.set(0, 1);
            continue;
        }
        else {
            lbph1 <<= 1;
            lbph1.set(0, 0);
        }
        if (getTemplate2(imgIntegral, Point(col, row + l), tamTemplate) > 240) {
            lbph2 <<= 1;
            lbph2.set(0, 1);
            continue;
        }
        else {
            lbph2 <<= 1;
            lbph2.set(0, 0);
        }
        if (getTemplate3(imgInt45, Point(col, row + l), tamTemplate) > 240) {
            lbph3 <<= 1;
            lbph3.set(0, 1);
            continue;
        }
        else {
            lbph3 <<= 1;
            lbph3.set(0, 0);
        }
        if (getTemplate4(imgInt45, Point(col, row + l), tamTemplate) > 240) {
            lbph4 <<= 1;
            lbph4.set(0, 1);
            continue;
        }
        else {
            lbph4 <<= 1;
            lbph4.set(0, 0);
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
}*/

TotalTemplates calcJanLBPH(Mat& imgIntegral, Mat& imgInt45, int row, int col, int tamJan, int tamTemplate) {
    std::bitset<32> lbph(std::string("0000000000000000000000000000000000000000"));
    std::bitset<32> lbphAux(std::string("0000000000000000000000000000000000000000"));
    std::bitset<16> lbph1(std::string("0000000000000000"));
    std::bitset<16> lbph2(std::string("0000000000000000"));
    std::bitset<16> lbph3(std::string("0000000000000000"));
    std::bitset<16> lbph4(std::string("0000000000000000"));
    /*std::bitset<1> lbph1(std::string("0"));
    std::bitset<1> lbph2(std::string("0"));
    std::bitset<1> lbph3(std::string("0"));
    std::bitset<1> lbph4(std::string("0"));*/
    TotalTemplates totalT;
    //cout << "Passou " << row << " " << col << "\n";
    /*if (getTemplate1(imgIntegral, Point(col, row), tamTemplate) > 150) {
            lbph1.set(0, 1);
    }
    if (getTemplate2(imgIntegral, Point(col, row), tamTemplate) > 150) {
        lbph1.set(0, 1);
    }
    if (getTemplate3(imgIntegral, Point(col, row), tamTemplate) > 150) {
        lbph1.set(0, 1);
    }
    if (getTemplate4(imgIntegral, Point(col, row), tamTemplate) > 150) {
        lbph1.set(0, 1);
    }*/

    /*for (int l = 0; l < tamJan; l += 2) {
        if (getTemplate1(imgIntegral, Point(col, row - tamTemplate+ l), tamTemplate) > 150) {
            lbph1 <<= 1;
            lbph1.set(0, 1);
        }
        else {
            lbph1 <<= 1;
            lbph1.set(0, 0);
        }
    }
    for (int l = 0; l < tamJan; l += 2) {
        if (getTemplate2(imgIntegral, Point(col - tamTemplate + l, row - tamTemplate), tamTemplate) > 150) {
            lbph2 <<= 1;
            lbph2.set(0, 1);
            continue;
        }
        else {
            lbph2 <<= 1;
            lbph2.set(0, 0);
        }
        if (getTemplate3(imgInt45, Point(col - tamTemplate + l, row - tamTemplate), tamTemplate) > 150) {
            lbph3 <<= 1;
            lbph3.set(0, 1);
            continue;
        }
        else {
            lbph3 <<= 1;
            lbph3.set(0, 0);
        }
        if (getTemplate4(imgInt45, Point(col - tamTemplate + l, row - tamTemplate), tamTemplate) > 150) {
            lbph4 <<= 1;
            lbph4.set(0, 1);
            continue;
        }
        else {
            lbph4 <<= 1;
            lbph4.set(0, 0);
        }
    }
    for (int l = 0; l < tamJan; l += 2) {
        if (getTemplate1(imgIntegral, Point(col - tamTemplate, row - tamTemplate + l), tamTemplate) > 150) {
            lbph1 <<= 1;
            lbph1.set(0, 1);
        }
        else {
            lbph1 <<= 1;
            lbph1.set(0, 0);
        }
    }
    for (int l = 0; l < tamJan; l += 2) {
        if (getTemplate2(imgIntegral, Point(col - tamTemplate + l, row), tamTemplate) > 150) {
            lbph2 <<= 1;
            lbph2.set(0, 1);
            continue;
        }
        else {
            lbph2 <<= 1;
            lbph2.set(0, 0);
        }
        if (getTemplate3(imgInt45, Point(col - tamTemplate + l, row), tamTemplate) > 150) {
            lbph3 <<= 1;
            lbph3.set(0, 1);
            continue;
        }
        else {
            lbph3 <<= 1;
            lbph3.set(0, 0);
        }
        if (getTemplate4(imgInt45, Point(col - tamTemplate + l, row), tamTemplate) > 150) {
            lbph4 <<= 1;
            lbph4.set(0, 1);
            continue;
        }
        else {
            lbph4 <<= 1;
            lbph4.set(0, 0);
        }
    }*/
    row += 2;
    col += 2;
    for (int l = 0; l < tamJan; l += 2) {
        if (getTemplate1(imgIntegral, Point(col, row + l), tamTemplate) > 150) {
            lbph1 <<= 1;
            lbph1.set(0, 1);
        }
        else {
            lbph1 <<= 1;
            lbph1.set(0, 0);
        }
    }
    for (int l = 0; l < tamJan; l += 2) {
        if (getTemplate2(imgIntegral, Point(col + l, row ), tamTemplate) > 150) {
            lbph2 <<= 1;
            lbph2.set(0, 1);
            continue;
        }
        else {
            lbph2 <<= 1;
            lbph2.set(0, 0);
        }
        if (getTemplate3(imgInt45, Point(col + l, row ), tamTemplate) > 150) {
            lbph3 <<= 1;
            lbph3.set(0, 1);
            continue;
        }
        else {
            lbph3 <<= 1;
            lbph3.set(0, 0);
        }
        if (getTemplate4(imgInt45, Point(col + l, row ), tamTemplate) > 150) {
            lbph4 <<= 1;
            lbph4.set(0, 1);
            continue;
        }
        else {
            lbph4 <<= 1;
            lbph4.set(0, 0);
        }
    }
    for (int l = 0; l < tamJan; l += 2) {
        if (getTemplate1(imgIntegral, Point(col, row + l), tamTemplate) > 150) {
            lbph1 <<= 1;
            lbph1.set(0, 1);
        }
        else {
            lbph1 <<= 1;
            lbph1.set(0, 0);
        }
    }
    for (int l = 0; l < tamJan; l += 2) {
        if (getTemplate2(imgIntegral, Point(col + l, row), tamTemplate) > 150) {
            lbph2 <<= 1;
            lbph2.set(0, 1);
            continue;
        }
        else {
            lbph2 <<= 1;
            lbph2.set(0, 0);
        }
        if (getTemplate3(imgInt45, Point(col + l, row), tamTemplate) > 150) {
            lbph3 <<= 1;
            lbph3.set(0, 1);
            continue;
        }
        else {
            lbph3 <<= 1;
            lbph3.set(0, 0);
        }
        if (getTemplate4(imgInt45, Point(col + l, row), tamTemplate) > 150) {
            lbph4 <<= 1;
            lbph4.set(0, 1);
            continue;
        }
        else {
            lbph4 <<= 1;
            lbph4.set(0, 0);
        }
    }
    totalT.qtdTemp1 = lbph1.count();
    totalT.qtdTemp2 = lbph2.count();
    totalT.qtdTemp3 = lbph3.count();
    totalT.qtdTemp4 = lbph4.count();
    return totalT;
}

vector<Retangulo> splitImage(Mat input) {
    double contGreen = 0, retSize = 0, l =0, m = 0, rateGreen = 0;
    int qtdEmpty = 0;
    Mat rgbchannels[3];
    split(input, rgbchannels);
    vector<Retangulo> saidaRet;
    Retangulo ret;
    retSize = input.cols / 6;
    for (int i = 0; i < input.rows; i+=retSize) {
        for (int j = 0; j < input.cols; j+=retSize) {
            contGreen = 0;
            for (l = i; l < i + retSize && l < input.rows; l++) {
                for (m = j; m < j + retSize && m < input.cols; m++) {
                    if (((int)rgbchannels[1].at<uchar>(l, m) >= (int)rgbchannels[2].at<uchar>(l, m) * 0.90)
                        && ((int)rgbchannels[1].at<uchar>(l, m) > (int) rgbchannels[0].at<uchar>(l, m))) {
                        contGreen++;
                    }
                }
            }
            rateGreen = contGreen * 100 / (retSize * retSize);
            if (rateGreen > 15) {
                ret.inicial = Point(j, i);
                ret.final = Point(m-1, l-1);
                ret.qtd2Templates = rateGreen;
                saidaRet.push_back(ret);
            }

        }
    }
    //Retangulo r;
    //cout << input.rows << " " << input.cols <<" "<<saidaRet.size()<< "\n";
    /*for (int h = 0; h < saidaRet.size(); h++) {
     
        r = saidaRet.at(h);

        /*if (r.qtd2Templates < 3) {
            qtdEmpty++;
        }*/
        //cout << r.inicial << " " << r.final << "\n";
        //rectangle(input, r.inicial, r.final, Scalar(0, 255, 0), 1, 8);
 
        //putText(input, to_string(contGreen*100/1600), r.inicial, 2, 1, Scalar(0, 0, 255),1,8, false);
       // putText(input, " "+ to_string(r.qtd2Templates), Point(r.inicial.x + 20, r.inicial.y+20), 1, 0.75, Scalar(0, 0, 255), 1, 2, false);

        //cout << "Ponto inicial " << r.inicial << "fim " << r.final << "\n";
    //}
    //cout << "Percentual de área a não ser analisadas " << qtdEmpty * 100 / saidaRet.size() << "\n";
    //imshow("SPlit",input);
    return saidaRet;
}

//Guardar o raio de um determinado ponto que foi achado a textura com pixels verde
//O valor do componente verde é parecedi com o do vermelho, 10%, o azul sempre menor

vector<GreenCircles> calcLBP(Mat entrada) {
    std::bitset<32> lbph(std::string("0000000000000000000000000000000000000000"));
    Mat rgbchannel[3];
    vector<GreenCircles>areasverdes;
    split(entrada, rgbchannel);
    int cont = 0, contV = 0, distance = 0;
    bool intersection;
    for (int i = 0; i < entrada.rows; i+=5) {
        for (int j = 0; j < entrada.cols; j+=5) {
            cont = 0;
            if (i < 0 || i >= entrada.rows) {
                break;
            }
            //cout << "PAssou 0 " << i << " " << j << "\n";
            //cout << (int) rgbchannel[0].at<uchar>(i, j)<<"\n";
            if (((int)rgbchannel[1].at<uchar>(i, j) >= (int)rgbchannel[2].at<uchar>(i, j)* 0.90)
                        && ((int) rgbchannel[1].at<uchar>(i, j) > (int) rgbchannel[0].at<uchar>(i, j))) {
                //cout << "Passou por aqui \n";
                GreenCircles gc;
                cont++;
                contV = 0;
                for (int h = i - cont; h <= i + cont; h++) {
                    for (int m = j - cont; m <= j + cont; m++) {
                        if (h < 0 || h >= entrada.rows || m < 0 || m >= entrada.cols) {
                            break;
                        }
                        if (h == i - cont || h == i + cont || m == j - cont || m == j + cont) {
                            if ((int)rgbchannel[1].at<uchar>(h, m) >= (int)rgbchannel[2].at<uchar>(h, m) * 0.90
                                && (int)rgbchannel[1].at<uchar>(h, m) > (int)rgbchannel[0].at<uchar>(h, m)) {
                                contV++;
                                //cout << "PAssou 1 " << h << " " << m << "\n";
                            }
                        }
                        else {
                            //cout << "PAssou 2 " << h << " " << m << "\n";
                            continue;
                        }
                        if (contV >= (cont * 8)*0.75) {
                            cont++;
                            h = i - cont;
                            m = j - cont;
                            contV = 0;
                        }
                        /*if (h >= i - cont + 1 && m >= j - cont + 1) {
                            cout << "PAssou 1 " << h << " " << m << "\n";
                            continue;
                        }
                        else {
                            if ((int)rgbchannel[1].at<uchar>(h, m) >= (int)rgbchannel[2].at<uchar>(h, m) * 1.10
                                && (int)rgbchannel[1].at<uchar>(h, m) > (int)rgbchannel[0].at<uchar>(h, m)) {
                                cout << "PAssou 2 "<<h<<" "<<m<< "\n"; 
                                contV++;
                            }
                        }
                        if (contV >= (cont * 8) - 2) {
                            cont++;
                            contV = 0;
                        }*/
                    }
                }
                if (cont >= 5) {
                    gc.central = Point(j, i);
                    gc.raio = cont;
                    areasverdes.push_back(gc);
                    j = j + 2 * cont;
                    /*intersection = false;
                    gc.central = Point(j,i);
                    gc.raio = cont;
                    GreenCircles auxCG;
                    for (int it = areasverdes.size()-1; it >=0; it--) {
                        intersection = false;
                        auxCG = areasverdes.at(it);
                        //cout << gc.central << " raio "<<gc.raio<< auxCG.central <<" raio "<<auxCG.raio<<"\n";
                        distance = sqrt(pow((gc.central.x - auxCG.central.x),2)+ pow((gc.central.y - auxCG.central.y), 2));
                        //cout << "Passou dist " << distance <<" contador "<<it<< "\n";
                        if (distance < gc.raio + auxCG.raio) {
                            intersection = true;
                            //cout << "Passou vetor verde \n";
                            if (gc.raio > auxCG.raio && distance > gc.raio) {
                                areasverdes.at(it).raio = gc.raio + (distance - gc.raio);
                                areasverdes.at(it).central = Point(((gc.central.x + auxCG.central.x) / 2), ((gc.central.y + auxCG.central.y) / 2));
                            }
                            else if (auxCG.raio > gc.raio && distance > auxCG.raio) {
                                areasverdes.at(it).raio = auxCG.raio + (distance - auxCG.raio);
                                areasverdes.at(it).central = Point(((gc.central.x + auxCG.central.x) / 2), ((gc.central.y + auxCG.central.y) / 2));
                            }else if (gc.raio > auxCG.raio) {
                                areasverdes.at(it).raio = gc.raio;
                                areasverdes.at(it).central = gc.central;
                            }
                            else {
                                areasverdes.at(it).raio = auxCG.raio;
                            }
                            //gc = areasverdes.at(it);
                            //areasverdes.erase(areasverdes.begin()+it);
                            //it = areasverdes.size() - 1;
                            break;
                        }

                    }
                    if (!intersection) {
                        areasverdes.push_back(gc);
                    }*/
                    //cout << "Passou circulo \n";
                    //circle(entrada, Point(j ,i), cont, Scalar(0, 0, 255),1, LINE_8);
                    //i = i + cont;
                    //j = j + cont;
                }
            }
        }
    }
    //imshow("LBP", entrada);
    return areasverdes;
}


vector<Retangulo> calcLBPH(Mat& img, vector<Retangulo>areasToAnalyse, int tamRect) {
    std::bitset<32> lbph(std::string("0000000000000000000000000000000000000000"));
    std::bitset<8> lbph1(std::string("00000000"));
    std::bitset<8> lbph2(std::string("00000000"));
    std::bitset<8> lbph3(std::string("00000000"));
    std::bitset<8> lbph4(std::string("00000000"));
    vector<Retangulo> resultado;
    Retangulo ret, retAux;
    vector<AVisitar>avisitar;
    cv::Mat imgIntegral, imgInt2, imgInt45;
    integral(img, imgIntegral, imgInt2, imgInt45, CV_64F);
    TotalTemplates totalTemps1, totalTemps2, totalTemps3, totalTemps4;
    int qtdTemp1 = 0, qtdTemp2 = 0, qtdTemp3 = 0, qtdTemp4 = 0, qtdTemps = 0, total = 0, passo = 0;
    Point paux;
    bool visitado;
    AVisitar av;
    GreenCircles gcAux;
    //cout << img.rows << " " << img.cols << "\n";
    for (int h = 0; h < areasToAnalyse.size(); h++) {
        retAux = areasToAnalyse.at(h);
        int sz = 8;
                visitado = false;
                qtdTemps = 0;
                //cout << "Passou " << m<<" "<<n << "\n";
                totalTemps1 = calcJanLBPH(imgIntegral, imgInt45, retAux.inicial.y, retAux.inicial.x, tamRect, sz); 
                //cout << totalTemps1.qtdTemp1<<" "<<totalTemps1.qtdTemp2<<" "<<totalTemps1.qtdTemp3<<" "<<totalTemps1.qtdTemp4 << "\n";
                if (totalTemps1.qtdTemp1 > 0) {
                    qtdTemps++;
                }
                if (totalTemps1.qtdTemp2 > 0) {
                    qtdTemps++;
                }
                if (totalTemps1.qtdTemp3 > 0) {
                    qtdTemps++;
                }
                if (totalTemps1.qtdTemp4 > 0) {
                    qtdTemps++;
                }
                if (qtdTemps >= 1) {
                    for (int k = 0; k < avisitar.size(); k++) {
                        paux = avisitar.at(k).p;
                        if (paux.x == retAux.inicial.x && paux.y == retAux.inicial.y) {
                            visitado = true;
                            //avisitar.erase(avisitar.begin() + k);
                            av = avisitar.at(k);
                            avisitar.erase(avisitar.begin() + k);
                        }
                        //cout << "Passou k " << k << "\n";
                    }
                    if (visitado == true) {
                        resultado.at(av.id).final = retAux.inicial;
                        if (qtdTemps >= 2) {
                            resultado.at(av.id).qtd2Templates++;
                        }
                        //ret.final = Point(j, i);
                        //resultado.emplace(resultado.begin()+av.id, ret);
                        av.p = Point(retAux.inicial.x, retAux.inicial.y + tamRect);
                        avisitar.push_back(av);
                        av.p = Point(retAux.inicial.x + tamRect, retAux.inicial.y);
                        avisitar.push_back(av);
                        av.p = Point(retAux.inicial.x + tamRect, retAux.inicial.y + tamRect);
                        avisitar.push_back(av);
                        //cout << "Passou visitado " << av.id << "\n";
                    }
                }
                if (qtdTemps >= 2) {
                    if (visitado == false) {
                        ret.inicial = retAux.inicial;
                        ret.final = Point(retAux.inicial.x + tamRect, retAux.inicial.y + tamRect);
                        ret.qtd2Templates = qtdTemps;
                        resultado.push_back(ret);
                        av.p = Point(retAux.inicial.x, retAux.inicial.y + tamRect);
                        av.id = resultado.size() - 1;
                        avisitar.push_back(av);
                        av.p = Point(retAux.inicial.x + tamRect, retAux.inicial.y);
                        avisitar.push_back(av);
                        av.p = Point(retAux.inicial.x + tamRect, retAux.inicial.y + tamRect);
                        avisitar.push_back(av);
                        //cout << "Passou novo " << resultado.size()-1 << "\n";
                    }
                }
            }
    /*for (int i = 40; i < img.rows - 40; i += 10 ) {
        for (int j = 40; j < img.cols - 40; j += 10) {
            qtdTemps = 0;
            passo = 0;
            visitado = false;
            totalTemps1 = calcJanLBPH(imgIntegral, imgInt45, i, j, 10, 10);
            if (totalTemps1.qtdTemp1 > 0) {
                qtdTemps++;
            }
            if (totalTemps1.qtdTemp2 > 0) {
                qtdTemps++;
            }
            if (totalTemps1.qtdTemp3 > 0) {
                qtdTemps++;
            }
            if (totalTemps1.qtdTemp4 > 0) {
                qtdTemps++;
            }
            if (qtdTemps >= 1) {
                for (int k = 0; k < avisitar.size(); k++) {
                    paux = avisitar.at(k).p;
                    if (paux.x == j && paux.y == i) {
                        visitado = true;
                        //avisitar.erase(avisitar.begin() + k);
                        av = avisitar.at(k);
                        avisitar.erase(avisitar.begin() + k);
                    }
                    //cout << "Passou k " << k << "\n";
                }
                if (visitado == true) {
                    resultado.at(av.id).final = Point(j, i);
                    if (qtdTemps >= 2) {
                        resultado.at(av.id).qtd2Templates++;
                    }
                    //ret.final = Point(j, i);
                    //resultado.emplace(resultado.begin()+av.id, ret);
                    av.p = Point(j + 10, i);
                    avisitar.push_back(av);
                    av.p = Point(j, i + 10);
                    avisitar.push_back(av);
                    av.p = Point(j + 10, i + 10);
                    avisitar.push_back(av);
                    //cout << "Passou visitado " << av.id << "\n";
                }
            }
            if (qtdTemps >= 2) {
                if(visitado==false) {
                    ret.inicial = Point(j, i);
                    ret.final = Point(j, i);
                    ret.qtd2Templates = 1;
                    resultado.push_back(ret);
                    av.p = Point(j+10,i);
                    av.id = resultado.size() - 1;
                    avisitar.push_back(av);
                    av.p = Point(j , i + 10);
                    avisitar.push_back(av);
                    av.p = Point(j + 10, i + 10);
                    avisitar.push_back(av);
                    //cout << "Passou novo " << resultado.size()-1 << "\n";
                }
            }*/

            //cout << "T1 " << totalTemps1.qtdTemp1 << " T2 " << totalTemps1.qtdTemp2 << " T3 " << totalTemps1.qtdTemp3 << " T4 " << totalTemps1.qtdTemp4 << " ";
            /*if (qtdTemps > 0) {
                passo++;
                qtdTemps = 0;
                for (int k = i ; k <= i + (passo * 8) && k < img.rows - 8; k += 8) {
                    for (int m = j - (passo* 8); m <= j + (passo * 8) && m < img.cols - 8 && m >= 8; m += 8) {
                        totalTemps1 = calcJanLBPH(imgIntegral, imgInt45, k, m, 8, 8);
                        
                        //cout << "Passou " << k << " " << m <<" passo "<<passo<< "\n";
                        if (totalTemps1.qtdTemp1 > 0) {
                            qtdTemps++;
                        }
                        if (totalTemps1.qtdTemp2 > 0) {
                            qtdTemps++;
                        }
                        if (totalTemps1.qtdTemp3 > 0) {
                            qtdTemps++;
                        }
                        if (totalTemps1.qtdTemp4 > 0) {
                            qtdTemps++;
                        }
                    }
                    if (qtdTemps > 0) {
                        passo++;
                    } 
                    }
                if (passo >= 3) {
                    pontos.push_back(Point(j, i));
                }
            }*/
            /*qtdTemps = 0;
            total = 0;
            totalTemps1 = calcJanLBPH(imgIntegral, imgInt45, i, j, 16, 16);
            if (totalTemps1.qtdTemp1 > 0) {
                qtdTemps++;
            }
            if (totalTemps1.qtdTemp2 > 0) {
                qtdTemps++;
            }
            if (totalTemps1.qtdTemp3 > 0) {
                qtdTemps++;
            }
            if (totalTemps1.qtdTemp4 > 0) {
                qtdTemps++;
            }
            total = (qtdTemps - 1) * (totalTemps1.qtdTemp1 + totalTemps1.qtdTemp2 + totalTemps1.qtdTemp3 + totalTemps1.qtdTemp4);
            if (qtdTemps >= 2) {
                pontos.push_back(Point(j, i));
                passo = 16;
            }
            else {
                passo = 4;
            }*/

           /* totalTemps1 = calcJanLBPH(imgIntegral, imgInt45, i, j, 8, 8);
            totalTemps2 = calcJanLBPH(imgIntegral, imgInt45, i+8, j, 8, 8);
            totalTemps3 = calcJanLBPH(imgIntegral, imgInt45, i, j+8, 8, 8);
            totalTemps4 = calcJanLBPH(imgIntegral, imgInt45, i+8, j+8, 8, 8);
            qtdTemp1 = totalTemps1.qtdTemp1 + totalTemps2.qtdTemp1 + totalTemps3.qtdTemp1 + totalTemps4.qtdTemp1;
            qtdTemp2 = totalTemps1.qtdTemp2 + totalTemps2.qtdTemp2 + totalTemps3.qtdTemp2 + totalTemps4.qtdTemp2;
            qtdTemp3 = totalTemps1.qtdTemp3 + totalTemps2.qtdTemp3 + totalTemps3.qtdTemp3 + totalTemps4.qtdTemp3;
            qtdTemp4 = totalTemps1.qtdTemp4 + totalTemps2.qtdTemp4 + totalTemps3.qtdTemp4 + totalTemps4.qtdTemp4;
            if ((qtdTemp1 > 0 && qtdTemp2 > 0) || (qtdTemp1 > 0 && qtdTemp3 > 0) || (qtdTemp1 > 0 && qtdTemp4 > 0) || (qtdTemp2 > 0 && qtdTemp3 > 0) ||
                (qtdTemp2 > 0 && qtdTemp4 > 0) || (qtdTemp3 > 0 && qtdTemp4 > 0)) {
                totalTemps1 = calcJanLBPH(imgIntegral, imgInt45, i+16, j+16, 8, 8);
                totalTemps2 = calcJanLBPH(imgIntegral, imgInt45, i + 24, j, 8, 8);
                totalTemps3 = calcJanLBPH(imgIntegral, imgInt45, i, j + 24, 8, 8);
                totalTemps4 = calcJanLBPH(imgIntegral, imgInt45, i + 24, j + 24, 8, 8);
                qtdTemp1 = totalTemps1.qtdTemp1 + totalTemps2.qtdTemp1 + totalTemps3.qtdTemp1 + totalTemps4.qtdTemp1;
                qtdTemp2 = totalTemps1.qtdTemp2 + totalTemps2.qtdTemp2 + totalTemps3.qtdTemp2 + totalTemps4.qtdTemp2;
                qtdTemp3 = totalTemps1.qtdTemp3 + totalTemps2.qtdTemp3 + totalTemps3.qtdTemp3 + totalTemps4.qtdTemp3;
                qtdTemp4 = totalTemps1.qtdTemp4 + totalTemps2.qtdTemp4 + totalTemps3.qtdTemp4 + totalTemps4.qtdTemp4;
                if ((qtdTemp1 > 0 && qtdTemp2 > 0) || (qtdTemp1 > 0 && qtdTemp3 > 0) || (qtdTemp1 > 0 && qtdTemp4 > 0) || (qtdTemp2 > 0 && qtdTemp3 > 0) ||
                    (qtdTemp2 > 0 && qtdTemp4 > 0) || (qtdTemp3 > 0 && qtdTemp4 > 0)) {
                    pontos.push_back(Point(j+24,i+24));
                    j = j + 24;
                    i = i + 24;

                }
            }*/
        //}

        //cout << "\n";
    //}
    return resultado;
}


/*vector<Point> calcLBPH(Mat& m) {
    std::bitset<32> lbph(std::string("0000000000000000000000000000000000000000"));
    vector<Point> pontos;
    cv::Mat imgIntegral, imgInt2, imgInt45;
    integral(m, imgIntegral, imgInt2, imgInt45, CV_64F);

    int contTemp1 = 0, contTemp2 = 0, contTemp3 = 0, contTemp4 = 0;
    for (int i = 4; i < m.rows - 30; i += 8) {
        for (int j = 4; j < m.cols - 30; j += 8) {
            contTemp1 = 0;
            contTemp2 = 0;
            contTemp3 = 0;
            contTemp4 = 0;
            lbph.reset();
            lbph = lbph |= calcJanLBPH(imgIntegral, imgInt45, i, j, 8, 8);
            lbph = lbph |= calcJanLBPH(imgIntegral, imgInt45, i + 8, j, 8, 8);
            lbph = lbph |= calcJanLBPH(imgIntegral, imgInt45, i, j + 8, 8, 8);
            lbph = lbph |= calcJanLBPH(imgIntegral, imgInt45, i + 8, j + 8, 8, 8);

            for (int k = 0; k < 32; k++) {
                if (k < 8 && lbph[k] == 1) {
                    contTemp1++;
                    k = 8;
                }
                else if (k < 16 && lbph[k] == 1) {
                    contTemp2++;
                    k = 16;
                }
                if (k < 24 && lbph[k] == 1) {
                    contTemp3++;
                    k = 24;
                }
                if (k < 32 && lbph[k] == 1) {
                    contTemp4++;
                    k = 32;
                }
            }
            //cout <<i<<" "<<j<<"\n";
            //if (lbph.count() >= 2) {

            if ((contTemp1 + contTemp2 + contTemp3 + contTemp4) > 0) {

                //cout <<lbph<<"\n"
                //cout << "Passou lbph\n";
                pontos.push_back(Point(j + 8, i + 8));


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
              cout<<"lpbh "<<lbph<<"\n";
        }
    }
    return pontos;
}*/

cv::Mat calculaJ(Mat& input) {
    vector<cv::Mat> channel(3);
    split(input, channel);
    cv::Mat b = channel[0];
    cv::Mat g = channel[1];
    cv::Mat r = channel[2];
    channel.clear();
    //cv::Mat exg = Mat::zeros(Size(b.rows,b.cols),CV_8UC1);
    //cv::Mat exr = Mat::zeros(Size(b.rows,b.cols),CV_8UC1);
    cv::Mat j = Mat::zeros(Size(input.cols, input.rows), CV_16SC1);
    for (int m = 0; m < b.rows; m++) {
        for (int n = 0; n < b.cols; n++) {
            j.at<uchar>(m, n) = ((2 * g.at<uchar>(m, n)) - b.at<uchar>(m, n) - r.at<uchar>(m, n)) - ((2 * r.at<uchar>(m, n)) - b.at<uchar>(m, n) - g.at<uchar>(m, n));
        }
    }
    return j;
}

bool compareQtdTemps(const Retangulo& a, const Retangulo& b) {
    return a.qtd2Templates < b.qtd2Templates;
}

vector<SuperPixel_features> superPixelPosition(Mat labels, Mat YCrCb, int total) {
    vector<SuperPixel_features> r(total);
    SuperPixel_features element;
    Point inicio, fim; //verificar se o elemento no meio da diagonal pertence ao superpixel, se não procurar outro na diagonal
    int cont = 0;
    int position = 0;
    int cc = 0, lc = 0;
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            position = labels.at<int>(i, j);
            if (!r.at(position).first) {
                r.at(position).first = true;
                r.at(position).inicio = Point(j, i);
                r.at(position).fim = Point(j, i);
            }
            else {
                r.at(position).fim = Point(j, i);
            }
            /* if (labels.at<int>(i, j) == cont) {
                 r.push_back(Point(j,i));
                 cont++;
             }*/
        }
    }

        for (int m = 0; m < r.size(); m++) {
            cc = r.at(m).inicio.x + ((r.at(m).fim.x - r.at(m).inicio.x)/2);
            lc = r.at(m).inicio.y + (r.at(m).fim.y - r.at(m).inicio.y) / 2;
            //cout << "C "<<cc<<" L "<<lc<<"\n";

            if (labels.at<int>(lc, cc) == m) {
                r.at(m).p_central = Point(cc,lc);
            }else {
               r.at(m).p_central = r.at(m).inicio;
            }

            r.at(m).Y = YCrCb.at<Vec3b>(r.at(m).p_central)[0];
            r.at(m).Cr = YCrCb.at<Vec3b>(r.at(m).p_central)[1];
            r.at(m).Cb = YCrCb.at<Vec3b>(r.at(m).p_central)[2];
        }


    return r;
}

cv::Mat getVenation(string path) {
    int total1 = 0, total2 = 0, total3 = 0, total4 = 0;
    double media = 0, var = 0, dp = 0, coef = 0;
    int res = 0;
    cv::Mat image = cv::imread(path);
    //cv::Mat mask;
    cv::Mat reduzida = Mat::zeros(Size(image.cols, image.rows), CV_8UC3);
    //, bgdModel, fgdModel;

    cv::resize(image, reduzida, Size(), 0.25 , 0.25, INTER_AREA);

    splitImage(reduzida);

    vector<GreenCircles>areasRoi;
    //imshow("Reduzida",reduzida);
    areasRoi = calcLBP(reduzida);
    GreenCircles element;
    //cout << areasRoi.at(0).raio<<"\n";

    //Circulos textura

    for (int l = 0; l < areasRoi.size(); l++) {
        //cout << element.central << " raio " << element.raio << "\n";
        element = areasRoi.at(l);
        circle(reduzida, element.central, element.raio, Scalar(0, 0, 255), 1, LINE_8);

    }

    imshow("Reduzida", reduzida);
    //reduzida = image.clone();

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


   // cv::Mat edge, edgeB, mediaImg, gausImg;
    //Mat3b hsv;
    Mat edgeB = Mat::zeros(Size(reduzida.cols, reduzida.rows), CV_8SC1);
    Mat gray2 = Mat::zeros(Size(reduzida.cols, reduzida.rows), CV_8SC1);
    //cv::GaussianBlur( reduzida, gausImg, Size( 5, 5 ), 0, 0 );
    //cv::medianBlur( reduzida, mediaImg, 13);
    cv::cvtColor(reduzida, gray2, COLOR_BGR2GRAY);
    //cv::cvtColor(outj, gray2, COLOR_BGR2GRAY);
    //cv::cvtColor(mediaImg, gray1, COLOR_BGR2GRAY);

    //Canny( gray1, edge, 100, 150, 3);
    Mat canny_output;
    Mat edge_output = Mat::zeros(reduzida.size(), CV_8UC1);

    for (int i = 1; i < reduzida.rows-1; i++) {
        for (int j = 1; j < reduzida.cols -1; j++) {
            if ((reduzida.at<Vec3b>(i-1,j)[0]>200 & reduzida.at<Vec3b>(i, j)[0] < 200 )|| 
                    (reduzida.at<Vec3b>(i, j -1)[0] > 200 & reduzida.at<Vec3b>(i, j)[0] < 200)||
                        (reduzida.at<Vec3b>(i+1, j)[0] > 200 & reduzida.at<Vec3b>(i, j)[0] < 200)||
                            (reduzida.at<Vec3b>(i, j + 1)[0] > 200 & reduzida.at<Vec3b>(i, j)[0] < 200)) {
                edge_output.at<uchar>(i, j) = 255;
            }
        }
    }


  /* double media_movel = 0;
    for (int i = 0; i < edge_output.rows-2; i++) {
        for (int j = 0; j < edge_output.cols-2; j++) {
            if (edge_output.at<unsigned short>(i, j) > 0 && edge_output.at<unsigned short>(i, j+1) == 0 && edge_output.at<unsigned short>(i, j+2) > 0) {
                edge_output.at<unsigned short>(i, j) = 0; edge_output.at<unsigned short>(i, j+2) = 0;
            }else if (edge_output.at<unsigned short>(i, j) > 0 && edge_output.at<unsigned short>(i +1, j) == 0 && edge_output.at<unsigned short>(i + 2, j) > 0) {
                edge_output.at<unsigned short>(i, j) = 0; edge_output.at<unsigned short>(i + 2, j) = 0;
            }else if (edge_output.at<unsigned short>(i, j) > 0 && edge_output.at<unsigned short>(i, j + 1) > 0 ) {
                edge_output.at<unsigned short>(i, j) = 0; edge_output.at<unsigned short>(i, j + 1) = 0;
            }
            else if (edge_output.at<unsigned short>(i, j) > 0 && edge_output.at<unsigned short>(i+1, j) > 0) {
                edge_output.at<unsigned short>(i, j) = 0; edge_output.at<unsigned short>(i + 1, j ) = 0;
            }
            media_movel = 0;
            media_movel = (edge_output.at<unsigned short>(i, j) + edge_output.at<unsigned short>(i, j+1) + edge_output.at<unsigned short>(i, j+2) +
                edge_output.at<unsigned short>(i+1, j) + edge_output.at<unsigned short>(i+1, j+1) + edge_output.at<unsigned short>(i+1, j+2) +
                edge_output.at<unsigned short>(i+2, j) + edge_output.at<unsigned short>(i+2, j+1) + edge_output.at<unsigned short>(i+2, j+2))/9;
            //cout << "Media " << media_movel << "\n";
            if (media_movel > 32000) {
                edge_output.at<unsigned short>(i, j) = 0; edge_output.at<unsigned short>(i, j + 1) = 0; edge_output.at<unsigned short>(i, j + 2) = 0;
                edge_output.at<unsigned short>(i+1, j) = 0; edge_output.at<unsigned short>(i+1, j + 1) = 0; edge_output.at<unsigned short>(i+1, j + 2) = 0;
                edge_output.at<unsigned short>(i+2, j) = 0; edge_output.at<unsigned short>(i+2, j + 1) = 0; edge_output.at<unsigned short>(i+2, j + 2) = 0;
            }
        }
    }*/

    /*Canny(gray2, canny_output, 150, 250);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(canny_output, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<Point2f>centers(hierarchy.size());
    vector<float>radius(contours.size());
    for (size_t i = 0; i < contours.size(); i++) 
    {
            if (hierarchy[i].val[3] > 0) {
                approxPolyDP(contours[i], contours_poly[i], 3, true);
                boundRect[i] = boundingRect(contours_poly[i]);
                minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
            }
        //approxPolyDP(hierarchy[i].val[0], contours_poly[i], 3, true);
        //boundRect[i] = boundingRect(contours_poly[i]);
        //minEnclosingCircle(hierarchy[i].val[0], centers[i], radius[i]);
    }
    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);

    for (size_t i = 0; i < contours_poly.size(); i++)
    {
        Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
        //drawContours(drawing, contours_poly, (int)i, color);
        rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2);
        //circle(drawing, centers[i], (int)radius[i], color, 2);
    }
    imshow("Contours", edge_output);*/

    cv::Canny(gray2, edgeB, 100, 250, 3);

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
    /*for (int i = 0; i < edgeB.rows; i++) {
        for (int j = 0; j < edgeB.cols; j++) {
            if (edge_output.at<uchar>(i, j) != 0 && edge_output.at<uchar>(i, j) != 0) {
                edgeB.at<uchar>(i, j) = 0;
            }
        }
    }*/
    int esq = edgeB.cols, dir = 0, cima = edgeB.rows, baixo = 0; // primeiro pixel diferente de 255.
    for (int l = 0; l < edgeB.rows; l++) {
        for (int c = 0; c < edgeB.cols; c++) {
            if (edgeB.at<schar>(l, c) >= 0) {
                //cout << "Passou \n";
                if (c < esq) esq = c;
                if (dir < c) dir = c;
                if (l < cima) cima = l;
                if (baixo < l) baixo = l;
            }
        }
    }
    cv::Mat roi(edgeB, Rect(esq, cima, dir - esq + 1, baixo - cima + 1));
    //imshow("Teste 10", dra    wing);

    vector<Retangulo> rects;
    //Mat outp1 = Mat::zeros(Size(roi.cols,roi.rows),CV_8UC1);
    rects = calcLBPH(roi, splitImage(reduzida),image.cols/6);
    //Mat saida = Mat::zeros(roi.size(), CV_8UC3);
    Mat saida;
    cv::cvtColor(roi, saida, COLOR_GRAY2BGR);
    std::sort(rects.begin(),rects.end(),compareQtdTemps);
    string txt1, txt2;
    int arearect;
   
    for (int i = 0; i < rects.size(); i++) {
        /*if (i == rects.size() - 2) {
            continue;
        }*/

        Retangulo r = rects.at(i);
        cout << r.inicial<<" "<<r.final << "\n";
        arearect = (r.final.x - r.inicial.x) * (r.final.y - r.inicial.y);
        txt1 = to_string(r.qtd2Templates);
        txt2 = to_string(arearect);
        //cout << "Relaçao cruzamentos " << txt1 << " area " << txt2 << "\n";
        //outp1.at<uchar>(p.x,p.y)=255;
        //circle(roi, Point(p.x, p.y), 16, 255, 1, 8, 0);
        if (r.qtd2Templates >= 2) {
           //rectangle(saida, r.inicial, r.final, Scalar(0, 255 - (i * 5), (i * 15)), 2, 8, 0);
        }
        //putText(saida, txt1+" "+txt2, r.inicial, 2, 1, Scalar(0, 0, 255),1,8, false);
        //cout << "Ponto inicial " << r.inicial << "fim " << r.final << "\n";
    }
    //imshow("LBPH",outp1);
    return saida;
}



int main(int argc, char** argv)
{
    clock_t t1, t4;
    t1 = clock();
    //std::string path = "C:/Projeto/LeavesSelection/Editadas/leaves_shadow/lf10.jpg";
    std::string path = "C:/Projeto/LeavesSelection/Editadas/edt01.JPG";
    //std::string path = "./LeavesSelection/Editadas/semfundo2.JPG";
    //std::string path = "./dataset_UFRA/DSCN0003.JPG";
    cv::Mat output;
    vector<Point2f> centers;

    output = getVenation(path);


    /*  for(int i = 0; i < centers.size();i++){
        circle(output,Point(centers[i].y,centers[i].x),20,255,1,8,0);
    } */


    t4 = clock();
    float diff3 = ((float)t4 - (float)t1);
    float seconds3 = diff3 / CLOCKS_PER_SEC;
    cout << seconds3 << "\n";
    cout << "Tempo processamento " << seconds3 << "\n";

    imwrite("out01.png", output);
    //imwrite("saida.png",saida);

    //edge.convertTo(draw, CV_8UC1);

    String windowName = "Folha"; //Name of the window

    namedWindow(windowName, WINDOW_AUTOSIZE); // Create a window

    imshow(windowName, output); // Show our image inside the created window.
    //imshow(windowName, saida);

    waitKey(0); // Wait for any keystroke in the window

    destroyWindow(windowName); //destroy the created window */

    return 0;
}