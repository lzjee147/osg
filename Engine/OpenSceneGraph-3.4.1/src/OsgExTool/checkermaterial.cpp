#include "OsgExTool/checkermaterial.h"

CheckerMaterial::CheckerMaterial()
{
    //ctor
}
CheckerMaterial::CheckerMaterial(float _scale,float _reflectiveness)
{
    scale=_scale;
    reflectiveness=_reflectiveness;
}
CheckerMaterial::~CheckerMaterial()
{
    //dtor
}
Color CheckerMaterial::sample(const CRay& ray,const GVector3& position,const GVector3& normal)
{
    float d=abs((floor(position.x * this->scale) + floor(position.z * this->scale)));
    d=fmod(d,2);
    return  d < 1 ? Color::black() : Color::white();
}
