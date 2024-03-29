/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2016 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <osgEarthUtil/GeodeticGraticule>
#include <osgEarthUtil/Shaders>

#include <osgEarth/Registry>
#include <osgEarth/NodeUtils>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/Terrain>

#include <osg/Depth>
#include <osg/ClipNode>
#include <osg/ClipPlane>

#define LC "[GeodeticGraticule] "

#define OE_TEST OE_NULL

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Symbology;

REGISTER_OSGEARTH_LAYER(geodetic_graticule, GeodeticGraticule);

namespace
{
    // Helper class to find a MapNode and set it in the graticule 
    // so it can install the terrain effect.
    struct MyGroup : public osg::Group
    {
        GeodeticGraticule* _grat;
        osg::ref_ptr<MapNode>& _mapNode;

        MyGroup(GeodeticGraticule* grat, osg::ref_ptr<MapNode>& mapNode)
            : _grat(grat), _mapNode(mapNode)
        {
            // Require an update traversal to udpate the labels.
            setNumChildrenRequiringUpdateTraversal(1);
        }

        void traverse(osg::NodeVisitor& nv)
        {
            if (nv.getVisitorType() == nv.UPDATE_VISITOR)
            {
                if (_mapNode.valid() == false)
                {
                    _mapNode = osgEarth::findInNodePath<MapNode>(nv);
                    if (_mapNode.valid() == true)
                    {
                        _grat->dirty();
                    }
                }
                _grat->updateLabels();
            }

            else if (nv.getVisitorType() == nv.CULL_VISITOR)
            {
                _grat->cull(static_cast<osgUtil::CullVisitor*>(&nv));
            }

            osg::Group::traverse(nv);
        }
    };

    // Cull callback installed on the terrain that applies
    // a stateset for the proper camera.
    struct GraticuleTerrainCallback : public osg::NodeCallback
    {
        osg::observer_ptr<GeodeticGraticule> _g;
        GraticuleTerrainCallback(GeodeticGraticule* g) : _g(g) { }
        void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            bool traversed = false;
            osg::ref_ptr<GeodeticGraticule> grat;
            if (_g.lock(grat))
            {
                osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
                if (cv)
                {
                    osg::StateSet* stateset = grat->getStateSet(cv);
                    if (stateset)
                        cv->pushStateSet(stateset);

                    traverse(node, nv);
                    traversed = true;

                    if (stateset)
                        cv->popStateSet();
                }
            }

            if (!traversed)
                traverse(node, nv);
        }
    };

    const char* textFadeFS =
        "#version 330\n"
        "uniform mat4 osg_ViewMatrixInverse;\n"
        "void oe_GeodeticGraticule_text_frag(inout vec4 color) { \n"
        "    const float maxHAE = 4000.0;\n"
        "    vec3 eye = osg_ViewMatrixInverse[3].xyz;\n"
        "    float hae = length(eye) - 6378137.0;\n"
        "    float alpha = clamp(hae/maxHAE, 0.0, 1.0); \n"
        "    color.a *= alpha;\n"
        "}\n";
}


GeodeticGraticule::GeodeticGraticule() :
VisibleLayer(&_optionsConcrete),
_options(&_optionsConcrete)
{
    init();
}

GeodeticGraticule::GeodeticGraticule(const GeodeticGraticuleOptions& options) :
VisibleLayer(&_optionsConcrete),
_options(&_optionsConcrete),
_optionsConcrete(options)
{
    init();
}

void
GeodeticGraticule::dirty()
{
    rebuild();
}

void
GeodeticGraticule::init()
{
    VisibleLayer::init();

    _defaultResolution = 10.0/180.0;

    // make the shared depth attr:
    this->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, 0);

    // Read the resolutions from the config.
    if (options().resolutions().isSet())
    {
        StringTokenizer tok(" ");
        StringVector tokens;
        tok.tokenize(*options().resolutions(), tokens);
        for (unsigned int i = 0; i < tokens.size(); i++)
        {
            double r = as<double>(tokens[i], -1.0);
            if (r > 0) 
            {
                _resolutions.push_back( r );
            }
        }
    }

    if (_resolutions.empty())
    {
        // Initialize the resolutions
        _resolutions.push_back( 10.0 );
        _resolutions.push_back( 5.0 );
        _resolutions.push_back( 2.5 );
        _resolutions.push_back( 1.0 );
        _resolutions.push_back( 0.5 );
        _resolutions.push_back( 0.25 );
        _resolutions.push_back( 0.125 );
        _resolutions.push_back( 0.0625 );
        _resolutions.push_back( 0.03125 );

    }

    // Divide all the resolutions by 180 so they match up with the terrain effects concept of resolutions
    for (unsigned int i = 0; i < _resolutions.size(); i++)
    {
        _resolutions[i] /= 180.0;
    }

    // Initialize the formatter
    _formatter = new LatLongFormatter(osgEarth::Util::LatLongFormatter::FORMAT_DEGREES_MINUTES_SECONDS_TERSE, LatLongFormatter::USE_SYMBOLS |LatLongFormatter::USE_PREFIXES);
}

void
GeodeticGraticule::addedToMap(const Map* map)
{
    _map = map;
    rebuild();
}

void
GeodeticGraticule::removedFromMap(const Map* map)
{
    if (_mapNode.valid())
    {
        removeEffect();

        if (_callback.valid())
        {
            _mapNode->getTerrainEngine()->removeCullCallback(_callback.get());
            _callback = 0L;
        }
    }
    _map = 0L;
}

osg::Node*
GeodeticGraticule::getOrCreateNode()
{
    if (_root.valid() == false)
    {
        _root = new MyGroup(this, _mapNode);
        rebuild();
    }

    return _root.get();
}

void
GeodeticGraticule::setVisible(bool value)
{
    VisibleLayer::setVisible(value);

    if (getVisible())
        installEffect();
    else
        removeEffect();
}

void
GeodeticGraticule::rebuild()
{
    // clear everything out
    if (!_root.valid())
        return;

    // we must have the map node, if not, wait
    if (_mapNode.valid() == false)
        return;

    // also need a map; if not, wait
    osg::ref_ptr<const Map> map;
    if (!_map.lock(map))
        return;

    // start from scratch
    _root->removeChildren( 0, _root->getNumChildren() );

    // requires a geocentric map
    if ( !map->isGeocentric() )
    {
        OE_WARN << LC << "Projected map mode is not yet supported" << std::endl;
        return;
    }

    if (!_callback.valid())
    {
        _callback = new GraticuleTerrainCallback(this);
        _mapNode->getTerrainEngine()->addCullCallback(_callback.get());
    }

    setVisible(getVisible());
}

#define RESOLUTION_UNIFORM "oe_GeodeticGraticule_resolution"
#define COLOR_UNIFORM "oe_GeodeticGraticule_color"
#define WIDTH_UNIFORM "oe_GeodeticGraticule_lineWidth"

void
GeodeticGraticule::installEffect()
{
    if (_mapNode.valid() == false)
        return;

    // shader components
    osg::StateSet* stateset = _mapNode->getTerrainEngine()->getSurfaceStateSet();
    VirtualProgram* vp = VirtualProgram::getOrCreate(stateset);

    // configure shaders
    Shaders package;
    package.load(vp, package.Graticule_Vertex);
    package.load(vp, package.Graticule_Fragment);
    
    stateset->addUniform(new osg::Uniform(COLOR_UNIFORM, options().color().get()));
    stateset->addUniform(new osg::Uniform(WIDTH_UNIFORM, options().lineWidth().get()));
}

void
GeodeticGraticule::removeEffect()
{
    if (_mapNode.valid() == false)
        return;

    osg::StateSet* stateset = _mapNode->getTerrainEngine()->getSurfaceStateSet();
    if ( stateset )
    {
        VirtualProgram* vp = VirtualProgram::get(stateset);
        if ( vp )
        {
            Shaders package;
            package.unload( vp, package.Graticule_Vertex );
            package.unload( vp, package.Graticule_Fragment );

            stateset->removeUniform( COLOR_UNIFORM );
            stateset->removeUniform( WIDTH_UNIFORM );
        }
    }
}

void
GeodeticGraticule::cull(osgUtil::CullVisitor* cv)
{
    osg::Matrixd viewMatrix = *cv->getModelViewMatrix();

    osg::Vec3d vp = cv->getViewPoint();

    CameraData& cdata = getCameraData(cv->getCurrentCamera());

    // Only update if the view matrix has changed.
    if (viewMatrix != cdata._lastViewMatrix && _mapNode.valid())
    {
        GeoPoint eyeGeo;
        eyeGeo.fromWorld(_mapNode->getMapSRS(), vp);
        cdata._lon = eyeGeo.x();
        cdata._lat = eyeGeo.y();

        osg::Viewport* viewport = cv->getViewport();

        float centerX = viewport->x() + viewport->width() / 2.0;
        float centerY = viewport->y() + viewport->height() / 2.0;

        float offsetCenterX = centerX;
        float offsetCenterY = centerY;

        bool hitValid = false;

        // Try the center of the screen.
        osg::Vec3d focalPoint;
        if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(cv->getCurrentCamera()->getView(), centerX, centerY, focalPoint))
        {
            hitValid = true;
        }

        if (hitValid)
        {
            GeoPoint focalGeo;
            focalGeo.fromWorld(_mapNode->getMapSRS(), focalPoint);
            cdata._lon = focalGeo.x();
            cdata._lat = focalGeo.y();
            // We only store the previous view matrix if we actually got a hit.  Otherwise we still need to update.
            cdata._lastViewMatrix = viewMatrix;
        }

        // Get the view extent.
        cdata._viewExtent = getViewExtent(cv);

        double targetResolution = (cdata._viewExtent.height() / 180.0) / options().gridLines().get();

        double resolution = _resolutions[0];
        for (unsigned int i = 0; i < _resolutions.size(); i++)
        {
            resolution = _resolutions[i];
            if (resolution <= targetResolution)
            {
                break;
            }
        }

        // Trippy
        //resolution = targetResolution;

        // Try to compute an approximate meters to pixel value at this view.
        double dist = osg::clampAbove(eyeGeo.z(), 1.0);
        double halfWidth;

        const osg::Matrix& proj = *cv->getProjectionMatrix();
        bool isOrtho = osg::equivalent(proj(3,3), 1.0);

        if (isOrtho)
        {
            double L, R, B, T, N, F;
            proj.getOrtho(L, R, B, T, N, F);
            halfWidth = 0.5*(R-L);
        }
        else // perspective
        {
            double fovy, aspectRatio, zNear, zFar;
            cv->getProjectionMatrix()->getPerspective(fovy, aspectRatio, zNear, zFar);
            halfWidth = osg::absolute(tan(osg::DegreesToRadians(fovy / 2.0)) * dist);
        }

        cdata._metersPerPixel = (2.0 * halfWidth) / (double)viewport->height();

        if (cdata._resolution != resolution)
        {
            cdata._resolution = (float)resolution;
            cdata._resolutionUniform->set(cdata._resolution);
        }

        OE_TEST << "EW=" << cdata._viewExtent.width() << ", ortho=" << isOrtho << ", hW=" << halfWidth << ", res=" << resolution << ", mPP=" << cdata._metersPerPixel << std::endl;
    }

    // traverse the label pool for this camera.
    cv->pushStateSet(cdata._labelStateset.get());

    for (std::vector< osg::ref_ptr< LabelNode > >::iterator i = cdata._labelPool.begin();
        i != cdata._labelPool.end();
        ++i)
    {
        i->get()->accept(*cv);
    }

    cv->popStateSet();
}

GeoExtent
GeodeticGraticule::getViewExtent(osgUtil::CullVisitor* cullVisitor) const
{
    // Get the corners of all points on the view frustum.  Mostly modified from osgthirdpersonview
    osg::Matrixd proj = *cullVisitor->getProjectionMatrix();
    osg::Matrixd mv = *cullVisitor->getModelViewMatrix();
    osg::Matrixd invmv = osg::Matrixd::inverse( mv );

    // clamp the projection far plane so it's not on the other 
    // side of the globe
    osg::Vec3d eye = osg::Vec3d(0,0,0) * invmv;

    const osgEarth::SpatialReference* srs = osgEarth::SpatialReference::create("epsg:4326");

    double nearPlane, farPlane;
    double nLeft, nRight, nTop, nBottom;
    double fLeft, fRight, fTop, fBottom;
    
    if (osg::equivalent(proj(3,3), 1.0)) // ORTHOGRAPHIC
    {
        proj.getOrtho(nLeft, nRight, nBottom, nTop, nearPlane, farPlane);

        fLeft = nLeft;
        fRight = nRight;
        fBottom = nBottom;
        fTop = nTop;

        // In an ortho projection the near plane can be negative;
        // That will disrupt our extent calculation, so we want to clamp
        // it to be between the eyepoint and the far plane.
        nearPlane = osg::clampBetween(nearPlane, 0.0, farPlane);
        farPlane = osg::clampBetween(farPlane, 1.0, eye.length() - srs->getEllipsoid()->getRadiusPolar());
    }
    else
    {
        double f, a, zn, zf;
        proj.getPerspective(f,a,zn,zf);
        zf = std::min(zf, eye.length()-1000.0);
        proj.makePerspective(f, a, zn, zf);
       
        nearPlane = proj(3,2) / (proj(2,2)-1.0);
        farPlane = proj(3,2) / (1.0+proj(2,2));

        // Get the sides of the near plane.
        nLeft = nearPlane * (proj(2,0)-1.0) / proj(0,0);
        nRight = nearPlane * (1.0+proj(2,0)) / proj(0,0);
        nTop = nearPlane * (1.0+proj(2,1)) / proj(1,1);
        nBottom = nearPlane * (proj(2,1)-1.0) / proj(1,1);

        // Get the sides of the far plane.
        fLeft = farPlane * (proj(2,0)-1.0) / proj(0,0);
        fRight = farPlane * (1.0+proj(2,0)) / proj(0,0);
        fTop = farPlane * (1.0+proj(2,1)) / proj(1,1);
        fBottom = farPlane * (proj(2,1)-1.0) / proj(1,1);
    }

    double dist = farPlane - nearPlane;

    std::vector< osg::Vec3d > verts;
    verts.reserve(9);

    // Include origin?
    //verts.push_back(osg::Vec3d(0., 0., 0. ));
    verts.push_back(osg::Vec3d( nLeft, nBottom, -nearPlane ));
    verts.push_back(osg::Vec3d( nRight, nBottom, -nearPlane ));
    verts.push_back(osg::Vec3d( nRight, nTop, -nearPlane ));
    verts.push_back(osg::Vec3d( nLeft, nTop, -nearPlane ));
    verts.push_back(osg::Vec3d( fLeft, fBottom, -farPlane ));
    verts.push_back(osg::Vec3d( fRight, fBottom, -farPlane ));
    verts.push_back(osg::Vec3d( fRight, fTop, -farPlane ));
    verts.push_back(osg::Vec3d( fLeft, fTop, -farPlane ));

    // Compute the bounding sphere of the frustum.
    osg::BoundingSphered bs;
    for (unsigned int i = 0; i < verts.size(); i++)
    {
        osg::Vec3d world = verts[i] * invmv;
        bs.expandBy( world );
    }

    // Get the center of the bounding sphere
    osgEarth::GeoPoint center;
    center.fromWorld(srs, bs.center());

    double radiusDegrees = bs.radius() / 111000.0;
    
    // Try to clamp the maximum radius so far out views don't go wacky.
    radiusDegrees = osg::minimum(radiusDegrees, 90.0);

    double minLon = center.x() - radiusDegrees;
    double minLat = osg::clampAbove(center.y() - radiusDegrees, -90.0);
    double maxLon = center.x() + radiusDegrees;
    double maxLat = osg::clampBelow(center.y() + radiusDegrees, 90.0);

    osgEarth::GeoExtent extent(srs, minLon, minLat, maxLon, maxLat);

    return extent;
}


void 
GeodeticGraticule::updateLabels()
{
    const osgEarth::SpatialReference* srs = osgEarth::SpatialReference::create("wgs84");
    
    Threading::ScopedMutexLock lock(_cameraDataMapMutex);
    for (CameraDataMap::iterator i = _cameraDataMap.begin(); i != _cameraDataMap.end(); ++i)
    {
        CameraData& cdata = i->second;

        std::vector< GeoExtent > extents;
        if (cdata._viewExtent.crossesAntimeridian())
        {
            GeoExtent first, second;
            cdata._viewExtent.splitAcrossAntimeridian(first, second);
            extents.push_back(first);
            extents.push_back(second);
        }
        else
        {
            extents.push_back( cdata._viewExtent );
        }

        double resDegrees = cdata._resolution * 180.0;
        // We want half the resolution so the labels don't appear as often as the grid lines
        resDegrees *= 2.0;

    
        // Hide all the labels
        for (unsigned int i = 0; i < cdata._labelPool.size(); i++)
        {
            cdata._labelPool[i]->setNodeMask(0);
        }

        // Approximate offset in degrees
        double degOffset = cdata._metersPerPixel / 111000.0;
     
        unsigned int labelIndex = 0;


        bool done = false;
        for (unsigned int extentIndex = 0; extentIndex < extents.size() && !done; extentIndex++)
        {
            GeoExtent extent = extents[extentIndex];

            int minLonIndex = floor(((extent.xMin() + 180.0)/resDegrees));
            int maxLonIndex = ceil(((extent.xMax() + 180.0)/resDegrees));

            int minLatIndex = floor(((extent.yMin() + 90)/resDegrees));
            int maxLatIndex = ceil(((extent.yMax() + 90)/resDegrees));

            // Generate horizontal labels
            for (int i = minLonIndex; i <= maxLonIndex && !done; i++)
            {
                GeoPoint point(srs, -180.0 + (double)i * resDegrees, cdata._lat + (_centerOffset.y() * degOffset), 0, ALTMODE_ABSOLUTE);
                LabelNode* label = cdata._labelPool[labelIndex++].get();

                label->setNodeMask(~0u);
                label->setPosition(point);
                std::string text = getText( point, false);
                label->setText( text );
                if (labelIndex == cdata._labelPool.size() - 1)
                {
                    done = true;
                }
            }

            // Generate the vertical labels
            for (int i = minLatIndex; i <= maxLatIndex && !done; i++)
            {
                GeoPoint point(srs, cdata._lon + (_centerOffset.x() * degOffset), -90.0 + (double)i * resDegrees, 0, ALTMODE_ABSOLUTE);
                // Skip drawing labels at the poles
                if (osg::equivalent(osg::absolute( point.y()), 90.0, 0.1))
                {
                    continue;
                }
                LabelNode* label = cdata._labelPool[labelIndex++].get();
                label->setNodeMask(~0u);
                label->setPosition(point);
                std::string text = getText( point, true);
                label->setText( text );
                if (labelIndex == cdata._labelPool.size() - 1)
                {
                    done = true;
                }
            }
        }
    }
}

GeodeticGraticule::CameraData&
GeodeticGraticule::getCameraData(osg::Camera* cam) const
{
    Threading::ScopedMutexLock lock(_cameraDataMapMutex);
    CameraData& cdata = _cameraDataMap[cam];

    // New camera data? Initialize:
    if (cdata._labelPool.empty())
    {
        cdata._stateset = new osg::StateSet();
        cdata._resolution = _defaultResolution;
        cdata._resolutionUniform = cdata._stateset->getOrCreateUniform(RESOLUTION_UNIFORM, osg::Uniform::FLOAT);
        cdata._resolutionUniform->set(cdata._resolution);
        cdata._viewExtent = GeoExtent(osgEarth::SpatialReference::create("wgs84"), -180, -90, 180, 90);
        cdata._lat = 0.0;
        cdata._lon = 0.0;
        const_cast<GeodeticGraticule*>(this)->initLabelPool(cdata);

        cdata._labelStateset = new osg::StateSet();
        VirtualProgram* vp = VirtualProgram::getOrCreate(cdata._labelStateset.get());
        vp->setFunction("oe_GeodeticGraticule_text_frag", textFadeFS, ShaderComp::LOCATION_FRAGMENT_COLORING);
    }

    return cdata;
}

std::string
GeodeticGraticule::getText(const GeoPoint& location, bool lat)
{ 
    double value = lat ? location.y() : location.x();
    return _formatter->format(value, lat);
}


void
GeodeticGraticule::initLabelPool(CameraData& cdata)
{
    const osgEarth::SpatialReference* srs = osgEarth::SpatialReference::create("wgs84");

    Style style;
    TextSymbol* text = style.getOrCreateSymbol<TextSymbol>();
    text->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
    text->fill()->color() = options().labelColor().get();
    AltitudeSymbol* alt = style.getOrCreateSymbol<AltitudeSymbol>();
    alt->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;

    unsigned int labelPoolSize = 8 * options().gridLines().get();
    for (unsigned int i = 0; i < labelPoolSize; i++)
    {
        GeoPoint pt(srs, 0,0,0);
        LabelNode* label = new LabelNode(_mapNode.get(), pt, "0,0");
        label->setDynamic(true);
        label->setStyle(style);
        cdata._labelPool.push_back(label);
    }
}

osg::StateSet*
GeodeticGraticule::getStateSet(osgUtil::CullVisitor* cv)
{
    CameraData& cdata = getCameraData(cv->getCurrentCamera());
    return cdata._stateset.get();
}
