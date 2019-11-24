/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2010 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
 *
 * ModelBimOrbitManipulator code Copyright (C) 2010 PCJohn (Jan Peciva)
 * while some pieces of code were taken from OSG.
 * Thanks to company Cadwork (www.cadwork.ch) and
 * Brno University of Technology (www.fit.vutbr.cz) for open-sourcing this work.
*/

#include <osgGA/ModelBimOrbitManipulator>
#include <osg/BoundsChecking>
#include <cassert>

#include "windows.h"
#include "osgDB/FileUtils"

#include <osg/BlendFunc>

#include <osg/PolygonMode>

using namespace osg;
using namespace osgGA;


int ModelBimOrbitManipulator::_minimumDistanceFlagIndex = allocateRelativeFlag();

osg::ref_ptr<osg::Geometry> CreateFacecccc(osg::Vec3d v1, osg::Vec3d v2, osg::Vec3d v3, osg::Vec3d v4)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;  

	osg::ref_ptr<osg::Vec3Array> va = new osg::Vec3Array(); 

	osg::StateSet *dstate = geom->getOrCreateStateSet();

	va->push_back(v1);
	va->push_back(v2);
	va->push_back(v3);
	va->push_back(v4);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f, 0.2f, 0.0f, 0.5f));
	geom->setColorArray(colors, osg::Array::BIND_OVERALL);

	geom->setVertexArray( va.get() );  

	osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
	polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	dstate->setAttribute( polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

	geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, va->size()) );

	return geom;
}

/// Constructor.
ModelBimOrbitManipulator::ModelBimOrbitManipulator( int flags )
   : inherited( flags ),
     _distance( 1. ),
     _trackballSize( 0.8 )
{
    setMinimumDistance( 0.05, true );
    setWheelZoomFactor( 0.1 );
    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
        setAnimationTime( 0.2 );

	_mouse_push.set(osg::Vec3d(0, 0, 0));
	_cameraspeedtime = 1;
	_isviewmoveing = false;

	_trans_total.set(0, 0, 0);
	_scale_total.set(1, 1, 1);

	//windowManager_ = windowManager;

	//_box_mouse = new osgWidget::Widget();

	//windowManager_->addChild(_box_mouse);

	//_box_mouse->setWidth(30);
	//_box_mouse->setHeight(30);
	std::string _currentworkpath = osgDB::getCurrentWorkingDirectory();
	//_box_mouse->setImage(_currentworkpath+"//picture//10.png", true);

	_select_node = NULL;
	ismovecurrentgroup = true;

	geodes_ = new osg::Geode();

	plane_ = false;
}


/// Constructor.
ModelBimOrbitManipulator::ModelBimOrbitManipulator( const ModelBimOrbitManipulator& om, const CopyOp& copyOp )
	: osg::Callback(om, copyOp),
	inherited( om, copyOp ),
	_center( om._center ),
	_rotation( om._rotation ),
	_distance( om._distance ),
	_trackballSize( om._trackballSize ),
	_wheelZoomFactor( om._wheelZoomFactor ),
	_minimumDistance( om._minimumDistance )
{
	_mouse_push.set(osg::Vec3d(0, 0, 0));
}


/** Set the position of the manipulator using a 4x4 matrix.*/
void ModelBimOrbitManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    _center = osg::Vec3d( 0., 0., -_distance ) * matrix;
    _rotation = matrix.getRotate();

    // fix current rotation
    if( getVerticalAxisFixed() )
        fixVerticalAxis( _center, _rotation, true );
	_viewMatrix = osg::Matrixd::translate( -_center ) *
		osg::Matrixd::rotate( _rotation.inverse() ) *
		osg::Matrixd::translate( 0.0, 0.0, -_distance );
}


/** Set the position of the manipulator using a 4x4 matrix.*/
void ModelBimOrbitManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
    setByMatrix( osg::Matrixd::inverse( matrix ) );
}


/** Get the position of the manipulator as 4x4 matrix.*/
osg::Matrixd ModelBimOrbitManipulator::getMatrix() const
{
    return osg::Matrixd::translate( 0., 0., _distance ) *
           osg::Matrixd::rotate( _rotation ) *
           osg::Matrixd::translate( _center );
}

osg::Matrixd ModelBimOrbitManipulator::getHomeMatrix() const
{
	return _viewMatrixInit;
}

bool ModelBimOrbitManipulator::handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	double current_frame_time = ea.getTime();

	_delta_frame_time = current_frame_time - _last_frame_time;
	_last_frame_time = current_frame_time;

	if (_isviewmoveing)
	{
		_curmoveradio = min(1.0,_curmoveradio+_delta_frame_time/_cameraspeedtime);
		BeginAutomatic();
		us.requestRedraw();
	}

	if( _animationData && _animationData->_isAnimating )
	{
		performAnimationMovement( ea, us );
	}

	return false;
}

bool ModelBimOrbitManipulator::handleKeyDown( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
	if( ea.getKey() == GUIEventAdapter::KEY_Space )
	{	
		ComputerRotateData();

		return true;
	}

	return false;
}

void ModelBimOrbitManipulator::ComputerRotateData()
{
	_rotationstemp.clear()  ;
	_eyestemp.clear();

	osg::Quat quat, quatInit;

	Vec3d eyecur;
	getTransformation( eyecur,quat);

	/*if( getAutoComputeHomePosition() )
		computeHomePosition( NULL, ( _flags & COMPUTE_HOME_USING_BBOX ) != 0 );*/

	quatInit = GetQuat(_homeEye, _homeCenter, _homeUp);

  	_eyestemp.push_back(eyecur);
	_eyestemp.push_back(_homeEye);

	_rotationstemp.push_back(quat);
	_rotationstemp.push_back(quatInit);
	_isviewmoveing = true;
	_curmoveradio = 0;
}

void ModelBimOrbitManipulator::ComputerRotateTo(Vec3d eyeto,Vec3d centerto,Vec3d upto)
{
	_rotationstemp.clear()  ;
	_eyestemp.clear();

	osg::Quat quat, quatInit;

	Vec3d eyecur;
	getTransformation( eyecur,quat);

	quatInit = GetQuat(eyeto, centerto, upto);

	_eyestemp.push_back(eyecur);
	_eyestemp.push_back(eyeto);

	_rotationstemp.push_back(quat);
	_rotationstemp.push_back(quatInit);
	_isviewmoveing = true;
	_curmoveradio = 0;
}

void ModelBimOrbitManipulator::BeginAutomatic()
{
	if (_curmoveradio>=1)
	{
		_isviewmoveing = false;
	}
	Vec3d eyescur = Vec3DLerp(_curmoveradio,_eyestemp[0],_eyestemp[1]);
	osg::Quat q;
	q.slerp(_curmoveradio, _rotationstemp[0] ,  _rotationstemp[1]);
	setTransformation(eyescur,q);
}

osg::Quat ModelBimOrbitManipulator::GetQuat( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
{
	Vec3d lv( center - eye );

	Vec3d f( lv );
	f.normalize();
	Vec3d s( f^up );
	s.normalize();
	Vec3d u( s^f );
	u.normalize();

	osg::Matrixd rotation_matrix( s[0], u[0], -f[0], 0.0f,
		s[1], u[1], -f[1], 0.0f,
		s[2], u[2], -f[2], 0.0f,
		0.0f, 0.0f,  0.0f, 1.0f );

	osg::Vec3d _center_ = center;
	float _distance_ = lv.length();
	osg::Quat _rotation_ = rotation_matrix.getRotate().inverse();

	return  _rotation_;
}

osg::Vec3d ModelBimOrbitManipulator::Vec3DLerp(double num,Vec3d eyeold,Vec3d eyeinit)
{
	double num2 = 1.0 - num;
	Vec3d neweye;
	neweye.x() = eyeold.x()*num2+eyeinit.x()*num;
	neweye.y() = eyeold.y()*num2+eyeinit.y()*num;
	neweye.z() = eyeold.z()*num2+eyeinit.z()*num;
	return neweye;
}

void ModelBimOrbitManipulator::setViewpointXYZ(osg::BoundingSphere& boundingSphere)
{
	osg::Vec3d center, eye, up;

	center = boundingSphere.center();

	double r = boundingSphere.radius();

	osg::Vec3d n(-1, -1, 1);
	n.normalize(); 

	eye =  center +  n * r * 2.8;

	up.set(1, 1, 0);

	setHomePosition(eye, center, up);

	home(0.0);
}

/** Get the position of the manipulator as a inverse matrix of the manipulator,
    typically used as a model view matrix.*/
osg::Matrixd ModelBimOrbitManipulator::getInverseMatrix() const
{
	/*return osg::Matrixd::translate( -_center ) *
           osg::Matrixd::rotate( _rotation.inverse() ) *
           osg::Matrixd::translate( 0.0, 0.0, -_distance );*/
	if (_isviewmoveing)
	{
		return osg::Matrixd::translate( -_center ) *
           osg::Matrixd::rotate( _rotation.inverse() ) *
           osg::Matrixd::translate( 0.0, 0.0, -_distance );
	} 
	else
	{
		return _viewMatrix;
	}

	/*if(_ga_t1.get() && _ga_t1->getButtonMask() == GUIEventAdapter::MIDDLE_MOUSE_BUTTON )
	{
		return  osg::Matrixd::translate( - _center- _mouse_push) * 
			osg::Matrixd::rotate( _rotation.inverse() ) * osg::Matrixd::translate(_mouse_push) * 
			osg::Matrixd::translate( 0.0, 0.0, -_distance );
	}
	else
	{
		return  osg::Matrixd::translate( - _center) *
			osg::Matrixd::rotate( _rotation.inverse() ) * 
			osg::Matrixd::translate( 0.0, 0.0, -_distance );
	}*/
}


// doc in parent
void ModelBimOrbitManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
{
    _center = eye + rotation * osg::Vec3d( 0., 0., -_distance );
    _rotation = rotation;

    // fix current rotation
    if( getVerticalAxisFixed() )
        fixVerticalAxis( _center, _rotation, true );

	_viewMatrix = osg::Matrixd::translate( -_center ) *
		osg::Matrixd::rotate( _rotation.inverse() ) *
		osg::Matrixd::translate( 0.0, 0.0, -_distance );
}


// doc in parent
void ModelBimOrbitManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const
{
    eye = _center - _rotation * osg::Vec3d( 0., 0., -_distance );
    rotation = _rotation;
}


// doc in parent
void ModelBimOrbitManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
{
	
    Vec3d lv( center - eye );

    Vec3d f( lv );
    f.normalize();
    Vec3d s( f^up );
    s.normalize();
    Vec3d u( s^f );
    u.normalize();

    osg::Matrixd rotation_matrix( s[0], u[0], -f[0], 0.0f,
                            s[1], u[1], -f[1], 0.0f,
                            s[2], u[2], -f[2], 0.0f,
                            0.0f, 0.0f,  0.0f, 1.0f );

    _center = center;
    _distance = lv.length();
    _rotation = rotation_matrix.getRotate().inverse();

    // fix current rotation
    if( getVerticalAxisFixed() )
        fixVerticalAxis( _center, _rotation, true );
	_viewMatrix.makeLookAt(eye,center,up);
}


// doc in parent
void ModelBimOrbitManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const
{
	center = _center;
	eye = _center + _rotation * osg::Vec3d( 0., 0., _distance );
	up = _rotation * osg::Vec3d( 0., 1., 0. );
}


/** Sets the transformation by heading. Heading is given as an angle in radians giving a azimuth in xy plane.
    Its meaning is similar to longitude used in cartography and navigation.
    Positive number is going to the east direction.*/
void ModelBimOrbitManipulator::setHeading( double azimuth )
{
    CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
    Vec3d localUp = getUpVector( coordinateFrame );
    Vec3d localRight = getSideVector( coordinateFrame );

    Vec3d dir = Quat( getElevation(), localRight ) * Quat( azimuth, localUp ) * Vec3d( 0., -_distance, 0. );

    setTransformation( _center + dir, _center, localUp );
}


/// Returns the heading in radians. \sa setHeading
double ModelBimOrbitManipulator::getHeading() const
{
    CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
    Vec3d localFront = getFrontVector( coordinateFrame );
    Vec3d localRight = getSideVector( coordinateFrame );

    Vec3d center, eye, tmp;
    getTransformation( eye, center, tmp );

    Plane frontPlane( localFront, center );
    double frontDist = frontPlane.distance( eye );
    Plane rightPlane( localRight, center );
    double rightDist = rightPlane.distance( eye );

    return atan2( rightDist, -frontDist );
}


/** Sets the transformation by elevation. Elevation is given as an angle in radians from xy plane.
    Its meaning is similar to latitude used in cartography and navigation.
    Positive number is going to the north direction, negative to the south.*/
void ModelBimOrbitManipulator::setElevation( double elevation )
{
    CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
    Vec3d localUp = getUpVector( coordinateFrame );
    Vec3d localRight = getSideVector( coordinateFrame );

    Vec3d dir = Quat( -elevation, localRight ) * Quat( getHeading(), localUp ) * Vec3d( 0., -_distance, 0. );

    setTransformation( _center + dir, _center, localUp );
}


/// Returns the elevation in radians. \sa setElevation
double ModelBimOrbitManipulator::getElevation() const
{
    CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
    Vec3d localUp = getUpVector( coordinateFrame );
    localUp.normalize();

    Vec3d center, eye, tmp;
    getTransformation( eye, center, tmp );

    Plane plane( localUp, center );
    double dist = plane.distance( eye );

    return asin( -dist / (eye-center).length() );
}


// doc in parent
bool ModelBimOrbitManipulator::handleMouseWheel( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

    // handle centering
    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
    {

        if( ((sm == GUIEventAdapter::SCROLL_DOWN && _wheelZoomFactor > 0.)) ||
            ((sm == GUIEventAdapter::SCROLL_UP   && _wheelZoomFactor < 0.)) )
        {

            if( getAnimationTime() <= 0. )
            {
                // center by mouse intersection (no animation)
                setCenterByMousePointerIntersection( ea, us );
            }
            else
            {
                // start new animation only if there is no animation in progress
                if( !isAnimating() )
                    startAnimationByMousePointerIntersection( ea, us );

            }

        }
    }

    switch( sm )
    {
        // mouse scroll up event
        case GUIEventAdapter::SCROLL_UP:
        {
            // perform zoom
            zoomModel( 0.1, true );
            us.requestRedraw();
            us.requestContinuousUpdate( isAnimating() || _thrown );
            return true;
        }

        // mouse scroll down event
        case GUIEventAdapter::SCROLL_DOWN:
        {
            // perform zoom
            zoomModel( -0.1, true );
            us.requestRedraw();
            us.requestContinuousUpdate( isAnimating() || _thrown );
            return true;
        }

        // unhandled mouse scrolling motion
        default:
            return false;
   }
}


// doc in parent
bool ModelBimOrbitManipulator::performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    rotateTrackball( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
                         _ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
                         getThrowScale( eventTimeDelta ) );
    return true;
}

void ModelBimOrbitManipulator::setSelectNode(osg::Node* node, bool plane)
{
    _select_node = node;

	plane_ = plane;
}

void ModelBimOrbitManipulator::setSelectPlane(osg::Group* group, osg::BoundingSphere sphere)
{
	osg::Vec3 c = sphere.center();

	double r = sphere.radius() * 20;

	_select_node = new osg::Geode();

	osg::Geode* p = (osg::Geode*)_select_node;

	p->addDrawable(CreateFacecccc(osg::Vec3d(r, r, 0) + c, osg::Vec3d(-r, r, 0) + c, osg::Vec3d(-r, -r, 0) + c, osg::Vec3d(r, -r, 0) + c));

	group->addChild(_select_node);

//	_select_node->setNodeMask(0x0);

	plane_ = true;
}

osg::Vec3d ModelBimOrbitManipulator::WindowToWorldPoint(const osgGA::GUIEventAdapter& event, float p)
{
	osg::Vec3d startVertex;

	if (event.getNumPointerData()>=1)
	{
		const osgGA::PointerData* pd = event.getPointerData(event.getNumPointerData()-1);
		const osg::Camera* camera = dynamic_cast<const osg::Camera*>(pd->object.get());

		osg::Matrixd matrix;

		matrix.postMult(camera->getViewMatrix());
		matrix.postMult(camera->getProjectionMatrix());

		osg::Matrixd inverse;
		inverse.invert(matrix);

		startVertex = osg::Vec3d(pd->getXnormalized(),pd->getYnormalized(),p) * inverse;
	}

	return startVertex;
}

class LineSegmentIntersector_Manipulator : public osgUtil::LineSegmentIntersector
{
public:
	LineSegmentIntersector_Manipulator(CoordinateFrame cf, const osg::Vec3d& start, const osg::Vec3d& end) : LineSegmentIntersector(cf, start, end,  NULL, osgUtil::Intersector::NO_LIMIT)
	{

	}


public:
	bool enter(const osg::Node& node)
	{
		return true;
	}
};


bool ModelBimOrbitManipulator::Pick(const osgGA::GUIEventAdapter& event, osg::Node* node, osg::Vec3d& select_result)
{
	//
	const osgGA::PointerData* pd = event.getPointerData(event.getNumPointerData()-1);
	const osg::Camera* camera = dynamic_cast<const osg::Camera*>(pd->object.get());

	if (!node || !camera)
	{
		return false;
	}
	
	osgUtil::LineSegmentIntersector::Intersections intersections;

	osg::Vec3d f = WindowToWorldPoint(event, -1);
	osg::Vec3d s = WindowToWorldPoint(event, 1) ;

	osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new LineSegmentIntersector_Manipulator(osgUtil::Intersector::MODEL, f, s);
	osgUtil::IntersectionVisitor iv(picker.get());
	iv.setTraversalMask(0xffffffff);

	node->accept(iv);

	if (picker->containsIntersections() && !plane_)
	{
		intersections = picker->getIntersections();
		select_result = (*intersections.begin()).getWorldIntersectPoint();

		return true;
	}
	else
	{
		osg::ref_ptr< osgUtil::LineSegmentIntersector > pickerr = new LineSegmentIntersector_Manipulator(osgUtil::Intersector::MODEL, f, s);

		osgUtil::IntersectionVisitor ivv(pickerr.get());
		ivv.setTraversalMask(0xffffffff);

		node->accept(iv);

		if (picker->containsIntersections())
		{
			intersections = picker->getIntersections();
			select_result = (*intersections.begin()).getWorldIntersectPoint();

			return true;
	     }
	}

	return false;
}


 bool ModelBimOrbitManipulator::handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
 {
	 //StandardManipulator::handleMousePush(ea, us);

	 osg::Vec3d select_result;

	 if (!_select_node)
	 {
		 return NULL;
	 }

	 if (ea.getButtonMask() == GUIEventAdapter::MIDDLE_MOUSE_BUTTON )
	 {
		 //while(ShowCursor(FALSE)>0){}
		 Pick(ea, _select_node, select_result);

		 _mouse_push = select_result;


		 _mouse_push_ga = &ea;

		// _box_mouse->setNodeMask(0xffffffff);

		// _box_mouse->setOrigin(_mouse_push_ga->getX()-15, _mouse_push_ga->getY()-15);
	 }


	 return true;
 }



bool ModelBimOrbitManipulator::performMovement()
{
	if( _ga_t0.get() == NULL || _ga_t1.get() == NULL )
		return false;

	// get delta time
	double eventTimeDelta = _ga_t0->getTime() - _ga_t1->getTime();

	if( eventTimeDelta < 0. )
	{
		OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
		eventTimeDelta = 0.;
	}

	// get deltaX and deltaY
	float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();

	// return if there is no movement.
	if( dx == 0. && dy == 0. )
		return false;

	unsigned int buttonMask = _ga_t1->getButtonMask();

	if( buttonMask == GUIEventAdapter::LEFT_MOUSE_BUTTON && ismovecurrentgroup)
	{
	    osg::Vec3d f, s;
		
		if (Pick(*_ga_t0.get(), _select_node, f) && Pick(*_ga_t1.get(), _select_node, s))
		{
			_viewMatrix = osg::RefMatrixd::translate( f - s) * _viewMatrix;

			UpdataTheMatrix(_viewMatrix);
			//setByMatrix(_viewMatrix);

			_trans_total += ( f - s);
		}

		return true;
	}
	else if( buttonMask == GUIEventAdapter::MIDDLE_MOUSE_BUTTON )
	{
		return performMovementLeftMouseButton( eventTimeDelta, dx, dy );
	}
	else if (buttonMask == (GUIEventAdapter::LEFT_MOUSE_BUTTON | GUIEventAdapter::RIGHT_MOUSE_BUTTON))
	{
		Vec3d eyes,centers,ups;
		getTransformation(eyes,centers,ups);
		Vec3d lv( centers - eyes );

		Vec3d f( lv );
		f.normalize();
		Vec3d s( f^ups );
		s.normalize();

		int deltax_ = _ga_t0->getX()-_ga_t1->getX();
		int deltay_ = _ga_t0->getY()-_ga_t1->getY();
		Vec2d slop(deltax_,deltay_);
		slop.normalize();

		osg::Vec3d f1, s1;

		if (Pick(*_ga_t0.get(), _select_node, f1) && Pick(*_ga_t1.get(), _select_node, s1))
		{

			if (fabs(slop.y())>0.7&&fabs(slop.y())<=1)
			{
				f1.z() = 0;
				s1.z() = s1.y()-f1.y();

				f1.y() = 0;
				s1.y() = 0;
				f1.x() = 0;
				s1.x() = 0;
			}
			else if (slop.y()>-0.7&&slop.y()<=0.7)
			{
				f1.z() = 0;
				s1.z() = 0;
			}
			
			_viewMatrix = osg::RefMatrixd::translate( f1 - s1) * _viewMatrix;

			UpdataTheMatrix(_viewMatrix);
		}

		

		
		
	}


	return false;
}

void ModelBimOrbitManipulator::UpdataTheMatrix(osg::Matrixd transmatrix)
{
	osg::Vec3d eyescur,centercur,upcur;
	transmatrix.getLookAt(eyescur,centercur,upcur);
	setTransformation(eyescur,centercur,upcur);
}

// doc in parent
bool ModelBimOrbitManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    // pan model
    float scale = -0.3f * _distance * getThrowScale( eventTimeDelta );
    panModel( dx*scale, dy*scale );
    return true;
}


// doc in parent
bool ModelBimOrbitManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double /*dx*/, const double dy )
{
    // zoom model
    zoomModel( dy * getThrowScale( eventTimeDelta ), true );
    return true;
}


bool ModelBimOrbitManipulator::performMouseDeltaMovement( const float dx, const float dy )
{
    // rotate camera
    if( getVerticalAxisFixed() )
        rotateWithFixedVertical( dx, dy );
    else
        rotateTrackball( 0.f, 0.f, dx, dy, 1.f );

    return true;
}


void ModelBimOrbitManipulator::applyAnimationStep( const double currentProgress, const double prevProgress )
{
    OrbitAnimationData *ad = dynamic_cast< OrbitAnimationData* >( _animationData.get() );
    assert( ad );

    // compute new center
    osg::Vec3d prevCenter, prevEye, prevUp;
    getTransformation( prevEye, prevCenter, prevUp );
    osg::Vec3d newCenter = osg::Vec3d(prevCenter) + (ad->_movement * (currentProgress - prevProgress));

    // fix vertical axis
    if( getVerticalAxisFixed() )
    {

        CoordinateFrame coordinateFrame = getCoordinateFrame( newCenter );
        Vec3d localUp = getUpVector( coordinateFrame );

        fixVerticalAxis( newCenter - prevEye, prevUp, prevUp, localUp, false );
   }

   // apply new transformation
   setTransformation( prevEye, newCenter, prevUp );
}


bool ModelBimOrbitManipulator::startAnimationByMousePointerIntersection(
      const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    // get current transformation
    osg::Vec3d prevCenter, prevEye, prevUp;
    getTransformation( prevEye, prevCenter, prevUp );

    // center by mouse intersection
    if( !setCenterByMousePointerIntersection( ea, us ) )
        return false;

    OrbitAnimationData *ad = dynamic_cast< OrbitAnimationData*>( _animationData.get() );
    assert( ad );

    // setup animation data and restore original transformation
    ad->start( osg::Vec3d(_center) - prevCenter, ea.getTime() );
    setTransformation( prevEye, prevCenter, prevUp );

    return true;
}


void ModelBimOrbitManipulator::OrbitAnimationData::start( const osg::Vec3d& movement, const double startTime )
{
    AnimationData::start( startTime );

    _movement = movement;
}


/** Performs trackball rotation based on two points given, for example,
    by mouse pointer on the screen.

    Scale parameter is useful, for example, when manipulator is thrown.
    It scales the amount of rotation based, for example, on the current frame time.*/
void ModelBimOrbitManipulator::rotateTrackball( const float px0, const float py0,
                                        const float px1, const float py1, const float scale )
{
    osg::Vec3d axis;
    float angle;

    trackball( axis, angle, px0 + (px1-px0)*scale, py0 + (py1-py0)*scale, px0, py0 );

	Vec3d eyes,centers,ups;
	getTransformation(eyes,centers,ups);
	Vec3d lv( centers - eyes );

	Vec3d f( lv );
	f.normalize();
	Vec3d s( f^ups );
	s.normalize();
	
	int deltax_ = _ga_t0->getX()-_ga_t1->getX();
	int deltay_ = _ga_t0->getY()-_ga_t1->getY();
	Vec2d slop(deltax_,deltay_);
	slop.normalize();
	
	if (slop.y()>0.7&&slop.y()<=1)
	{
		axis =s;
		angle = angle*2;
	}
	else if (slop.y()>-0.7&&slop.y()<=0.7&&deltax_>0)
	{
		axis = Vec3d(0,0,-1);
		angle = angle*10;
	}
	else if (slop.y()>=-1&&slop.y()<=-0.7)
	{
		axis =-s;
		angle = angle*2;
	}
	else if (slop.y()>-0.7&&slop.y()<=0.7&&deltax_<0)
	{
		axis = Vec3d(0,0,1);
		angle = angle*10;
	}

    Quat new_rotate;
    new_rotate.makeRotate( angle, axis );

	//setTransformation(_viewMatrix.get);
	_viewMatrix =(osg::Matrixd::translate(-_mouse_push) * osg::Matrixd::rotate( new_rotate.inverse() ) * osg::Matrixd::translate(_mouse_push) )* _viewMatrix;
	UpdataTheMatrix(_viewMatrix);
}


/** Performs rotation horizontally by dx parameter and vertically by dy parameter,
    while keeping UP vector.*/
void ModelBimOrbitManipulator::rotateWithFixedVertical( const float dx, const float dy )
{
    CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
    Vec3d localUp = getUpVector( coordinateFrame );

    rotateYawPitch( _rotation, dx, dy, localUp );
}


/** Performs rotation horizontally by dx parameter and vertically by dy parameter,
    while keeping UP vector given by up parameter.*/
void ModelBimOrbitManipulator::rotateWithFixedVertical( const float dx, const float dy, const Vec3f& up )
{
    rotateYawPitch( _rotation, dx, dy, up );
}


/** Moves camera in x,y,z directions given in camera local coordinates.*/
void ModelBimOrbitManipulator::panModel( const float dx, const float dy, const float dz )
{
    Matrix rotation_matrix;
    rotation_matrix.makeRotate( _rotation );

    Vec3d dv( dx, dy, dz );

    _center += dv * rotation_matrix;

	_viewMatrix = osg::Matrixd::translate( -_center ) *
		osg::Matrixd::rotate( _rotation.inverse() ) *
		osg::Matrixd::translate( 0.0, 0.0, -_distance );
}

bool ModelBimOrbitManipulator::handleMouseMove( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
	osg::Vec3d select_result;

	Pick(ea, _select_node, select_result);

	_mouse_move = select_result;

	return false;
}


/** Changes the distance of camera to the focal center.
    If pushForwardIfNeeded is true and minimumDistance is reached,
    the focal center is moved forward. Otherwise, distance is limited
    to its minimum value.
    \sa ModelBimOrbitManipulator::setMinimumDistance
 */// 从模型视图的角度来实现放大缩小
void ModelBimOrbitManipulator::zoomModel( const float dy, bool pushForwardIfNeeded )
{
    // scale
	float scale = 1.0f + dy;

	_scale_total *= scale;

	_viewMatrix = (osg::Matrixd::translate(-_mouse_move) * osg::Matrixd::scale(osg::Vec3d(scale, scale, scale)) * osg::Matrixd::translate(_mouse_move) ) * _viewMatrix;
	UpdataTheMatrix(_viewMatrix);

	return;

    // minimum distance
    float minDist = _minimumDistance;
    if( getRelativeFlag( _minimumDistanceFlagIndex ) )
        minDist *= _modelSize;

    if( _distance*scale > minDist )
    {
        // regular zoom
        _distance *= scale;
		_viewMatrix = osg::Matrixd::translate( -_center ) *
			osg::Matrixd::rotate( _rotation.inverse() ) *
			osg::Matrixd::translate( 0.0, 0.0, -_distance );
    }
    else
    {
        if( pushForwardIfNeeded )
        {
            // push the camera forward
            float scale = -_distance;
            Matrixd rotation_matrix( _rotation );
            Vec3d dv = (Vec3d( 0.0f, 0.0f, -1.0f ) * rotation_matrix) * (dy * scale);
            _center += dv;
			_viewMatrix = osg::Matrixd::translate( -_center ) *
				osg::Matrixd::rotate( _rotation.inverse() ) *
				osg::Matrixd::translate( 0.0, 0.0, -_distance );
        }
        else
        {
            // set distance on its minimum value
            _distance = minDist;
			_viewMatrix = osg::Matrixd::translate( -_center ) *
				osg::Matrixd::rotate( _rotation.inverse() ) *
				osg::Matrixd::translate( 0.0, 0.0, -_distance );
        }
    }
}


/**
 * Simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
void ModelBimOrbitManipulator::trackball( osg::Vec3d& axis, float& angle, float p1x, float p1y, float p2x, float p2y )
{
    /*
        * First, figure out z-coordinates for projection of P1 and P2 to
        * deformed sphere
        */

    osg::Matrixd rotation_matrix(_rotation);

    osg::Vec3d uv = Vec3d(0.0f,1.0f,0.0f)*rotation_matrix;
    osg::Vec3d sv = Vec3d(1.0f,0.0f,0.0f)*rotation_matrix;
    osg::Vec3d lv = Vec3d(0.0f,0.0f,-1.0f)*rotation_matrix;

    osg::Vec3d p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere(_trackballSize, p1x, p1y);
    osg::Vec3d p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere(_trackballSize, p2x, p2y);

    /*
        *  Now, we want the cross product of P1 and P2
        */
    axis = p2^p1;
    axis.normalize();

    /*
        *  Figure out how much to rotate around that axis.
        */
    float t = (p2 - p1).length() / (2.0 * _trackballSize);

    /*
        * Avoid problems with out-of-control values...
        */
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    angle = inRadians(asin(t));
}


/**
 * Helper trackball method that projects an x,y pair onto a sphere of radius r OR
 * a hyperbolic sheet if we are away from the center of the sphere.
 */
float ModelBimOrbitManipulator::tb_project_to_sphere( float r, float x, float y )
{
    float d, t, z;

    d = sqrt(x*x + y*y);
                                 /* Inside sphere */
    if (d < r * 0.70710678118654752440)
    {
        z = sqrt(r*r - d*d);
    }                            /* On hyperbola */
    else
    {
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
}


/** Get the FusionDistanceMode. Used by SceneView for setting up stereo convergence.*/
osgUtil::SceneView::FusionDistanceMode ModelBimOrbitManipulator::getFusionDistanceMode() const
{
    return osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE;
}

/** Get the FusionDistanceValue. Used by SceneView for setting up stereo convergence.*/
float ModelBimOrbitManipulator::getFusionDistanceValue() const
{
    return _distance;
}


/** Set the center of the manipulator. */
void ModelBimOrbitManipulator::setCenter( const Vec3d& center )
{
    _center = center;
	_viewMatrix = osg::Matrixd::translate( -_center ) *
		osg::Matrixd::rotate( _rotation.inverse() ) *
		osg::Matrixd::translate( 0.0, 0.0, -_distance );
}


/** Get the center of the manipulator. */
const Vec3d& ModelBimOrbitManipulator::getCenter() const
{
    return _center;
}


/** Set the rotation of the manipulator. */
void ModelBimOrbitManipulator::setRotation( const Quat& rotation )
{
    _rotation = rotation;
	_viewMatrix = osg::Matrixd::translate( -_center ) *
		osg::Matrixd::rotate( _rotation.inverse() ) *
		osg::Matrixd::translate( 0.0, 0.0, -_distance );
}


/** Get the rotation of the manipulator. */
const Quat& ModelBimOrbitManipulator::getRotation() const
{
    return _rotation;
}


/** Set the distance of camera to the center. */
void ModelBimOrbitManipulator::setDistance( double distance )
{
    _distance = distance;
	_viewMatrix = osg::Matrixd::translate( -_center ) *
		osg::Matrixd::rotate( _rotation.inverse() ) *
		osg::Matrixd::translate( 0.0, 0.0, -_distance );
}


/** Get the distance of the camera to the center. */
double ModelBimOrbitManipulator::getDistance() const
{
    return _distance;
}


/** Set the size of the trackball. Value is relative to the model size. */
void ModelBimOrbitManipulator::setTrackballSize( const double& size )
{
    /*
    * This size should really be based on the distance from the center of
    * rotation to the point on the object underneath the mouse.  That
    * point would then track the mouse as closely as possible.  This is a
    * simple example, though, so that is left as an Exercise for the
    * Programmer.
    */
    _trackballSize = size;
    clampBetweenRange( _trackballSize, 0.1, 1.0, "TrackballManipulator::setTrackballSize(float)" );
}


/** Set the mouse wheel zoom factor.
    The amount of camera movement on each mouse wheel event
    is computed as the current distance to the center multiplied by this factor.
    For example, value of 0.1 will short distance to center by 10% on each wheel up event.
    Use negative value for reverse mouse wheel direction.*/
void ModelBimOrbitManipulator::setWheelZoomFactor( double wheelZoomFactor )
{
    _wheelZoomFactor = wheelZoomFactor;
}


/** Set the minimum distance of the eye point from the center
    before the center is pushed forward.*/
void ModelBimOrbitManipulator::setMinimumDistance( const double& minimumDistance, bool relativeToModelSize )
{
    _minimumDistance = minimumDistance;
    setRelativeFlag( _minimumDistanceFlagIndex, relativeToModelSize );
}


/** Get the minimum distance of the eye point from the center
    before the center is pushed forward.*/
double ModelBimOrbitManipulator::getMinimumDistance( bool *relativeToModelSize ) const
{
    if( relativeToModelSize )
        *relativeToModelSize = getRelativeFlag( _minimumDistanceFlagIndex );

    return _minimumDistance;
}
