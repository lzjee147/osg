#include "vtkExTool/vtkExToolObserver.h"

//vtkExToolObserver _g_vtkExToolObserver;

vtkExToolObserver::vtkExToolObserver(void)
{
  mUpdata = false;
 
}
vtkExToolObserver::~vtkExToolObserver(void)
{
 
}


void vtkExToolObserver::_init()
{

}

HRESULT vtkExToolObserver::OnUpdate(/*CView*/void* pSender, LPARAM lHint, WPARAM wHint)
{

   return TRUE;  

}

