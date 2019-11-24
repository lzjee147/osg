#include "OsgExTool/union.h"

Union::Union()
{
    //ctor
}

Union::~Union()
{
    //dtor
}
void Union::push(CObject* object)

{
    cobjects.push_back(object);
}
IntersectResult Union::isIntersected(CRay& _ray)
{
    const float Infinity=1e30;
    float minDistance = Infinity;
    IntersectResult minResult = IntersectResult::noHit();
    long size=this->cobjects.size();
    for (long i=0;i<size;++i){
        IntersectResult result = this->cobjects[i]->isIntersected(_ray);
        if (result.object && (result.distance < minDistance)) {
            minDistance = result.distance;
            minResult = result;
        }
    }
    return minResult;
}
