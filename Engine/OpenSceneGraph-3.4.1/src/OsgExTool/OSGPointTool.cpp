
#include "OsgExTool/MaterialManger.h"
#include "OsgExTool/OSGPointTool.h"
#include <osg/DrawPixels>
#include <osg/PolygonMode>

#include <osg/ValueObject>
#include <osg/PolygonOffset>
#include <osg/Depth>

std::vector<osg::Vec3f> justSmoothPoints(std::vector<osg::Vec3f>& points)
{
    std::vector<osg::Vec3f> first;

    if(points.size() > 1)
    {
      first.push_back(points[0]);

      for(size_t i = 0; i < points.size() - 2; i++)
      {
        osg::Vec3f vec1 = points[i] - points[i + 1] ;
        osg::Vec3f vec2 = points[i + 2] - points[i + 1];

        double v1 = 0.05;
        double v2 = 0.05;

        osg::Vec3f p01 = vec1 * v1 + points[i + 1] ;
        osg::Vec3f p12 = vec2 * v2 + points[i + 1] ;

        first.push_back(p01);
        first.push_back(p12);
       }

      first.push_back(points[points.size() - 1]);
    }

    return first;
}

osg::Vec2d multiVec(osg::Vec2d f, osg::Vec2d s)
{
    return osg::Vec2d( f[0]*s[0] - f[1]*s[1], f[1] * s[0] + f[0] * s[1] );
}


std::vector<osg::Vec3f> createBezier(osg::Vec3f p0, osg::Vec3f p1, osg::Vec3f p2, int num)
{
   std::vector<osg::Vec3f> points;

   for(size_t i = 0; i < num; i++)
   {
       double t = 1.0 / num * i;
       osg::Vec3f v =  p0 * (1 - t) * (1 - t) + p1 * 2 * t * (1 -t) + p2 * t * t ;

       points.push_back(v);
   }

   return points;
}


std::vector<osg::Vec3f> getDivLineNum(std::vector<osg::Vec3f>& points, int num)
{
    double total = 0.0;

    for(size_t i = 0; i < points.size() - 1; i++)
    {
        total += (points[i+1] - points[i]).length();
    }

    double div_l = total / num;

    total = 0.0;

    std::vector<osg::Vec3f> p;


    for(size_t j = 0; j < num; j++)
    {
        for(size_t i = 0; i < points.size() - 1; i++)
        {
            osg::Vec3f v = points[i+1] - points[i];
            total += v.length();

            if(total > div_l * j)
            {
               double l = total - div_l * j;
               double ll = v.length() - l;

               v.normalize();

               p.push_back( points[i] + v * ll );

               total = 0.0;
               break;
            }
        }
    }

    return p;
}

std::vector<osg::Vec3f> getDivLineNumWindow(std::vector<osg::Vec3f>& points, int num, osg::Matrix mvp)
{
    double total = 0.0;

    for(size_t i = 0; i < points.size() - 1; i++)
    {
        total += (points[i+1] * mvp - points[i] * mvp).length();
    }

    double div_l = total / num;

    total = 0.0;

    std::vector<osg::Vec3f> p;


    for(size_t j = 0; j < num; j++)
    {
        for(size_t i = 0; i < points.size() - 1; i++)
        {
            osg::Vec3f v = points[i+1] * mvp - points[i] * mvp;
            total += v.length();

            if(total > div_l * j)
            {
               double l = total - div_l * j;
               double ll = v.length() - l;

               v.normalize();

               p.push_back( points[i] * mvp + v * ll );

               total = 0.0;
               break;
            }
        }
    }

    osg::Matrix mat = osg::Matrix::inverse(mvp);

    for(size_t j = 0; j < p.size(); j++)
    {
        p[j] = p[j] * mat;
    }

    return p;
}



