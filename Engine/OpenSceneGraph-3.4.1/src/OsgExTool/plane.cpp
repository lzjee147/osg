#include "OsgExTool/gvector3.h"
#include "OsgExTool/plane.h"

Plane::Plane()
{
    //ctor
}

Plane::~Plane()
{
    //dtor
}

Plane::Plane(const GVector3& _normal,float _d)
{
    normal_=_normal;
    d=_d;
}

IntersectResult Plane::isIntersected(CRay& ray)
{
   IntersectResult result = IntersectResult::noHit();
   float a = ray.getDirection().dotMul(normal_);
   if (a <0)
   {
       result.isHit=1;
       result.object = this;
       float b = normal_.dotMul(ray.getOrigin()-normal_*d);
       result.distance = -b / a;
       result.position = ray.getPoint(result.distance);
       result.normal = normal_;
       return result;
    }
}

