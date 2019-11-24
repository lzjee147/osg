#include "OsgExTool/material.h"

Material::Material()
{
    //ctor
}

Material::Material(float _reflectiveness)
{
    reflectiveness=_reflectiveness;
}
Material::~Material()
{
    //dtor
}
float Material::getRef()
{
    return reflectiveness;
}
void Material::setRef(float _reflectiveness)
{
    reflectiveness=_reflectiveness;
}
Color Material::sample(const CRay& ray,const GVector3& position,const GVector3& normal)
{
    return Color();
}
