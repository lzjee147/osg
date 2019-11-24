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
#include <osgEarth/VideoLayer>
#include <osg/ImageStream>
#include <osgEarth/Registry>

using namespace osgEarth;

REGISTER_OSGEARTH_LAYER(video, VideoLayer);

VideoLayerOptions::VideoLayerOptions() :
ImageLayerOptions()
{
    setDefaults();
    fromConfig(_conf);
}

VideoLayerOptions::VideoLayerOptions(const ConfigOptions& options) :
ImageLayerOptions( options )
{
    setDefaults();
    fromConfig( _conf );
}

VideoLayerOptions::VideoLayerOptions(const std::string& name) :
ImageLayerOptions( name )
{
    setDefaults();
    fromConfig( _conf );
}

void
VideoLayerOptions::setDefaults()
{
}

Config
VideoLayerOptions::getConfig() const
{
    Config conf = ImageLayerOptions::getConfig();
    conf.key() = "video";

    conf.set("url", _url);

    return conf;
}

void
VideoLayerOptions::fromConfig( const Config& conf )
{
    conf.getIfSet("url", _url );
}

void
VideoLayerOptions::mergeConfig( const Config& conf )
{
    ImageLayerOptions::mergeConfig( conf );
    fromConfig( conf );
}


VideoLayer::VideoLayer()
{
    init();
}

VideoLayer::VideoLayer( const VideoLayerOptions& options ):
ImageLayer(&_optionsConcrete),
_options(&_optionsConcrete),
_optionsConcrete(options)
{    
    init();    
}

const Status&
VideoLayer::open()
{
    if ( !_openCalled )
    {

        if (!options().url().isSet())
        {
            return setStatus(Status::Error(Status::ConfigurationError, "Missing required url"));
        }

        osg::ref_ptr< osg::Image > image = options().url()->readImage().getImage();
        if (image.valid())
        {             
            osg::ImageStream* is = dynamic_cast< osg::ImageStream*>( image.get() );
            if (is)
            {
                is->setLoopingMode(osg::ImageStream::LOOPING);
                is->play();                 
            }

            _texture = new osg::Texture2D( image );
            _texture->setResizeNonPowerOfTwoHint( false );
            _texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::LINEAR);
            _texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::LINEAR);
            _texture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
            _texture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
            _texture->setUnRefImageDataAfterApply(false);
        }
        else
        {
            std::stringstream buf;
            buf << "Failed to load " << options().url()->full();
            return setStatus(Status::Error(Status::ServiceUnavailable, buf.str()));
        }

        setProfile(osgEarth::Registry::instance()->getGlobalGeodeticProfile());

        if (getStatus().isOK())
        {
            return ImageLayer::open();
        }
    }

    return getStatus();
}

osg::Texture* VideoLayer::createTexture(const TileKey& key, ProgressCallback* progress, osg::Matrixf& textureMatrix)
{    
    if (key.getLOD() > 0) return 0;

    bool flip = _texture->getImage()->getOrigin()==osg::Image::TOP_LEFT;
    osg::Matrixf scale = osg::Matrixf::scale(0.5, flip? -1.0 : 1.0, 1.0);         

    if (key.getTileX() == 0)
    {
        textureMatrix = scale;
    }
    else if (key.getTileX() == 1)
    {
        textureMatrix =  scale * osg::Matrixf::translate(0.5, 0.0, 0.0);
    }

    return _texture.get();
}