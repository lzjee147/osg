// -*-c++-*-

/*
 * OpenSceneGraph - Copyright (C) 1998-2009 Robert Osfield
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
 */

/*
 * osgFX::Transparent - Copyright (C) 2004,2009 Ulrich Hertlein
 */

#include <osgFX/Transparent>
#include <osgFX/Registry>

#include <osg/Group>
#include <osg/Stencil>
#include <osg/CullFace>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/Texture1D>


namespace
{
    const unsigned int Override_On = osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE;
    const unsigned int Override_Off = osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE;
}


namespace osgFX
{
    /// Register prototype.
   // Registry::Proxy proxy(new Transparent);

    /**
     * Transparent technique.
     */
    class Transparent::TransparentTechnique : public Technique
    {
    public:
        /// Constructor.
        TransparentTechnique() : Technique(),
            _lineWidth(), _width(2),
            _material(), _color(1,1,1,1) {
        }

        /// Validate.
        bool validate(osg::State&) const {
            return true;
        }

        /// Set Transparent width.
        void setWidth(float w) {
            _width = w;
            if (_lineWidth.valid()) {
                _lineWidth->setWidth(w);
            }
        }

        /// Set Transparent color.
        void setColor(const osg::Vec4& color) {
            _color = color;
            if (_material.valid()) {
                const osg::Material::Face face = osg::Material::FRONT_AND_BACK;
                _material->setAmbient(face, osg::Vec4(0.0f, 0.0f, 0.0f, 0.5));
                _material->setDiffuse(face, osg::Vec4(0.0f, 0.0f, 0.0f, 0.5));
                _material->setSpecular(face, osg::Vec4(0.0f, 0.0f, 0.0f, 0.5));
                _material->setEmission(face, color);
            }
        }

    protected:
        /// Define render passes.
        void define_passes() {

             osg::StateSet* state = new osg::StateSet;

			 state->setMode(GL_BLEND ,osg::StateAttribute::ON);  

			 osg::ref_ptr<osg::Material> mat = new osg::Material;  

			 mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,1.0,0.0,0.3));  

			 mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,1.0,0.0,0.3));  

			 mat->setTransparency(osg::Material::FRONT_AND_BACK, 0.3);  

			 state->setAttributeAndModes(mat, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);  


			 state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);  


             addPass(state);
        }

    private:
        /// Transparent width.
        osg::ref_ptr<osg::LineWidth> _lineWidth;
        float _width;

        /// Transparent Material.
        osg::ref_ptr<osg::Material> _material;
        osg::Vec4 _color;
    };


    /**
     * Transparent effect.
     */
    Transparent::Transparent() : Effect(), _width(2), _color(1,1,1,1), _technique(0)
    {
    }

    void Transparent::setWidth(float w)
    {
        _width = w;
        if (_technique) {
            _technique->setWidth(w);
        }
    }

    void Transparent::setColor(const osg::Vec4& color)
    {
        _color = color;
        if (_technique) {
            _technique->setColor(color);
        }
    }

    bool Transparent::define_techniques()
    {
        _technique = new TransparentTechnique;
        addTechnique(_technique);

        setWidth(_width);
        setColor(_color);

        return true;
    }
}
