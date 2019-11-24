// -*-c++-*- osgWidget - Code by: Jeremy Moles (cubicool) 2007-2008

#include <osgGe/Math>
#include <osgWidget/WindowManager>
#include <osgWidget/Label>

#include <osgWidget/PushbuttonBox>
#include <osgDB/readFile>

#include <osgWidget/PushButton>

using namespace osgWidget;

PushButton::PushButton(osgWidget::Box* box) : osgWidget::Widget("", 80, 80)
{
	//接收所有事件
	setEventMask(osgWidget::EVENT_ALL); 
	box_ = box;
	mouse_state_ = mouse_push_state1;
	
}

PushButton::~PushButton()
{

}

int PushButton::getCurrentState()
{
	return mouse_state_;
}

//根据鼠标状态设置文字和图片
void PushButton::setLabel()
{
	int textNum = 0;
	int imageNum = 0;
	if(mouse_state_ == mouse_push_state1)
	{
		imageNum = 1;
		textNum = 0;
	}
	else if(mouse_state_ == mouse_push_state2)
	{
		imageNum = 3;
		textNum = 1;
	}
	else if(mouse_state_ == mouse_push_state3)
	{
		imageNum = 5;
		textNum = 2;
	}

	std::vector<std::string>::iterator itImage = button_Info_.images_.begin();
	std::vector<std::wstring>::iterator itText = button_Info_.texts_.begin();

	for(int i = 0;i<textNum;i++)
	{
		itText++;
	}
	for(int i = 0;i<imageNum;i++)
	{
		itImage++;
	}

	((PushbuttonBox*)box_)->addLable((*itText));
	setImage((*itImage), true,true);
}

bool PushButton::mousePush(double, double, const osgWidget::WindowManager* wm)
{
	((PushbuttonBox*)box_)->removeLabel();
	box_->removeWidget(this);
	if(button_Info_.images_.size() == 6)
	{
		if(mouse_state_ == mouse_push_state1)
		{
			mouse_state_ = mouse_push_state2;
		}
		else if (mouse_state_ == mouse_push_state2)
		{
			mouse_state_ = mouse_push_state3;
		}
		else if (mouse_state_ == mouse_push_state3)
		{
			mouse_state_ = mouse_push_state1;
		}
	}
	else if(button_Info_.images_.size() == 4)
	{
		if(mouse_state_ == mouse_push_state1)
		{
			mouse_state_ = mouse_push_state2;
		}
		else if (mouse_state_ == mouse_push_state2)
		{
			mouse_state_ = mouse_push_state1;
		}
	}
	else if(button_Info_.images_.size() == 2)
	{
		mouse_state_ = mouse_push_state1;
	}


	setLabel();
	box_->addWidget(this);
	return true;
}

bool PushButton::mouseRelease(double, double, const osgWidget::WindowManager*)
{
	return true;
}

bool PushButton::mouseEnter(double, double, const osgWidget::WindowManager* wm)
{
	box_->removeWidget(this);

	setLabel();
	setWidth(96) ;
	setHeight(96);

	box_->addOrigin(-24, -80);
	box_->addWidget(this);

	if (wm)
	{
	   osgWidget::WindowManager* wmm = const_cast<osgWidget::WindowManager*>(wm);
		wmm->setCurrentWidget(this);
	}

	return true;
}

bool PushButton::mouseLeave(double, double, const osgWidget::WindowManager* wm)
{
	if (wm)
	{
		osgWidget::WindowManager* wmm = const_cast<osgWidget::WindowManager*>(wm);
		wmm->setCurrentWidget(0);
	}
	
	box_->removeWidget(this);

	int imageNum = 0;
	if(mouse_state_ == mouse_push_state1)
	{
		imageNum = 0;
	}
	else if(mouse_state_ == mouse_push_state2)
	{
		imageNum = 2;
	}
	else if(mouse_state_ == mouse_push_state3)
	{
		imageNum = 4;
	}

	std::vector<std::string>::iterator it = button_Info_.images_.begin();
	for(int i = 0;i<imageNum;i++)
	{
		it++;
	}

	setImage((*it), true,true);

	setWidth(56) ;
	setHeight(56);

	box_->addOrigin(24, 80);
	box_->addWidget(this);

	((PushbuttonBox*)box_)->removeLabel();

	return true;
}

void PushButton::setMouseStateInfo(buttonInfo& button_Info)
{
	//normal_image_ = osgDB::readRefImageFile(file_pasths[0]);
	//hover_image_  = osgDB::readRefImageFile(file_pasths[1]);
	//press_image_  = osgDB::readRefImageFile(file_pasths[2]);

	button_Info_ = button_Info;
}

void PushButton::setMouseState(int mouseState)
{
	mouse_state_ = mouseState;
}

int PushButton::getImageSize()
{
	return mouse_state_;
}