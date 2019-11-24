#include "OsgExTool/perspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera()
{

}
PerspectiveCamera::~PerspectiveCamera()
{
}
PerspectiveCamera::PerspectiveCamera(const GVector3& _eye,const GVector3& _front,const GVector3& _refUp,float _fov)
{
    eye=_eye;
    front=_front;
    refUp=_refUp;
    fov=_fov;
    right=front.crossMul(refUp);
    up = right.crossMul(front);
    fovScale = tan(fov* (PI  * 0.5f / 180)) * 2;
}
CRay PerspectiveCamera::generateRay(float x,float y)
{
    GVector3 r = right*((x - 0.5f) * fovScale);
    GVector3 u = up*((y - 0.5f) * fovScale);
    GVector3 tmp=front+r+u;
    tmp.normalize();
    return CRay(eye,tmp);
}
