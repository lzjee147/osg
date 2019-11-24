#include <osg/Geometry>
#include <osgDB/readFile>
#include <osg/LineWidth>
#include <osg/Depth>
#include <windows.h>
#include <iomanip>
#include <sstream>
#include <osgText/Text>
#include "osgDB/FileUtils"

#include <osgWidget/PushbuttonBox>
#include <osgWidget/PushButton>

using namespace osgWidget;

PushbuttonBox::PushbuttonBox() : osgWidget::Box("", BoxType::VERTICAL, false)
{
	button_ = new osgWidget::PushButton(this);

	label_ = NULL;

}


PushbuttonBox::~PushbuttonBox()
{

}   

void PushbuttonBox::setMouseStateInfo(buttonInfo& buttonInfo)
{
	button_->setMouseStateInfo(buttonInfo); 



	button_->setWidth(56) ;
	button_->setHeight(56);


	//³õÊ¼»¯
	std::string imagePath = buttonInfo.images_.front();
	button_->setImage(imagePath, true);

	addWidget(button_); 
}

void PushbuttonBox::removeLabel()
{
	removeWidget(label_);
}


void PushbuttonBox::addLable()   
{
	if(label_)
	{
		label_ = NULL;
	}
	label_  = new osgWidget::Label("","");
	std::string fontpath = osgDB::geExeDirectory()+"\\font\\msyhbd.ttf";
	label_->setFont(fontpath); 

	label_->setFontSize(18);
	label_->setFontColor(1.0f, 1.0f, 1.0f, 1.0f);
	label_->setAlignVertical(osgWidget::Widget::VA_BOTTOM);
	label_->setPadding(1.0f);

	label_->setLabel(wtext_,1);

	addWidget(label_);
}

void PushbuttonBox::addLable(const std::wstring& text)   
{
	if(label_)
	{
		label_ = NULL;
	}
	label_  = new osgWidget::Label("","");
	std::string fontpath = osgDB::geExeDirectory()+"\\font\\msyhbd.ttf";
	label_->setFont(fontpath); 

	

	label_->setFontSize(18);
	label_->setFontColor(1.0f, 1.0f, 1.0f, 1.0f);
	label_->setAlignVertical(osgWidget::Widget::VA_BOTTOM);
	label_->setPadding(1.0f);

	label_->setLabel(text,1);

	addWidget(label_);
}

osgWidget::PushButton* PushbuttonBox::getButton()
{
	return button_;
}

osgWidget::Label* PushbuttonBox::getLabel()
{
	return label_;
}

void PushbuttonBox::setText(const std::string& text) 
{
	text_ = text;
}

void PushbuttonBox::setWText(const std::wstring& text) 
{
	wtext_ = text;
}
