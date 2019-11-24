#include "OsgExTool/gvector3.h"
#include "OsgExTool/csphere.h"

#include "OsgExTool/intersectresult.h"
CSphere::CSphere()
{    
}
CSphere::CSphere(GVector3 c,double r)
{
center=c;
radius=r;
}
CSphere::CSphere(CSphere& s)
{
center=s.getCenter();
radius=s.getRadius();
}
CSphere::~CSphere()
{
}
void CSphere::setCenter(GVector3& c)
{
center=c;
}
void CSphere::setRadius(double r)
{
radius=r;
}
GVector3 CSphere::getCenter()
{
return center;
}
double CSphere::getRadius()
{
return radius;
}
GVector3 CSphere::getNormal(GVector3 p)
{
return p-center;
}
IntersectResult CSphere::isIntersected(CRay& _ray)
{
IntersectResult result = IntersectResult::noHit();
GVector3 v = _ray.getOrigin() - center;
float a0 = v.dotMul(v) - radius*radius;
float DdotV = _ray.getDirection().dotMul(v);

if(DdotV <= 0)
{ 
float discr = DdotV * DdotV - a0;
if (discr >= 0)
{
  result.isHit=1;
result.object = this;
result.distance=-DdotV - sqrt(discr);
result.position=_ray.getPoint(result.distance);
result.normal = result.position-center;
result.normal.normalize();
}
} 
return result;
}
