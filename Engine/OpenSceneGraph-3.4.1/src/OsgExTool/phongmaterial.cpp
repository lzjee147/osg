#include "OsgExTool/phongmaterial.h"

PhongMaterial::PhongMaterial()
{
    //ctor
}
PhongMaterial::PhongMaterial(const Color& _diffuse,const Color& _specular,const float& _shininess,float _reflectiveness)
{
    diffuse=_diffuse;
    specular=_specular;
    shininess=_shininess;
    reflectiveness=_reflectiveness;
}
PhongMaterial::~PhongMaterial()
{
    //dtor
}
Color PhongMaterial::sample(const CRay& ray,const GVector3& position,const GVector3& normal)
{
    float NdotL = normal.dotMul(lightDir);
    GVector3 H = (lightDir-ray.getDirection()).normalize();
    float NdotH = normal.dotMul(H);
    Color diffuseTerm = this->diffuse.multiply(std::max(NdotL, (float)0));
    Color specularTerm = this->specular.multiply(pow(std::max(NdotH, (float)0), this->shininess));
    return lightColor.modulate(diffuseTerm.add(specularTerm));
}
