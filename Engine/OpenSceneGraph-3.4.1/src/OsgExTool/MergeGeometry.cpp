
#include <osg/Group>  
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osgDB/FileUtils>  
#include <osgUtil/PrintVisitor>  
#include <osgViewer/ViewerEventHandlers>  
#include <osgViewer/Viewer>  
#include <iostream>  
#include <fstream>  
#include <sstream>  

#include <OsgExTool/MergeGeometry>
#include <OsgExTool/OsgExTool>

#include <osg/BlendFunc>
#include <osg/BlendEquation>

using namespace OsgExTool;

osg::Program* MergeGeometryVisitor::createSelectShader()
{
	osg::Program* pgm = new osg::Program;
	pgm->setName( "demo" );

	pgm->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("D://test//lod.vert")));
	pgm->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("D://test//lod.frag")));

	return pgm;
}

osg::StateSet* MergeGeometryVisitor::createStateSet(osg::Node* node, osg::Image* image, float total)
{
	osg::StateSet* stateset = node->getOrCreateStateSet();

	osg::Texture2D * texture = new osg::Texture2D;
	texture->setImage(image);

	//这几行代码重要，不能删，否则纹理
	texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
	texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

	texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
	texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);

	//压缩，LOD，PBO
	texture->setInternalFormatMode(osg::Texture::USE_S3TC_DXT1_COMPRESSION);

	stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON); 

	//
	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);

	stateset->setAttributeAndModes(blendFunc);

	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);

	//osg::ref_ptr<osg::BlendEquation> blendEquation = new osg::BlendEquation();
	//blendEquation->setEquationRGB(osg::BlendEquation::FUNC_ADD);
	//stateset->setAttributeAndModes(blendEquation);

	stateset->setAttribute( createSelectShader() );

	osg::Uniform* u_anim1( new osg::Uniform( "total", total ) ); 

	stateset->addUniform( u_anim1 );

	osg::Uniform* texUniform = new osg::Uniform(osg::Uniform::SAMPLER_2D, "texA"); 
	texUniform->set(0);

	stateset->addUniform( texUniform );

	return stateset;
}

void MergeGeometryVisitor::GetGeometry(osg::Geometry* geom_)
{
		osg::Vec2Array* texcoords_ = (osg::Vec2Array*)(geom_->getTexCoordArray(0));

		osg::Vec3Array* va_     = (osg::Vec3Array*)(geom_->getVertexArray() )     ;
		osg::Vec3Array* normal_ = (osg::Vec3Array*)(geom_->getNormalArray() )     ;
		osg::Vec4Array* color_  = (osg::Vec4Array*)(geom_->getColorArray() )      ;

		for(size_t i = 0; i < va_vec_.size() / 3; i++)
		{
			osg::Vec3f f = va_vec_[3 * i];
			osg::Vec3f s = va_vec_[3 * i + 1];
			osg::Vec3f d = va_vec_[3 * i + 2];

			float a = (f-s).length();
			float b = (f-d).length();
			float c = (d-s).length();

			float p = (a + b + c) * 0.5;

			float dis = sqrt(p * ( p - a) * (p - b) * (p - c));

			//if(dis < ratios_)
			//{
			//	continue;
			//}

			va_->push_back(va_vec_[3 * i])    ;
			va_->push_back(va_vec_[3 * i + 1]);
			va_->push_back(va_vec_[3 * i + 2]);

			normal_->push_back(normal_vec_[3 * i])    ;
			normal_->push_back(normal_vec_[3 * i + 1]);
			normal_->push_back(normal_vec_[3 * i + 2]);		

			texcoords_->push_back(texcoords_vec_[3 * i])    ;
			texcoords_->push_back(texcoords_vec_[3 * i + 1]);
			texcoords_->push_back(texcoords_vec_[3 * i + 2]);

			color_->push_back(color_vec_[3 * i])    ;
			color_->push_back(color_vec_[3 * i + 1]);
			color_->push_back(color_vec_[3 * i + 2]);

		}


		//osg::ref_ptr<osg::DrawElementsUInt> triangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

	    //for (size_t kk = 0; kk < index_.size(); kk++)
	    //    {
		//	 triangle->push_back(index_[kk]);
	    //    }

		//geom_->addPrimitiveSet(triangle.get());

		geom_->addPrimitiveSet( new osg::DrawArrays(GL_TRIANGLES, 0, va_->size()) );
}

int MergeGeometryVisitor::GetTotalGrid()
{
	int i = 0;
	while( i * i < image_geos.size())
	{
		i++;
	}

	return i;
}


bool MergeGeometryVisitor::IsEmpty(osg::Image* image)
{
	for(size_t i = 0; i < 100; i++)
	{
		if(image->data()[i] != 0)
		{
			return true;
		}
	}

	return false;
}

void MergeGeometryVisitor::GetRowColumn(int image_num, float& col, float & row)
{
	int g  = GetTotalGrid();

	row = 0;
	while(row *  g <= image_num)
	{
		row++;
	}

	//得到行
	row--;
	col = image_num - row * g;
	row++;
	col++;
}

	MergeGeometryVisitor::MergeGeometryVisitor(int image_size, float ratios) : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
	{
		image_size_ = 128;

		index_siz_ = 0;

		ratios_ = ratios;

		isScale_ = false;

		color_second_ = new osg::Vec4Array();

		fog_ = new osg::Vec4Array();

		osg::Image* image = new osg::Image;
		image->allocateImage(image_size_, image_size_, 1,
			GL_RGB, GL_UNSIGNED_BYTE);

		image->setFileName("kongbaiwenli");

		for(unsigned int r=0;r<image_size_;++r)
		{
			for(unsigned int c=0;c<image_size_;++c)
			{
				unsigned char* p = image->data(r, c);
				*p++ = 150;
				*p++ = 150;
				*p++ = 150;
			}
		}

		std::vector<osg::Geometry*> geos;

		image_geos[image] = geos;	

		//
		test_group_ = new osg::Group();

		mm_ = 100;

	}

	osg::Image * MergeGeometryVisitor::GridImage(int image_size)
	{
		if(image_geos.size() == 0)
		{
			return NULL;
		}

		std::vector<osg::Image*> images;
		int total = GetTotalGrid();

		std::map<osg::Image*, std::vector<osg::Geometry*>>::iterator it = image_geos.begin();

		for(int k = 0; it != image_geos.end(); k++, it++)
		{
			osg::Image* img = it->first;

			images.push_back(it->first);
		}

		//所有图片大小都一样
		osg::Image *markerImage = new osg::Image();
		markerImage->allocateImage(total * images[0]->s(), total * images[0]->t(), 1, images[0]->getPixelFormat(), GL_UNSIGNED_BYTE, 1);

		osg::Image* temp = NULL;
		for(size_t i = 0; i < total ; i++)
		{
			for(size_t j = 0; j < total; j++)
			{
				if(i * total + j < images.size())
				{
					temp = images[i * total + j];
				}
				else
				{
					temp = images[0];
				}

				markerImage->copySubImage(temp->s() * j, temp->t() * i, 0, temp);
			}
		}

		/*	if (isScale_)*/
		if (ratios_ < 0.01 && total > 5 && total != 8)
		{
			markerImage->scaleImage(1024, 1024, 1);
		}
		else if (ratios_ < 0.01 &&  (total == 5))
		{
			markerImage->scaleImage(512, 512, 1);
		}
		else if (ratios_ < 0.01 &&  (total == 3))
		{
			markerImage->scaleImage(256, 256, 1);
		}
		else if (ratios_ > 0.01)
		{
			markerImage->scaleImage(image_size, image_size, 1);
		}

		return markerImage;
	}

	void MergeGeometryVisitor::ModifyImage(int image_num, osg::Image* image, std::vector<osg::Geometry*>& geoms)
	{			  
		for(size_t j = 0; j < geoms.size(); j++)
		{
			osg::Vec2Array* texcoord = (osg::Vec2Array*)(geoms[j]->getTexCoordArray(0));

			osg::Vec3Array* vertice = (osg::Vec3Array*)(geoms[j]->getVertexArray() );
			osg::Vec3Array* normal = (osg::Vec3Array*)(geoms[j]->getNormalArray() );

			
			//处理索引
			//osg::DrawElementsUInt* drawArrays = dynamic_cast<osg::DrawElementsUInt*> (geoms[j]->getPrimitiveSet(0));

			//for (size_t u = 0; u < drawArrays->size(); u++)
			//{
			//	index_.push_back(index_siz_ + drawArrays->at(u));
			//}

			//处理索引
		/*	index_siz_ = index_siz_ + vertice->size();*/

			osg::ref_ptr<osg::StateSet> stateset = geoms[j]->getStateSet();

			osg::Material* material = NULL;
			osg::TexMat* mat = NULL;

			if(stateset)
			{	
				material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));

				mat = dynamic_cast<osg::TexMat*>(stateset->getTextureAttribute(0,osg::StateAttribute::TEXMAT));
			}

			for(size_t i = 0; i < vertice->size(); i++)
			{
				va_vec_.push_back(vertice->at(i));
				//插入纹理, 考虑纹理和顶点不一致的情况
				if(texcoord && i < texcoord->size())
				{
					if (mat)
					{
						osg::Vec4 vec(texcoord->at(i)[0],  texcoord->at(i)[1], 0, 1);
						vec = mat->getMatrix() * vec;
						texcoords_vec_.push_back(osg::Vec2(vec[0], vec[1]));
					}
					else
					{
						texcoords_vec_.push_back(texcoord->at(i));
					}

				}
				else if(texcoord)//纹理的数目小于顶点的数目
				{
					texcoords_vec_.push_back(osg::Vec2f(0.1, 0.1));
				}

				//把法向量放到总的纹理数组
				if(normal && i < normal->size())
				{
					normal_vec_.push_back(normal->at(i));
				}
				else
				{
					normal_vec_.push_back(osg::Vec3f(0.0, 0.0, 1.0));
				}

				float  row,  col;
				GetRowColumn(image_num, row, col);

				//此处出现错误，要有纹理坐标
				if(material)
				{
					osg::Vec4 ambient = material->getAmbient(osg::Material::FRONT_AND_BACK);

					osg::Vec4 diffuse =  material->getDiffuse(osg::Material::FRONT_AND_BACK);

					osg::Vec4 specular = material->getSpecular(osg::Material::FRONT_AND_BACK);

					float d = 0.0;

					if (diffuse.w() > 0.99)
					{
						d = 0.99;
					}
					else
					{
						d = diffuse.w();
					}

					color_vec_.push_back(osg::Vec4(image_num + d, diffuse.x(), diffuse.y(), diffuse.z())); 
				}
				else
				{
					color_vec_.push_back(osg::Vec4f(image_num + 0.99, 1.0, 1.0, 1.0));
				}

			}
		}
	}


	void MergeGeometryVisitor::MergerTex()
	{
		total_ = GetTotalGrid();

		std::map<osg::Image*, std::vector<osg::Geometry*>>::iterator it =  image_geos.begin();

		for(int i = 0; it != image_geos.end(); i++, it++)
		{
			ModifyImage(i, it->first, it->second);
		}

		//再处理没有纹理的
		int k = 0;
		{
			std::map<osg::Image*, std::vector<osg::Geometry*>>::iterator it = image_geos.begin();

			for(k = 0; it != image_geos.end(); k++, it++)
			{
				osg::Image* img = it->first;

				if (img->getFileName() == "kongbaiwenli")
				{
					break;
				}
			}
		}

		for(size_t j = 0; j < geometrys_.size(); j++)
		{
			osg::Vec3Array* vertice = (osg::Vec3Array*)(geometrys_[j]->getVertexArray() );
			osg::Vec3Array* normal = (osg::Vec3Array*)(geometrys_[j]->getNormalArray() ) ;
			osg::Vec4Array* color = (osg::Vec4Array*)(geometrys_[j]->getColorArray() )   ;

			//osg::DrawElementsUInt* drawArrays = dynamic_cast<osg::DrawElementsUInt*> (geometrys_[j]->getPrimitiveSet(0));

			//for (size_t u = 0; u < drawArrays->size(); u++)
			//{
			//	index_.push_back(index_siz_ + drawArrays->at(u));
			//}

			////处理索引
			//index_siz_ = index_siz_ + vertice->size();

			for(size_t i = 0; i < vertice->size(); i++)
			{
				va_vec_.push_back(vertice->at(i));

				if(normal && i < normal->size())
				{
					normal_vec_.push_back(normal->at(i));
				}
				else
				{
					normal_vec_.push_back(osg::Vec3f(0.0, 0.0, 1.0));
				}

				//没有纹理，就用-100代替
				texcoords_vec_.push_back(osg::Vec2f(0.1, 0.1));

				osg::ref_ptr<osg::StateSet> stateset = geometrys_[j]->getStateSet();

				osg::Material* material = NULL;
				
				if (stateset)
				{
				    material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
				}
				
	
				if(material)
				{
					//
					osg::Vec4 diffuse =  material->getDiffuse(osg::Material::FRONT_AND_BACK);

					float d = 0.0;

					if (diffuse.w() > 0.99)
					{
						d = 0.99;
					}
					else
					{
						d = diffuse.w();
					}

					color_vec_.push_back(osg::Vec4(k + d, diffuse.x(), diffuse.y(), diffuse.z())); 
					//color_vec_.push_back(osg::Vec4(k, 1.0, 1.0, 1.0)); 
				}
				else
				{
					//color_vec_.push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));
					color_vec_.push_back(osg::Vec4(k + 0.99, 1.0, 1.0, 1.0)); 
				}

			}

		}

	}

void MergeGeometryVisitor::TransformTriangle(osg::Geometry& geo)
{

}

void MergeGeometryVisitor::apply(osg::Geode& geo)
{
	test_group_->addChild(&geo);

	static int ccc = 0;
	ccc++;

	if (ccc == 50)
	{
		osgDB::writeNodeFile(*test_group_, "D://xxx1.ive");
	}
}

void MergeGeometryVisitor::apply(osg::Geometry& geo)
{
	   // OsgExTool::OsgExToolSet::TransformGeometry(geo);

		osg::ref_ptr<osg::StateSet> stateset = geo.getStateSet();

		if(stateset)
		{
			osg::ref_ptr<osg::Texture2D> texture2D = dynamic_cast<osg::Texture2D*>(stateset->getTextureAttribute(0,osg::StateAttribute::TEXTURE));

			osg::Vec2Array* texcoord = (osg::Vec2Array*)(geo.getTexCoordArray(0));
			//如果txcoord为空，这里判断不严密，可能存在坐标自动生成的可能
			if(texcoord && texture2D != NULL && texture2D->getImage() && texture2D->getImage()->data() && IsEmpty(texture2D->getImage()))
			{
				osg::Texture2D::WrapMode wrap_s = texture2D->getWrap(osg::Texture2D::WRAP_S);
				osg::Texture2D::WrapMode wrap_t = texture2D->getWrap(osg::Texture2D::WRAP_T);

				osg::Image* img =  texture2D->getImage();

				img->scaleImage(128, 128, 1);

				std::map<osg::Image*, std::vector<osg::Geometry*>>::iterator it = image_geos.find(img);

				if(it == image_geos.end())
				{
					bool is_find_image = false;

					for (it = image_geos.begin(); it != image_geos.end(); it++)
					{
						if (osgDB::getSimpleFileName(it->first->getFileName()) == osgDB::getSimpleFileName(img->getFileName()) )
						{
							it->second.push_back(&geo);

							is_find_image = true;
							break;
						}
					}

					if (!is_find_image)
					{
						std::vector<osg::Geometry*> geos;

						geos.push_back(&geo);

						image_geos[img] = geos;
					}
				}
				else
				{
					it->second.push_back(&geo);
				}
			}
			else
			{
				//如果没有纹理,就集中出来
				geometrys_.push_back(&geo);
			}
		}
		else
		{
			geometrys_.push_back(&geo);
		}

		traverse(geo);
}


osg::ref_ptr<osg::Geode> MergeGeometryVisitor::GetMergeGeometry(osg::ref_ptr<osg::Node> node, std::string out_path, int image_size )
{
	MergeGeometryVisitor merge(image_size, 0.0001);

	node->accept(merge);

	merge.MergerTex();

	std::cout<<"MergerTex done"<<std::endl;

	osg::Image* image = merge.GridImage(256);

	osgDB::writeImageFile(*image, "D://xxxxx.png");

	//image = osgDB::readImageFile("D://xxxxx.dds");

	//image->setPixelFormat(osg::Texture::GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);

	osg::ref_ptr<osg::Geometry> geom_input_      = new osg::Geometry   ;  
	osg::ref_ptr<osg::Vec3Array>     va_input_        = new osg::Vec3Array(); 
	osg::ref_ptr<osg::Vec3Array> normal_input_    = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec2Array> texcoords_input_ = new osg::Vec2Array(); 
	osg::ref_ptr<osg::Vec4Array> colors_input_    = new osg::Vec4Array();

	geom_input_->setVertexArray( va_input_)                                        ;  
	geom_input_->setNormalArray( normal_input_, osg::Array::BIND_PER_VERTEX)       ;
	geom_input_->setColorArray(colors_input_, osg::Array::BIND_PER_VERTEX)         ;
	geom_input_->setTexCoordArray(0, texcoords_input_, osg::Array::BIND_PER_VERTEX);

	std::cout<<"GetGeometry"<<std::endl;

	merge.GetGeometry(geom_input_);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();

	geode->addChild(geom_input_);

	std::cout<<"createStateSet"<<std::endl;

	merge.createStateSet(geode, image, merge.GetTotalGrid());

	osg::ref_ptr<osgDB::ReaderWriter::Options>  op = new osgDB::ReaderWriter::Options();
	op->setOptionString("compressed"); 

	if (!out_path.empty())
	{
		osgDB::writeNodeFile(*geode.get(), out_path, op.get());
	}
	

	return geode;
}
