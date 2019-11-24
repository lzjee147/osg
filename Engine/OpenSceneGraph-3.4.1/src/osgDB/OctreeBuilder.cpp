
#include <osg/Group>  
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osgUtil/PrintVisitor>  
#include <osgViewer/ViewerEventHandlers>  
#include <osgViewer/Viewer>  
#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <osgDB/OctreeBuilder>

#include <osg/ComputeBoundsVisitor>

#include <osg/LineSegment>

#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <osgDB/ReadFile>


using namespace osgDB;

int GetGlobalName_()
{
	int static global_name = 0;
	global_name++;

	return global_name;
}

void GetNodeFile(FourthNode* root, osg::LineSegment* line, std::vector<std::string>& files)
{
	if(root)
	{
		if (line->intersect(root->box) && root->group_ && root->group_->getNumChildren())
		{
			osg::ComputeBoundsVisitor boundVisitor;

			root->group_->accept(boundVisitor);

			if (line->intersect(boundVisitor.getBoundingBox()))
			{
				files.push_back(osgDB::getFilePath(root->block_name_) + std::string("/Data/") + osgDB::getSimpleFileName(root->block_name_));
			}
		}

		GetNodeFile(root->node[0], line, files);

		GetNodeFile(root->node[1], line, files);

		GetNodeFile(root->node[2], line, files);

		GetNodeFile(root->node[3], line, files);
	}
}

std::vector<std::string> OctreeBuilder::GetNodeFiles(osg::Vec3d f, osg::Vec3d s)
{
	osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment();

	std::vector<std::string> files;

	line->set(f, s);

	GetNodeFile(fourthNode_, line.get(), files);

	return files;
}


void GetSelectNodesT(FourthNode* root, osg::LineSegment* line, osg::Group*& group,std::string projectpath)
{
	if(root)
	{
		//
		if (line->intersect(root->box) && !root->group_)
		{
			//root->group_ = (osg::Group*)osgDB::readNodeFile( osgDB::getFilePath(root->block_name_) + std::string("/Data/") + osgDB::getSimpleFileName(root->block_name_));
			
			//if(root->group_)
			//{
				//代码暂时不用
				//for (size_t i = 0; i < root->group_->getNumChildren(); i++)
				//{
				//	((osg::Geode*)root->group_->getChild(i))->getOrCreateUserDataContainer()->addUserObject(root);
				//}

				// group->addChild(root->group_);

			/*		}
			else
			{*/
				//assert(0);
		/*	}*/
			//算出包围盒
            std::map<std::string, osg::BoundingBox>::iterator it = root->geodes_box_.begin();

			for (; it != root->geodes_box_.end(); it++)
			{
				if (line->intersect(it->second))
				{
					group->addChild(osgDB::readNodeFile( projectpath+ std::string("/data/") + osgDB::getSimpleFileName(it->first)));
				}
			}

		}

		GetSelectNodesT(root->node[0], line, group,projectpath);

		GetSelectNodesT(root->node[1], line, group,projectpath);

		GetSelectNodesT(root->node[2], line, group,projectpath);

		GetSelectNodesT(root->node[3], line, group,projectpath);
	}
}

void OctreeBuilder::GetSelectNodes(osg::Vec3d f, osg::Vec3d s, osg::Group* group)
{
	osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment();

	std::vector<std::string> files;

	line->set(f, s);

	GetSelectNodesT(fourthNode_, line.get(), group,project_path_);
}

void GetMeasureNodesT(FourthNode* root, osg::LineSegment* line, osg::Group*& group,std::string project_path_)
{
	if(root)
	{
		if (line->intersect(root->box))
		{
			group->addChild( osgDB::readNodeFile( project_path_ + std::string("/Data/") + osgDB::getSimpleFileName(root->block_name_)) );
		}

		GetMeasureNodesT(root->node[0], line, group,project_path_);

		GetMeasureNodesT(root->node[1], line, group,project_path_);

		GetMeasureNodesT(root->node[2], line, group,project_path_);

		GetMeasureNodesT(root->node[3], line, group,project_path_);
	}
}


void OctreeBuilder::GetMeasureNodes(osg::Vec3d f, osg::Vec3d s, osg::Group* group)
{
	osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment();

	std::vector<std::string> files;

	line->set(f, s);

	GetMeasureNodesT(fourthNode_, line.get(), group,project_path_);
}

void OctreeBuilder::build(FourthNode* root, int num,std::string projectpath)
{  
	project_path_ = projectpath;
	fourthNode_ = root;

	root->name = "X0";
	FindNode(num, root);
}

void OctreeBuilder::FindNode(int num, FourthNode* root)
{
	osg::BoundingBoxf total = root->box;

	if (root->num < num)
	{
		osg::BoundingBoxf box1(( total.corner(3) + total.corner(0) ) * 0.5,  total.corner(7));

		osg::BoundingBoxf box2(( total.corner(2) + total.corner(0) ) * 0.5, ( total.corner(6) + total.corner(7) ) * 0.5);

		osg::BoundingBoxf box3( total.corner(0), ( total.corner(6) + total.corner(5) ) * 0.5);

		osg::BoundingBoxf box4(( total.corner(1) + total.corner(0) ) * 0.5, ( total.corner(5) + total.corner(7) ) * 0.5);


		if (!root->node[0])
		{
			root->node[0] = new FourthNode();

			root->node[0]->root = root;

			root->node[0]->name = root->name + "X0";
		}

		root->node[0]->box = box1;
		root->node[0]->ive_fold = root->ive_fold;
		root->node[0]->num = root->num + 1;
	

		FindNode(num, root->node[0]);


		if (!root->node[1])
		{
			root->node[1] = new FourthNode();

			root->node[1]->root = root;

			root->node[1]->name = root->name + "X1";
		}

		root->node[1]->box = box2;


		root->node[1]->ive_fold = root->ive_fold;

		root->node[1]->num = root->num + 1;



		FindNode(num, root->node[1]);


		if (!root->node[2])
		{
			root->node[2] = new FourthNode();

			root->node[2]->root = root;

			root->node[2]->name = root->name + "X2";
		}

		root->node[2]->box = box3;


		root->node[2]->ive_fold = root->ive_fold;

		root->node[2]->num = root->num + 1;


		FindNode(num, root->node[2]);


		if (!root->node[3])
		{
			root->node[3] = new FourthNode();

			root->node[3]->root = root;

			root->node[3]->name = root->name + "X3";
		}

		root->node[3]->box = box4;

		root->node[3]->ive_fold = root->ive_fold;

		root->node[3]->num = root->num + 1;

		FindNode(num, root->node[3]);   
	}
}

FourthNode* OctreeBuilder::GetRoot()
{
	return fourthNode_;
}

void SetFace(osg::ref_ptr<osg::Vec3Array> va, const osg::BoundingBoxf& box, int pos1, int pos2, int pos3, int pos4)
{
	va->push_back(box.corner(pos1)) ;
	va->push_back(box.corner(pos2)) ;
	va->push_back(box.corner(pos3)) ;
	va->push_back(box.corner(pos4)) ;
}

void DrawBoundBox(const FourthNode* node, osg::ref_ptr<osg::Vec3Array> va)
{
	SetFace(va, node->box, 0, 1, 5, 4);
	SetFace(va, node->box, 2, 3, 7, 6);

	SetFace(va, node->box, 1, 3, 7, 5);
	SetFace(va, node->box, 0, 2, 6, 4);

	SetFace(va, node->box, 0, 1, 3, 2);
	SetFace(va, node->box, 4, 5, 7, 6);
}

void GetTreeGeode(const FourthNode* node, osg::ref_ptr<osg::Vec3Array> va)
{
	if(node)
	{
		DrawBoundBox(node, va);

		if(node->node[0])
		{
			GetTreeGeode(node->node[0], va);
		}

		if(node->node[1])
		{
			GetTreeGeode(node->node[1], va);
		}

		if(node->node[2])
		{
			GetTreeGeode(node->node[2], va);
		}

		if(node->node[3])
		{
			GetTreeGeode(node->node[3], va);
		}

	}

}

osg::Geode* OctreeBuilder::DrawTree()
{
	osg::Geode* geode = new osg::Geode();

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;  

	osg::ref_ptr<osg::Vec3Array> va = new osg::Vec3Array(); 

	GetTreeGeode(fourthNode_, va);

	geom->setVertexArray( va.get() );  
	geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, va->size()) ); 

	geode->addDrawable(geom);

	osg::StateSet* stateset = geode->getOrCreateStateSet();

	osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();

	polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

	stateset->setAttribute( polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

	return geode;
}
