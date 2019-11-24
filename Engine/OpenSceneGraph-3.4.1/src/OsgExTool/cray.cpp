#include "OsgExTool/cray.h"
CRay::CRay()
{
}
CRay::~CRay()
{
}
CRay::CRay(GVector3 o,GVector3 d)
{
    origin=o;
    direction=d;
}
void CRay::setDirection(GVector3 d)
{
    direction=d;
}
void CRay::setOrigin(GVector3 o)
{
    origin=o;
}
GVector3 CRay::getDirection() const
{
    return direction;
}
GVector3 CRay::getOrigin()
{
    return origin;
}
GVector3 CRay::getPoint(double t)
{
    return origin+direction*t;
}
