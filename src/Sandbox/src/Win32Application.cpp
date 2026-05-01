#include "..\include\Win32Application.h"
#include "..\..\Engine\efg.Platform\include\Window.h"

void Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	Window window;
	window.Create(hInstance);
	window.Show(nCmdShow);

	while (window.IsOpen())
	{
		window.PollEvents();
	}
}