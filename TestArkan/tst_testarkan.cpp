#include <QtTest>
#include "arkan_processing.h"
#include "tst_testarkan.h"
#include <QObject>
#include <vector>
#include <iostream>
#include <math.h>
#include <cmath>

class testArkan : public QObject
{
    Q_OBJECT

public:
    testArkan();
    ~testArkan();

private slots:
    void test1_caseConvertToFFT();
    void test2_caseConvertToFFT();

    void test1_caseConvertFFTtoLog();
    void test2_caseConvertFFTtoLog();

    void test1_caseProcessCmplxFFT();

    void test1_caseProcessRawCmplxI32();
    void test2_caseProcessRawCmplxI32();

    void test1_caseProcessCmplxFlt();
private:
    ArkanProcessing* process = new ArkanProcessing();

};

testArkan::testArkan(){}

testArkan::~testArkan(){delete process;}

void testArkan::test1_caseConvertToFFT()
{
    size_t size = 10;
    std::complex<int32_t> *in = new std::complex<int32_t>[size];
    std::complex<float> *out = new std::complex<float>[size];

    for(int i = 0; i < size; i++)
    {
        in[i] = {i + 1, 0};
    }

    process->convertSrcToFft(in, out, size);

    std::complex<float> *result = new std::complex<float>[size];
    //Значения посчитанные с онлайн калькулятор БПФ
    result[0] = {55,0};
    result[1] = {-5,15.3884};
    result[2] = {-5,6.88191};
    result[3] = {-5,3.63271};
    result[4] = {-5,1.6246};
    result[5] = {-5,0};
    result[6] = {-5,-1.6246};
    result[7] = {-5,-3.63271};
    result[8] = {-5,-6.88191};
    result[9] = {-5,-15.3884};

    for(int i = 0; i < size; i++)
    {
        std::cout << out[i] << std::endl;
    }

     QCOMPARE(out[0].real(),result[0].real());
     QCOMPARE(out[0].imag(),result[0].imag());
     QCOMPARE(out[1].imag(),result[1].imag());
     QCOMPARE(out[1].real(),result[1].real());
     QCOMPARE(out[2].imag(),result[2].imag());
     QCOMPARE(out[2].real(),result[2].real());
     QCOMPARE(out[3].imag(),result[3].imag());
     QCOMPARE(out[3].real(),result[3].real());
     QCOMPARE(out[4].imag(),result[4].imag());
     QCOMPARE(out[4].real(),result[4].real());
     QCOMPARE(out[5].imag(),result[5].imag());
     QCOMPARE(out[5].real(),result[5].real());
     QCOMPARE(out[6].imag(),result[6].imag());
     QCOMPARE(out[6].real(),result[6].real());
     QCOMPARE(out[7].imag(),result[7].imag());
     QCOMPARE(out[7].real(),result[7].real());
     QCOMPARE(out[8].imag(),result[8].imag());
     QCOMPARE(out[8].real(),result[8].real());
     QCOMPARE(out[9].imag(),result[9].imag());
     QCOMPARE(out[9].real(),result[9].real());

     delete[] in;
     delete[] out;
     delete[] result;
}

void testArkan::test2_caseConvertToFFT()
{
    size_t size = 5;
    std::complex<int32_t> *in = new std::complex<int32_t>[size];
    std::complex<float> *out = new std::complex<float>[size];

    for(int i = 0; i < size; i++)
    {
        in[i] = {i + 1, 0};
    }

    process->convertSrcToFft(in, out, size);

    std::complex<float> *result = new std::complex<float>[size];
    //Значения посчитанные с онлайн калькулятор БПФ
    result[0] = {15,0};
    result[1] = {-2.5,3.44095};
    result[2] = {-2.5,0.812299};
    result[3] = {-2.5,-0.812299};
    result[4] = {-2.5,-3.44095};

    for(int i = 0; i < size; i++)
    {
        std::cout << out[i] << std::endl;
    }

    QCOMPARE(out[0].real(), result[0].real());
    QCOMPARE(out[0].imag(), result[0].imag());
    QCOMPARE(out[1].real(), result[1].real());
    QCOMPARE(out[1].imag(), result[1].imag());
    QCOMPARE(out[2].real(), result[2].real());
    QCOMPARE(out[2].imag(), result[2].imag());
    QCOMPARE(out[3].real(), result[3].real());
    QCOMPARE(out[3].imag(), result[3].imag());
    QCOMPARE(out[4].real(), result[4].real());
    QCOMPARE(out[4].imag(), result[4].imag());

    delete[] in;
    delete[] out;
    delete[] result;
}

void testArkan::test1_caseConvertFFTtoLog()
{
    int size = 10;
    std::complex<float> *cmplx = new std::complex<float>[size];
    //init
    cmplx[0] = {55,0};
    cmplx[1] = {-5,15.3884};
    cmplx[2] = {-5,6.88191};
    cmplx[3] = {-5,3.63271};
    cmplx[4] = {-5,1.6246};
    cmplx[5] = {-5,0};
    cmplx[6] = {-5,-1.6246};
    cmplx[7] = {-5,-3.63271};
    cmplx[8] = {-5,-6.88191};
    cmplx[9] = {-5,-15.3884};

    float *in = new float[size];

    for(int i = 0; i < size; i++)
    {
        in[i] = sqrt(cmplx[i].real()*cmplx[i].real() + cmplx[i].imag()*cmplx[i].imag());
    }

    float *out = new float[size];

    process->convertFftToLog(in, out, size);

    float *result = new float[size];
    result[0] = 1.74036;
    result[1] = 1.20899;
    result[2] = 0.929751;
    result[3] = 0.791012;
    result[4] = 0.720764;
    result[5] = 0.69897;
    result[6] = 0.720764;
    result[7] = 0.791012;
    result[8] = 0.929751;
    result[9] = 1.20899;

    QCOMPARE(out[0], result[0]);
    QCOMPARE(out[1], result[1]);
    QCOMPARE(out[2], result[2]);
    QCOMPARE(out[3], result[3]);
    QCOMPARE(out[4], result[4]);
    QCOMPARE(out[5], result[5]);
    QCOMPARE(out[6], result[6]);
    QCOMPARE(out[7], result[7]);
    QCOMPARE(out[8], result[8]);
    QCOMPARE(out[9], result[9]);

    delete[] cmplx;
}

void testArkan::test2_caseConvertFFTtoLog()
{
    //Пробуем передать отрицательные значения
    int size = 5;
    float *in = new float[size];
    for(int i = 0; i < size; i++)
    {
        in[i] = -i;
    }

    float *out = new float[size];

    process->convertFftToLog(in, out, size);

    QCOMPARE(out[0], -INFINITY);
    QCOMPARE(out[1], NAN);
    QCOMPARE(out[2], NAN);
    QCOMPARE(out[3], NAN);
    QCOMPARE(out[4], NAN);

    delete[] in;
    delete[] out;
}

void testArkan::test1_caseProcessCmplxFFT()
{
    size_t size = 5;
    int angle = 100;
    std::complex<float>* data = new std::complex<float>[size];
    for(int i = 0; i < size; i++)
    {
        data[i].real(i);
        data[i].imag(i+1);
    }

    process->processCmplxFFT(angle, data, size);
    //Проверка правильности копирования прилетевших данных
    QCOMPARE(process->_fftComplex[0].real(),0);
    QCOMPARE(process->_fftComplex[0].imag(),1);
    QCOMPARE(process->_fftComplex[1].real(),1);
    QCOMPARE(process->_fftComplex[1].imag(),2);
    QCOMPARE(process->_fftComplex[2].real(),2);
    QCOMPARE(process->_fftComplex[2].imag(),3);
    QCOMPARE(process->_fftComplex[3].real(),3);
    QCOMPARE(process->_fftComplex[3].imag(),4);
    QCOMPARE(process->_fftComplex[4].real(),4);
    QCOMPARE(process->_fftComplex[4].imag(),5);

    //Тест гипотинузы
    QCOMPARE(process->_fftAbs[0], 1);
    QCOMPARE(process->_fftAbs[1], 2.23606801033);
    QCOMPARE(process->_fftAbs[2], 3.60555124283);
    QCOMPARE(process->_fftAbs[3], 5);
    QCOMPARE(process->_fftAbs[4], 6.40312433243);

    QCOMPARE(process->_8bitBuffer[0], 0);
    QCOMPARE(process->_8bitBuffer[1], 0.349485009909);
    QCOMPARE(process->_8bitBuffer[2], 0.556971669197);
    QCOMPARE(process->_8bitBuffer[3], 0.698970019817);
    QCOMPARE(process->_8bitBuffer[4], 0.806391954422);

    QCOMPARE(process->_2bitbuffer[0], 0);
    QCOMPARE(process->_2bitbuffer[1], 0.349485009909);
    QCOMPARE(process->_2bitbuffer[2], 0.556971669197);
    QCOMPARE(process->_2bitbuffer[3], 0.698970019817);
    QCOMPARE(process->_2bitbuffer[4], 0.806391954422);

    delete[] data;
}

 void testArkan::test1_caseProcessRawCmplxI32()
 {
     size_t size = 5;
     int angle = 100;
     std::complex<int32_t>* rowData = new std::complex<int32_t>[size];
     //Заполнить только реальную часть
     for(int i = 0; i < size; i++)
     {
         rowData[i].real(i+1);
         rowData[i].imag(0);
     }

     process->processRawCmplxI32(angle, rowData, size);

     //Проверка правильности копирования данных
     QCOMPARE(process->_srcComplexI32[0].real(),1);
     QCOMPARE(process->_srcComplexI32[0].imag(),0);
     QCOMPARE(process->_srcComplexI32[1].real(),2);
     QCOMPARE(process->_srcComplexI32[1].imag(),0);
     QCOMPARE(process->_srcComplexI32[2].real(),3);
     QCOMPARE(process->_srcComplexI32[2].imag(),0);
     QCOMPARE(process->_srcComplexI32[3].real(),4);
     QCOMPARE(process->_srcComplexI32[3].imag(),0);
     QCOMPARE(process->_srcComplexI32[4].real(),5);
     QCOMPARE(process->_srcComplexI32[4].imag(),0);

     //Проверка правильности работы FFT
     QCOMPARE(process->_fftOutputBuffer[0].real(),15);
     QCOMPARE(process->_fftOutputBuffer[0].imag(),0);
     QCOMPARE(process->_fftOutputBuffer[1].real(),-2.5);
     QCOMPARE(process->_fftOutputBuffer[1].imag(), 3.44095468521);
     QCOMPARE(process->_fftOutputBuffer[2].real(),-2.5);
     QCOMPARE(process->_fftOutputBuffer[2].imag(),0.812299251556);
     QCOMPARE(process->_fftOutputBuffer[3].real(),-2.5);
     QCOMPARE(process->_fftOutputBuffer[3].imag(),-0.812299251556);
     QCOMPARE(process->_fftOutputBuffer[4].real(),-2.5);
     QCOMPARE(process->_fftOutputBuffer[4].imag(),-3.44095468521);

     //Проверка конвертирования в log _8bitbuf
     QCOMPARE(process->_8bitBuffer[0], 1.17609131336f);
     QCOMPARE(process->_8bitBuffer[1], 0.628721296787);

     //Проверка конверироавния в log _2bitbuf
     QCOMPARE(process->_2bitbuffer[0], 1.17609131336f);
     QCOMPARE(process->_2bitbuffer[1], 0.628721296787);

     delete[] rowData;
 }

 void testArkan::test2_caseProcessRawCmplxI32()
 {
     size_t size = 20;
     int angle = 100;
     int count = 5;
     std::complex<int32_t>* rowData = new std::complex<int32_t>[size];
     for(int i = 0; i < size; i++)
     {
         rowData[i].real(count); // 5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100
         rowData[i].imag(0);
         count += 5;
     }

     process->processRawCmplxI32(angle, rowData, size);

     //Проверка правильости копирования данных первых 5 и последних 5
     QCOMPARE(process->_srcComplexI32[0].real(),5);
     QCOMPARE(process->_srcComplexI32[0].imag(),0);
     QCOMPARE(process->_srcComplexI32[1].real(),10);
     QCOMPARE(process->_srcComplexI32[1].imag(),0);
     QCOMPARE(process->_srcComplexI32[2].real(),15);
     QCOMPARE(process->_srcComplexI32[2].imag(),0);
     QCOMPARE(process->_srcComplexI32[3].real(),20);
     QCOMPARE(process->_srcComplexI32[3].imag(),0);
     QCOMPARE(process->_srcComplexI32[4].real(),25);
     QCOMPARE(process->_srcComplexI32[4].imag(),0);
     QCOMPARE(process->_srcComplexI32[15].real(),80);
     QCOMPARE(process->_srcComplexI32[15].imag(),0);
     QCOMPARE(process->_srcComplexI32[16].real(),85);
     QCOMPARE(process->_srcComplexI32[16].imag(),0);
     QCOMPARE(process->_srcComplexI32[17].real(),90);
     QCOMPARE(process->_srcComplexI32[17].imag(),0);
     QCOMPARE(process->_srcComplexI32[18].real(),95);
     QCOMPARE(process->_srcComplexI32[18].imag(),0);
     QCOMPARE(process->_srcComplexI32[19].real(),100);
     QCOMPARE(process->_srcComplexI32[19].imag(),0);

     //Проверка FFT
     QCOMPARE(process->_fftOutputBuffer[0].real(), 1050);
     QCOMPARE(process->_fftOutputBuffer[0].imag(), 0);
     QCOMPARE(process->_fftOutputBuffer[1].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[1].imag(), 315.688f);
     QCOMPARE(process->_fftOutputBuffer[2].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[2].imag(), 153.884f);
     QCOMPARE(process->_fftOutputBuffer[3].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[3].imag(), 98.131f);
     QCOMPARE(process->_fftOutputBuffer[4].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[4].imag(), 68.819f);
     QCOMPARE(process->_fftOutputBuffer[5].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[5].imag(), 50);
     QCOMPARE(process->_fftOutputBuffer[6].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[6].imag(), 36.327f);
     QCOMPARE(process->_fftOutputBuffer[7].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[7].imag(), 25.4763f);
     QCOMPARE(process->_fftOutputBuffer[8].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[8].imag(), 16.246f);
     QCOMPARE(process->_fftOutputBuffer[9].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[9].imag(), 7.91922f);
     QCOMPARE(process->_fftOutputBuffer[10].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[10].imag(), 0);
     QCOMPARE(process->_fftOutputBuffer[11].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[11].imag(), -7.91922f);
     QCOMPARE(process->_fftOutputBuffer[12].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[12].imag(), -16.246f);
     QCOMPARE(process->_fftOutputBuffer[13].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[13].imag(), -25.4763f);
     QCOMPARE(process->_fftOutputBuffer[14].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[14].imag(), -36.327f);
     QCOMPARE(process->_fftOutputBuffer[15].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[15].imag(), -50);
     QCOMPARE(process->_fftOutputBuffer[16].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[16].imag(), -68.819f);
     QCOMPARE(process->_fftOutputBuffer[17].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[17].imag(), -98.131f);
     QCOMPARE(process->_fftOutputBuffer[18].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[18].imag(), -153.884f);
     QCOMPARE(process->_fftOutputBuffer[19].real(), -50);
     QCOMPARE(process->_fftOutputBuffer[19].imag(), -315.688f);

     //Проверка конвертирования в log _8bitbuf
     QCOMPARE(process->_8bitBuffer[0], 3.02119f);
     QCOMPARE(process->_8bitBuffer[1], 2.504634f);
     QCOMPARE(process->_8bitBuffer[2], 2.20898f);
     QCOMPARE(process->_8bitBuffer[3], 2.04192f);
     QCOMPARE(process->_8bitBuffer[4], 1.92975f);
     QCOMPARE(process->_8bitBuffer[5], 1.8495f);
     QCOMPARE(process->_8bitBuffer[6], 1.79101f);
     QCOMPARE(process->_8bitBuffer[7], 1.74909f);
     QCOMPARE(process->_8bitBuffer[8], 1.720758f);
     QCOMPARE(process->_8bitBuffer[9], 1.70435f);
     delete[] rowData;
 }

 void testArkan::test1_caseProcessCmplxFlt()
 {
    size_t size = 12;
    int angle = 100;
    std::complex<float>* rowData = new std::complex<float>[size];
    for(int i = 0; i < size; i++)
    {
        rowData[i].real(i * 1.2);
        rowData[i].imag(0);
    }
    process->processCmplxFlt(angle, rowData, size);

    //Проверка правильности копирования данных
    QCOMPARE(process->_srcComplexFlt[0].real(), 0);
    QCOMPARE(process->_srcComplexFlt[0].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[1].real(), 1.2f);
    QCOMPARE(process->_srcComplexFlt[1].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[2].real(), 2.4f);
    QCOMPARE(process->_srcComplexFlt[2].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[3].real(), 3.6f);
    QCOMPARE(process->_srcComplexFlt[3].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[4].real(), 4.8f);
    QCOMPARE(process->_srcComplexFlt[4].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[5].real(), 6.0f);
    QCOMPARE(process->_srcComplexFlt[5].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[6].real(), 7.2f);
    QCOMPARE(process->_srcComplexFlt[6].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[7].real(), 8.4f);
    QCOMPARE(process->_srcComplexFlt[7].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[8].real(), 9.6f);
    QCOMPARE(process->_srcComplexFlt[8].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[9].real(), 10.8f);
    QCOMPARE(process->_srcComplexFlt[9].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[10].real(), 12.0f);
    QCOMPARE(process->_srcComplexFlt[10].imag(), 0);
    QCOMPARE(process->_srcComplexFlt[11].real(), 13.2f);
    QCOMPARE(process->_srcComplexFlt[11].imag(), 0);

    //Проверка преобразования FFT
    QCOMPARE(process->_fftOutputBuffer[0].real(), 79.2f);
    QCOMPARE(process->_fftOutputBuffer[0].imag(), 0);
    QCOMPARE(process->_fftOutputBuffer[1].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[1].imag(), 26.871f);
    QCOMPARE(process->_fftOutputBuffer[2].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[2].imag(), 12.4708f);
    QCOMPARE(process->_fftOutputBuffer[3].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[3].imag(), 7.2f);
    QCOMPARE(process->_fftOutputBuffer[4].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[4].imag(), 4.15692f);
    QCOMPARE(process->_fftOutputBuffer[5].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[5].imag(), 1.92923f);
    QCOMPARE(process->_fftOutputBuffer[6].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[6].imag(), 0);
    QCOMPARE(process->_fftOutputBuffer[7].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[7].imag(), -1.92923f);
    QCOMPARE(process->_fftOutputBuffer[8].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[8].imag(), -4.15692f);
    QCOMPARE(process->_fftOutputBuffer[9].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[9].imag(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[10].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[10].imag(), -12.4708f);
    QCOMPARE(process->_fftOutputBuffer[11].real(), -7.2f);
    QCOMPARE(process->_fftOutputBuffer[11].imag(), -26.871f);

    //Проверка преобразования в log _8bitbuf
    QCOMPARE(process->_8bitBuffer[0], 1.8987252f);
    QCOMPARE(process->_8bitBuffer[1], 1.44434f);
    QCOMPARE(process->_8bitBuffer[2], 1.1583624f);
    QCOMPARE(process->_8bitBuffer[3], 1.007848f);
    QCOMPARE(process->_8bitBuffer[4], 0.9198019f);
    QCOMPARE(process->_8bitBuffer[5], 0.872389f);

    delete[] rowData;
 }

QTEST_APPLESS_MAIN(testArkan)

#include "tst_testarkan.moc"
