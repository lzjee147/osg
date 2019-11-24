
#include "OsgExTool/OSGAnimation.h"
#include "OsgExTool/OSGGeodeTool.h"
#include "OsgExTool/OSGArrowTool.h"
#include "OsgExTool/OSGUniverseTool.h"
#include "OsgExTool/OSGPointTool.h"
#include <osgAnimation/Keyframe>
#include <osgAnimation/Interpolator>

#include "OsgExTool/OSGGridTool.h"

class AnimationCallback : public osg::NodeCallback
{
public:
    AnimationCallback(std::vector<osg::Vec3f>& points)
    {
        _keyframes = new osgAnimation::MatrixKeyframeContainer();

        for(size_t i = 0; i < points.size() -1; i++)
        {
            osg::Vec2f vecy = osg::Vec2f(points[i + 1][0], points[i + 1][2]) - osg::Vec2f(points[i][0], points[i][2]);
            vecy.normalize();

            osg::Vec2f vecx(vecy[1], -vecy[0]);

            osg::Matrixf trs;

            trs.makeTranslate(points[i]);

            osg::Matrixf rot;

            rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vecy[0], 0, vecy[1]));

            osg::Matrixf mat = rot * trs;

            osgAnimation::MatrixKeyframe kf(i,  mat) ;

            _keyframes->push_back(kf);
        }

        _startTick = osg::Timer::instance()->tick();

        _interpolator = new osgAnimation::Vec2LinearInterpolator();

        index_ = 0;
    }

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
        if (mt)
        {
//          double currentTick = osg::Timer::instance()->tick();
//          double elaspedTime = osg::Timer::instance()->delta_s(_startTick, currentTick);

//          osg::Vec2 currentRot;
//          _interpolator->getValue(*_keyframes, elaspedTime, currentRot);

//            if(_keyframes->size() == 0 || _keyframes->size() == index_+1)
//                return;

            if(index_ >= _keyframes->size())
                index_ = 0;

            if(_keyframes->size() > 0){

                osg::Matrixf pos = _keyframes->at(index_).getValue();

          //  osg::Matrixd tran;
          //  tran.makeTranslate(osg::Vec3f(pos[0], 0, pos[1]));

                mt->setMatrix(pos);
            }

            index_++;
        }

        traverse(node, nv);
    }

protected:

    osgAnimation::MatrixKeyframeContainer   *_keyframes;

    osgAnimation::Vec2LinearInterpolator  *_interpolator;
    double                                   _startTick;

    int index_;

};

osg::ref_ptr<osg::MatrixTransform> createMatrixTransformCallback(std::vector<osg::Vec3f> points, double w, double h, osg::Vec4 color, bool isAnimation)
{
	//osg::ref_ptr<osg::Geode> geode = createQuadAnimation(w, h, color);
	osg::ref_ptr<osg::Geode> geode = createArrowAnimation(osg::Vec2d(0, 1), 5, 1, 2, osg::Vec4(0.0, 1.0, 0.0, 1.0));

	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

	transform->addChild(geode);

	//isAnimation = OSGRenderParameter::stop_animation_;;

    if(!isAnimation && points.size() > 1)
    {
        osg::Vec2f vecy = osg::Vec2f(points[1][0], points[1][2]) - osg::Vec2f(points[0][0], points[0][2]);
        vecy.normalize();

        osg::Matrixf trs;

        trs.makeTranslate(points[0]);

        osg::Matrixf rot;

        rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vecy[0], 0, vecy[1]));

        osg::Matrixf mat = rot * trs;

        transform->setMatrix(mat);
    }
    else
    {
        osg::ref_ptr<AnimationCallback> animationCallback = new AnimationCallback(points);

        transform->addUpdateCallback(animationCallback);
    }

    return transform;
}

/***
 * draw square arrow
 * ***/
osg::ref_ptr<osg::MatrixTransform> createSquareArrowCallback(std::vector<osg::Vec3f> points, double angle, double h, double l, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geode> geode = createArrowAnimation(osg::Vec2d(0, 1), angle, h, l, color);

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
    transform->addChild(geode);

    osg::Vec2f vecy = osg::Vec2f(points[1][0], points[1][2]) - osg::Vec2f(points[0][0], points[0][2]);
    vecy.normalize();

    osg::Matrixf trs;

    trs.makeTranslate(points[0]);

    osg::Matrixf rot;
    rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vecy[0], 0, vecy[1]));
    osg::Matrixf mat = rot * trs;
    transform->setMatrix(mat);

    return transform;
}

osg::ref_ptr<osg::MatrixTransform> create3DtextCallback(std::vector<osg::Vec3f> points, std::string str, float size, osg::Vec4 color)
{
     osg::ref_ptr<osg::Geode> geode = createText(str, osg::Vec3f(0, 0, 0), size, color);

     osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
     transform->addChild(geode);

     osg::Vec2f vecy = osg::Vec2f(points[1][0], points[1][2]) - osg::Vec2f(points[0][0], points[0][2]);
     vecy.normalize();

     osg::Matrixf trs;

     trs.makeTranslate(points[0]);

     osg::Matrixf rot;
     rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vecy[0], 0, vecy[1]));
     osg::Matrixf mat = rot * trs;
     transform->setMatrix(mat);

     return transform;
}


osg::ref_ptr<osg::MatrixTransform> createNormalArrowCallback(std::vector<osg::Vec3f> points, double angle, double h, double l, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geode> geode = createNormalArrow(angle, h, l, color);

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
    transform->addChild(geode);

    osg::Vec2f vecy = osg::Vec2f(points[1][0], points[1][2]) - osg::Vec2f(points[0][0], points[0][2]);
    vecy.normalize();

    osg::Matrixf trs;

    trs.makeTranslate(points[0]);

    osg::Matrixf rot;
    rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vecy[0], 0, vecy[1]));
    osg::Matrixf mat = rot * trs;
    transform->setMatrix(mat);

    return transform;
}


class AnimationErectCallback : public osg::NodeCallback
{
public:
    AnimationErectCallback(std::vector<osg::Vec3f>& points)
    {
        _keyframes = new osgAnimation::MatrixKeyframeContainer();

        for(size_t i = 0; i < points.size() -1; i++)
        {
            osg::Vec2f vecy = osg::Vec2f(points[i + 1][0], points[i + 1][2]) - osg::Vec2f(points[i][0], points[i][2]);
            vecy.normalize();

            osg::Matrixf trs;

            trs.makeTranslate(points[i]);

            osg::Matrixf rot;

            rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vecy[0], 0, vecy[1]));

            osg::Vec3f vecc(vecy[0], 0, vecy[1]);

            osg::Vec3f z(0, 0, 1);

            osg::Vec3f dir = z ^ vecc;
            dir.normalize();

            double angle = abs(z * vecc);

            //angle = 1.0;

            if(dir[1] > 0)
            {
                //OSGRenderParameter::erect_arrow_rotate_x_ = -angle * 5;
            }
            else if(dir[1] < 0)
            {
               // OSGRenderParameter::erect_arrow_rotate_x_ = angle *5;
            }
            else if(dir[1] == 0)
            {
                //OSGRenderParameter::erect_arrow_rotate_x_ = 2;
            }

            osg::Matrixf rotvec;
           // rotvec.makeRotate(OSGRenderParameter::erect_arrow_rotate_x_/ 180 * 3.14, vecc);

            osg::Matrixf mat = rot * rotvec * trs;

            osgAnimation::MatrixKeyframe kf(i,  mat) ;

            _keyframes->push_back(kf);
        }

        _startTick = osg::Timer::instance()->tick();

        _interpolator = new osgAnimation::Vec2LinearInterpolator();

        index_ = 0;

        //
    }

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
        if (mt)
        {
            if(_keyframes->size() == 0 || _keyframes->size() == index_+1)
                return;

            if(index_ >= _keyframes->size())
                index_ = 0;

            osg::Matrixf pos = _keyframes->at(index_).getValue();

            mt->setMatrix(pos);

            index_++;
        }

        traverse(node, nv);
    }

protected:

    osgAnimation::MatrixKeyframeContainer   *_keyframes;

    osgAnimation::Vec2LinearInterpolator  *_interpolator;
    double                                   _startTick;

    int index_;

};

osg::ref_ptr<osg::MatrixTransform> createErectMatrixTransformCallback(std::vector<osg::Vec3f> points, double w, double h, osg::Vec4 color, bool isAnimation)
{
    osg::ref_ptr<osg::Geode> geode = createErectArrowAnimation(osg::Vec2d(0, 1), 5, 1, 12, osg::Vec4(0.0, 1.0, 0.0, 1.0));

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

    transform->addChild(geode);

   // isAnimation = OSGRenderParameter::stop_animation_;

    isAnimation = true;

    if(!isAnimation && points.size() > 1)
    {
        osg::Vec2f vecy = osg::Vec2f(points[1][0], points[1][2]) - osg::Vec2f(points[0][0], points[0][2]);
        vecy.normalize();

        osg::Matrixf trs;

        trs.makeTranslate(points[0]);

        osg::Matrixf rot;

        rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vecy[0], 0, vecy[1]));

        osg::Vec3f vecc(vecy[0], 0, vecy[1]);

        osg::Vec3f z(0, 0, 1);

        osg::Vec3f dir = z ^ vecc;
        dir.normalize();

        double angle = abs(z * vecc);

        //angle = ;

        if(dir[1] > 0)
        {
           // OSGRenderParameter::erect_arrow_rotate_x_ = -angle * 10;
        }
        else if(dir[1] <= 0)
        {
           // OSGRenderParameter::erect_arrow_rotate_x_ = angle *10;
        }

        osg::Matrixf rotvec;
        //rotvec.makeRotate(OSGRenderParameter::erect_arrow_rotate_x_/ 180 * 3.14, vecc);

        osg::Matrixf mat = rot * rotvec * trs;

        transform->setMatrix(mat);
    }
    else
    {
        osg::ref_ptr<AnimationErectCallback> animationCallback = new AnimationErectCallback(points);

        transform->addUpdateCallback(animationCallback);
    }

    return transform;
}

bool createAnimationNode(osg::Vec3f f, osg::Vec3f s, osg::Matrixd mvp)
{
    osg::Vec3f screenf = f * mvp;
    osg::Vec3f screens = s * mvp;

    osg::Vec3f v = screens - screenf;
    v.normalize();

    double a = osg::Vec3f(v[0], v[1], 0) * osg::Vec3(1, 0, 0);

    return a > 0.95 || a < -0.95;
}

class AnimationErectCallback_ : public osg::NodeCallback
{
public:
    AnimationErectCallback_(std::vector<osg::Vec3f>& div_point, osg::Matrixd mvp)
    {
        for(size_t i = 0; i < div_point.size(); i++)
        {
            //CreateNormalArrow
            //CreateDefArrow
             if(createAnimationNode(div_point[i], div_point[i + 1], mvp))
             {
                nodes_.push_back( createNormalArrow(osg::Vec2d(div_point[i][0], div_point[i][2]), osg::Vec2d(div_point[i + 1][0], div_point[i + 1][2]), osg::Vec4f(1.0, 0.0, 0.0, 1.0)) );
             }
             else
             {
                nodes_.push_back( createDefArrow(osg::Vec2d(div_point[i][0], div_point[i][2]), osg::Vec2d(div_point[i + 1][0], div_point[i + 1][2]), false, osg::Vec4f(1.0, 0.0, 0.0, 1.0)) );
             }
        }

        nodes_.push_back(NULL);

        index_ = 0;
    }

    virtual bool isAnimation()
    {
        return index_ == nodes_.size();
    }

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);

        if (mt)
        {
            if( nodes_.size() == index_)
            {
                index_ = 0;
            }

            if(index_ < nodes_.size())
            {
               if(mt->getNumChildren() == 1)
               {
                  mt->removeChildren(0, 1) ;
               }

               mt->addChild(nodes_[index_]);

               index_++;
            }
        }

        traverse(node, nv);
    }

protected:

    std::vector<osg::ref_ptr<osg::Node>> nodes_;

    int index_;
};


//class AnimationErectCallback_ : public osg::NodeCallback
//{
//public:
//    AnimationErectCallback_(std::vector<osg::Vec3f>& div_point, osg::Matrixd mvp)
//    {
//        _mat = mvp;

//        for(size_t i = 0; i < div_point.size(); i++)
//        {
//            //CreateNormalArrow
//            //CreateDefArrow
////             if(createAnimationNode(div_point[i], div_point[i + 1], mvp))
////             {
////                nodes_.push_back( CreateNormalArrow(osg::Vec2d(div_point[i][0], div_point[i][2]), osg::Vec2d(div_point[i + 1][0], div_point[i + 1][2]), true, osg::Vec4f(1.0, 0.0, 0.0, 1.0)) );
////             }
////             else
////             {
////                nodes_.push_back( CreateDefArrow(osg::Vec2d(div_point[i][0], div_point[i][2]), osg::Vec2d(div_point[i + 1][0], div_point[i + 1][2]), false, osg::Vec4f(1.0, 0.0, 0.0, 1.0)) );
////             }

//            XXX_.push_back(std::make_pair(osg::Vec2d(div_point[i][0], div_point[i][2]), osg::Vec2d(div_point[i + 1][0], div_point[i + 1][2])));
//        }

//       // nodes_.push_back(NULL);

//        index_ = 0;
//    }

//    virtual bool isAnimation()
//    {
//        return index_ == XXX_.size();
//    }

//    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//    {
//        osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);

//        if (mt)
//        {
//            if( XXX_.size() - 1 == index_)
//            {
//                index_ = 0;
//            }

//            if(index_ < XXX_.size() - 1)
//            {
//               mt->removeChildren(0, mt->getNumChildren());

//              if(createAnimationNode(osg::Vec3f(XXX_[index_].first[0], 0, XXX_[index_].first[2]), osg::Vec3f(XXX_[index_].second[0], 0, XXX_[index_].second[2]), _mat))
//              {
//                 mt->addChild(CreateNormalArrow(XXX_[index_].first, XXX_[index_].second, osg::Vec4f(1.0, 0.0, 0.0, 1.0)));
//              }
//              else
//              {
//                 mt->addChild(CreateDefArrow(XXX_[index_].first, XXX_[index_].second, false, osg::Vec4f(1.0, 0.0, 0.0, 1.0)));
//              }

//              index_ ++;
//            }
//        }

//        traverse(node, nv);
//    }

//protected:

//    std::vector<osg::ref_ptr<osg::Node>> nodes_;

//    std::vector<std::pair<osg::Vec2d, osg::Vec2d>> XXX_;

//    int index_;

//    osg::Matrix _mat;
//};

osg::ref_ptr<osg::MatrixTransform> createErectMatrixTransformCallback_(std::vector<osg::Vec3f> points, osg::Matrixd mvp, bool isAnimation)
{
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

    osg::Vec3f f = points[0];
    osg::Vec3f s = points[1];

    //  CreateNormalArrow
    //  CreateDefArrow
    if(createAnimationNode(f, s, mvp))
    {
       transform->addChild( createNormalArrow(osg::Vec2d(f[0], f[2]), osg::Vec2d(s[0], s[2]), osg::Vec4f(1.0, 0.0, 0.0, 1.0)) );
    }
   else
    {
       transform->addChild( createDefArrow(osg::Vec2d(f[0], f[2]), osg::Vec2d(s[0], s[2]), false, osg::Vec4f(1.0, 0.0, 0.0, 1.0)) );
    }

    if(isAnimation)
    {
        osg::ref_ptr<AnimationErectCallback_> animationCallback = new AnimationErectCallback_(points, mvp);

        transform->addUpdateCallback(animationCallback);
    }

    return transform;
}

OSGAnimation::OSGAnimation()
{
    animation_ = new osg::MatrixTransform();
}

void OSGAnimation::removeAnimation()
{
    grout_root_->removeChild(animation_);
}

void OSGAnimation::startAnimation()
{
    createAnimation(pointsx_, num_, mvp_, true);
}

void OSGAnimation::plotModel()
{
    createAnimation(pointsx_, num_, mvp_, false);
}

void OSGAnimation::stopAnimation()
{
    grout_root_->removeChild(animation_);
}

bool OSGAnimation::isAnimation()
{
    return grout_root_->getChildIndex(animation_) != grout_root_->getNumChildren();
}

void OSGAnimation::setRootGroup(osg::ref_ptr<osg::Group> group)
{
    grout_root_ = group;
}

void OSGAnimation::createAnimation(std::vector<osg::Vec3f> pointsx, int num, osg::Matrixd mvp, bool isanimation)
{
    pointsx_ = pointsx;
    num_     = num    ;
    mvp_     = mvp    ;

    grout_root_->removeChild(animation_);
    animation_ = new osg::MatrixTransform();

    std::vector<osg::Vec3f> div_point = getDivLineNumWindow(pointsx, num, mvp);

    //Animation
    for(size_t j = 0; j < div_point.size(); j ++)
    {
          std::vector<osg::Vec3f> pathPoint;
          size_t k = j;

          for(; k < div_point.size(); k++)
          {
              pathPoint.push_back(div_point[k]);
          }

          if(pathPoint.size()  > 1)
          {
             animation_->addChild( createErectMatrixTransformCallback_(pathPoint, mvp, isanimation) );
          }
     }

     grout_root_->addChild(animation_);
}


