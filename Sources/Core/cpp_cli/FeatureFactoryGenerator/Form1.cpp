#include "stdafx.h"
#include "Form1.h"
#include "FeatureEditor.h"
#include <windows.h>


using namespace FeatureFactoryGenerator;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
	Application::Run(new FeatureEditor());
	return 0;
}

