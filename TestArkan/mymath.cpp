#include "mymath.h"

MyMath::MyMath()
{

}

void MyMath::Hypot(std::vector<std::complex<float>>* complexF, std::vector<float>* absF)
{
    for(int i = 0; i < complexF->size(); i++)
    {
      absF->insert(absF->begin() + i, abs(hypot(complexF->at(i).real(),complexF->at(i).imag())));
    }
}

void MyMath::Hypot(std::vector<std::complex<int32_t> >* complexF, std::vector<float>* absF)
{
    for(int i = 0; i < complexF->size(); i++)
    {
      absF->insert(absF->begin() + i, abs(hypot(complexF->at(i).real(),complexF->at(i).imag())));
    }
}
