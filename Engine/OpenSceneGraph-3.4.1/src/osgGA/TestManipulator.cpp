
#include <osgGA/TestManipulator>
#include <osg/BoundsChecking>
#include <cassert>
#include <iostream>

using namespace osg;
using namespace osgGA;



int TestManipulator::_minimumDistanceFlagIndex = allocateRelativeFlag();


/// Constructor.
TestManipulator::TestManipulator( int flags )
   : inherited( flags ),
     _distance( 1. ),
     _trackballSize( 0.8 )
{
    setMinimumDistance( 0.05, true );
    setWheelZoomFactor( 0.1 );
    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
        setAnimationTime( 0.2 );
}


/// Constructor.
TestManipulator::TestManipulator( const TestManipulator& om, const CopyOp& copyOp )
	: osg::Callback(om, copyOp),
	inherited( om, copyOp ),
	_center( om._center ),
	_rotation( om._rotation ),
	_distance( om._distance ),
	_trackballSize( om._trackballSize ),
	_wheelZoomFactor( om._wheelZoomFactor ),
	_minimumDistance( om._minimumDistance )
{
	
}


/** Set the position of the manipulator using a 4x4 matrix.*/
void TestManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    _center = osg::Vec3d( 0., 0., -_distance ) * matrix;
    _rotation = matrix.getRotate();

    // fix current rotation
    if( getVerticalAxisFixed() )
        fixVerticalAxis( _center, _rotation, true );
}


/** Set the position of the manipulator using a 4x4 matrix.*/
void TestManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
    setByMatrix( osg::Matrixd::inverse( matrix ) );
}


/** Get the position of the manipulator as 4x4 matrix.*/
osg::Matrixd TestManipulator::getMatrix() const
{
	return osg::Matrixd::translate( 0., 0., _distance ) *
		osg::Matrixd::rotate( _rotation ) *
		osg::Matrixd::translate( _center );
}


/** Get the position of the manipulator as a inverse matrix of the manipulator,
    typically used as a model view matrix.*/
osg::Matrixd TestManipulator::getInverseMatrix() const
{
	return osg::Matrixd::translate( -_center ) *
		osg::Matrixd::rotate( _rotation.inverse() ) *
		osg::Matrixd::translate( 0.0, 0.0, -_distance );
}


// doc in parent
void TestManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
{
    _center = eye + rotation * osg::Vec3d( 0., 0., -_distance );
    _rotation = rotation;

    // fix current rotation
    if( getVerticalAxisFixed() )
        fixVerticalAxis( _center, _rotation, true );
}


// doc in parent
void TestManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const
{
    eye = _center - _rotation * osg::Vec3d( 0., 0., -_distance );
    rotation = _rotation;
}


// doc in parent
void TestManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
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
}


// doc in parent
void TestManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const
{
    center = _center;
    eye = _center + _rotation * osg::Vec3d( 0., 0., _distance );
    up = _rotation * osg::Vec3d( 0., 1., 0. );
}


/** Sets the transformation by heading. Heading is given as an angle in radians giving a azimuth in xy plane.
    Its meaning is similar to longitude used in cartography and navigation.
	Positive number is going to the east direction.*/
void TestManipulator::setHeading( double azimuth )
{
	CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
	Vec3d localUp = getUpVector( coordinateFrame );
	Vec3d localRight = getSideVector( coordinateFrame );

	Vec3d dir = Quat( getElevation(), localRight ) * Quat( azimuth, localUp ) * Vec3d( 0., -_distance, 0. );

	setTransformation( _center + dir, _center, localUp );
}


/// Returns the heading in radians. \sa setHeading
double TestManipulator::getHeading() const
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
void TestManipulator::setElevation( double elevation )
{
    CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
    Vec3d localUp = getUpVector( coordinateFrame );
    Vec3d localRight = getSideVector( coordinateFrame );

    Vec3d dir = Quat( -elevation, localRight ) * Quat( getHeading(), localUp ) * Vec3d( 0., -_distance, 0. );

    setTransformation( _center + dir, _center, localUp );
}


/// Returns the elevation in radians. \sa setElevation
double TestManipulator::getElevation() const
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

/** Handles events. Returns true if handled, false otherwise.*/
bool TestManipulator::handle( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
	switch( ea.getEventType() )
	{

	case GUIEventAdapter::FRAME:
		return handleFrame( ea, us );

	case GUIEventAdapter::RESIZE:
		return handleResize( ea, us );

	default:
		break;
	}

	if( ea.getHandled() )
		return false;

	switch( ea.getEventType() )
	{
	case GUIEventAdapter::MOVE:
		return handleMouseMove( ea, us );

	case GUIEventAdapter::DRAG:
		return handleMouseDrag( ea, us );

	case GUIEventAdapter::PUSH:
		return handleMousePush( ea, us );

	case GUIEventAdapter::RELEASE:
		return handleMouseRelease( ea, us );

	case GUIEventAdapter::KEYDOWN:
		return handleKeyDown( ea, us );

	case GUIEventAdapter::KEYUP:
		return handleKeyUp( ea, us );

	case GUIEventAdapter::SCROLL:
		if( _flags & PROCESS_MOUSE_WHEEL )
			return handleMouseWheel( ea, us );
		else
			return false;
	case GUIEventAdapter::DOUBLECLICK:
		return handleDoubleClick( ea, us );
	default:
		return false;
	}
}


osg::Vec3d world2Camera(osg::Vec3 worldPoint,Camera *camera)//世界转相机
{
	osg::Vec3d vec3;

	osg::Matrix mV = camera->getViewMatrix();
	vec3 = worldPoint * mV;
	return vec3;
}
osg::Vec3d camera2World(osg::Vec3 cameraPoint,Camera *camera)//相机转世界
{
	osg::Vec3d vec3;

	//osg::Vec3d vScreen(x,y, 0);
	osg::Matrix mV = camera->getViewMatrix();
	osg::Matrix invertmV;
	invertmV.invert(mV);
	vec3 = cameraPoint * invertmV ;
	return vec3;
}

osg::Vec3d screen2Camera(osg::Vec3 screenPoint,Camera *camera)//屏幕转相机
{

	osg::Vec3d vec3;
	osg::Matrix mPW = camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	osg::Matrix invertmPW;
	invertmPW.invert(mPW);
	vec3 = screenPoint * invertmPW;
	return vec3;
}
osg::Vec3d camera2Screen(osg::Vec3 cameraPoint,Camera *camera)//相机转屏幕
{
	osg::Vec3d vec3;

	//osg::Vec3d vScreen(x,y, 0);
	osg::Matrix mPW = camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	vec3 = cameraPoint * mPW;
	return vec3;
}

osg::Vec3d screen2World(osg::Vec3 screenPoint,Camera *camera)//将屏幕坐标转换到世界坐标
{
	osg::Vec3d vec3;

	//osg::Vec3d vScreen(x,y, 0);
	osg::Matrix mVPW = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	osg::Matrix invertVPW;
	invertVPW.invert(mVPW);
	vec3 = screenPoint * invertVPW;
	return vec3;
}
osg::Vec3d world2Screen(osg::Vec3 worldPoint,Camera *camera)//世界到屏幕
{
	osg::Vec3d vec3;

	osg::Matrix mVPW = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	vec3 = worldPoint * mVPW;
	return vec3;
}

// doc in parent
bool TestManipulator::handleDoubleClick( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
	//DoubleClickModel( -_wheelZoomFactor, true );

	osg::View* view = us.asView();
	if( !view )
		return false;

	Camera *camera = view->getCamera();
	if( !camera )
		return false;


	osg::Vec3f screenCen3D = world2Screen(_center,camera);

	osg::Vec3f cameraCen3D = screen2Camera(screenCen3D,camera);


	osg::Vec3f vScreen(ea.getX(),ea.getY(), 1.0f);

	//osg::Vec3f camera3D = screen2Camera(vScreen,camera);



	

	std::cout<<std::endl;

	std::cout<<"distance is : "<<_distance<<std::endl;

	//std::cout<<"mouse X is : "<<ea.getX()<<std::endl;

	//std::cout<<"mouse Y is : "<<ea.getY()<<std::endl;

	std::cout<<"center screen X is : "<<screenCen3D.x()<<std::endl;

	std::cout<<" center screen  Y is : "<<screenCen3D.y()<<std::endl;

	std::cout<<" center screen  z is : "<<screenCen3D.z()<<std::endl;


	//std::cout<<"mouse camera X is : "<<camera3D.x()<<std::endl;

	//std::cout<<"mouse camera Y is : "<<camera3D.y()<<std::endl;

	//std::cout<<"mouse camera z is : "<<camera3D.z()<<std::endl;


	float Xmax = ea.getXmax() - ea.getXmin();

	float Ymax = ea.getYmax() - ea.getYmin();



	float detalX =   (ea.getX()/Xmax - screenCen3D.x()/Xmax)*2.0f;

	float detalY =   (ea.getY()/Ymax - screenCen3D.y()/Ymax)*2.0f;
	


	//osg::Vec3f centerCamera(ea.getX(),ea.getY(),screenCen3D.z());

	osg::Vec3f centerworld = screen2World(vScreen,camera);



	osg::Vec3f centerTemp(centerworld.x(),centerworld.y(),1000.0f);

	//_center = centerTemp;

	std::cout<<"center X is : "<<_center.x()<<std::endl;

	std::cout<<"center Y is : "<<_center.y()<<std::endl;

	std::cout<<"center Z is : "<<_center.z()<<std::endl;


	//float scale =  _distance;

	//panModel( detalX*scale, detalY*scale );

	//setHeading(PI);

	rotateYawPitch(_rotation,PI,PI);


	//Matrixd rotation_matrix( _rotation );
	//Vec3d dv = (Vec3d( 0.0f, 0.0f, -1.0f ) * rotation_matrix) * (0.1f * _distance);
	//_center += dv;



	us.requestRedraw();
	us.requestContinuousUpdate( isAnimating() || _thrown );

	return true;
}

 void TestManipulator::setViewpointX()
 {
	 rotateYawPitch(_rotation,PI,PI);
 }
 void TestManipulator::setViewpointY()
 {
	 rotateYawPitch(_rotation,PI,0);
 }
 void TestManipulator::setViewpointZ()
 {
	 rotateYawPitch(_rotation,0,PI);
 }

 void TestManipulator::setHomeView()
 {

 }

// doc in parent
bool TestManipulator::handleMouseWheel( const GUIEventAdapter& ea, GUIActionAdapter& us )
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
			zoomModel( _wheelZoomFactor, true );
			us.requestRedraw();
			us.requestContinuousUpdate( isAnimating() || _thrown );
			return true;
        }

        // mouse scroll down event
        case GUIEventAdapter::SCROLL_DOWN:
        {
            // perform zoom
			zoomModel( -_wheelZoomFactor, true );
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
bool TestManipulator::performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    // rotate camera
    if( getVerticalAxisFixed() )
        rotateWithFixedVertical( dx, dy );
    else
        rotateTrackball( _ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
                         _ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
                         getThrowScale( eventTimeDelta ) );
    return true;
}


// doc in parent
bool TestManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    // pan model
    float scale = -0.3f * _distance * getThrowScale( eventTimeDelta );
    panModel( dx*scale, dy*scale );
    return true;
}


// doc in parent
bool TestManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double /*dx*/, const double dy )
{
    // zoom model
    zoomModel( dy * getThrowScale( eventTimeDelta ), true );
    return true;
}


bool TestManipulator::performMouseDeltaMovement( const float dx, const float dy )
{
    // rotate camera
	if( getVerticalAxisFixed() )
		rotateWithFixedVertical( dx, dy );
	else
		rotateTrackball( 0.f, 0.f, dx, dy, 1.f );

    return true;
}


void TestManipulator::applyAnimationStep( const double currentProgress, const double prevProgress )
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


bool TestManipulator::startAnimationByMousePointerIntersection(
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


void TestManipulator::OrbitAnimationData::start( const osg::Vec3d& movement, const double startTime )
{
    AnimationData::start( startTime );

    _movement = movement;
}


/** Performs trackball rotation based on two points given, for example,
by mouse pointer on the screen.

Scale parameter is useful, for example, when manipulator is thrown.
It scales the amount of rotation based, for example, on the current frame time.*/
void TestManipulator::rotateTrackball( const float px0, const float py0,
                                        const float px1, const float py1, const float scale )
{
    osg::Vec3d axis;
    float angle;

    trackball( axis, angle, px0 + (px1-px0)*scale, py0 + (py1-py0)*scale, px0, py0 );

    Quat new_rotate;
    new_rotate.makeRotate( angle, axis );

    _rotation = _rotation * new_rotate;
}


/** Performs rotation horizontally by dx parameter and vertically by dy parameter,
    while keeping UP vector.*/
void TestManipulator::rotateWithFixedVertical( const float dx, const float dy )
{
    CoordinateFrame coordinateFrame = getCoordinateFrame( _center );
    Vec3d localUp = getUpVector( coordinateFrame );

    rotateYawPitch( _rotation, dx, dy, localUp );
}


/** Performs rotation horizontally by dx parameter and vertically by dy parameter,
    while keeping UP vector given by up parameter.*/
void TestManipulator::rotateWithFixedVertical( const float dx, const float dy, const Vec3f& up )
{
    rotateYawPitch( _rotation, dx, dy, up );
}


/** Moves camera in x,y,z directions given in camera local coordinates.*/
void TestManipulator::panModel( const float dx, const float dy, const float dz )
{
	Matrix rotation_matrix;
	rotation_matrix.makeRotate( _rotation );

	Vec3d dv( dx, dy, dz );

	_center += dv * rotation_matrix;
}


/** Changes the distance of camera to the focal center.
    If pushForwardIfNeeded is true and minimumDistance is reached,
    the focal center is moved forward. Otherwise, distance is limited
    to its minimum value.
    \sa OrbitManipulator::setMinimumDistance
 */
void TestManipulator::zoomModel( const float dy, bool pushForwardIfNeeded )
{
    // scale
    float scale = 1.0f + dy;

    // minimum distance
    float minDist = _minimumDistance;
    if( getRelativeFlag( _minimumDistanceFlagIndex ) )
        minDist *= _modelSize;

    if( _distance*scale > minDist )
    {
        // regular zoom
        _distance *= scale;
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
        }
        else
        {
            // set distance on its minimum value
            _distance = minDist;
		
        }
    }
}



void TestManipulator::DoubleClickModel( const float dy, bool pushForwardIfNeeded )
{
	// scale
	float scale = 1.0f + dy;

	// minimum distance
	float minDist = _minimumDistance;
	if( getRelativeFlag( _minimumDistanceFlagIndex ) )
		minDist *= _modelSize;

	if( _distance*scale > minDist )
	{
		// regular zoom
		_distance *= scale;
	}
	else
	{
		if( pushForwardIfNeeded )
		{
			// push the camera forward
			float scale = -_distance;
			Matrixd rotation_matrix( _rotation );

			Vec3d dv = (Vec3d( 0.0f, 0.0f, -1.0f ) * rotation_matrix) * (dy * scale);

			Vec3d dv2 = (Vec3d( 0.0f, 1.0f, 1.0f ) * rotation_matrix);

			_center += dv;
		}
		else
		{
			// set distance on its minimum value
			_distance = minDist;
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
void TestManipulator::trackball( osg::Vec3d& axis, float& angle, float p1x, float p1y, float p2x, float p2y )
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
float TestManipulator::tb_project_to_sphere( float r, float x, float y )
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
osgUtil::SceneView::FusionDistanceMode TestManipulator::getFusionDistanceMode() const
{
    return osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE;
}

/** Get the FusionDistanceValue. Used by SceneView for setting up stereo convergence.*/
float TestManipulator::getFusionDistanceValue() const
{
    return _distance;
}


/** Set the center of the manipulator. */
void TestManipulator::setCenter( const Vec3d& center )
{
    _center = center;
}


/** Get the center of the manipulator. */
const Vec3d& TestManipulator::getCenter() const
{
    return _center;
}


/** Set the rotation of the manipulator. */
void TestManipulator::setRotation( const Quat& rotation )
{
    _rotation = rotation;
}


/** Get the rotation of the manipulator. */
const Quat& TestManipulator::getRotation() const
{
    return _rotation;
}


/** Set the distance of camera to the center. */
void TestManipulator::setDistance( double distance )
{
    _distance = distance;
}


/** Get the distance of the camera to the center. */
double TestManipulator::getDistance() const
{
    return _distance;
}


/** Set the size of the trackball. Value is relative to the model size. */
void TestManipulator::setTrackballSize( const double& size )
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
void TestManipulator::setWheelZoomFactor( double wheelZoomFactor )
{
    _wheelZoomFactor = wheelZoomFactor;
}


/** Set the minimum distance of the eye point from the center
    before the center is pushed forward.*/
void TestManipulator::setMinimumDistance( const double& minimumDistance, bool relativeToModelSize )
{
    _minimumDistance = minimumDistance;
    setRelativeFlag( _minimumDistanceFlagIndex, relativeToModelSize );
}


/** Get the minimum distance of the eye point from the center
    before the center is pushed forward.*/
double TestManipulator::getMinimumDistance( bool *relativeToModelSize ) const
{
    if( relativeToModelSize )
        *relativeToModelSize = getRelativeFlag( _minimumDistanceFlagIndex );

    return _minimumDistance;
}
