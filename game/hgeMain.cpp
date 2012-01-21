#include "stdafx.h"
#include "Core.h"

extern Core * core;

int hgeMain()
{	
	core->runSystem();
	return 0;
}

#ifdef _EXECUTABLE
int main(int argc, char* argv[])
{	
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );	
	//_CrtSetBreakAlloc(14074);
	try
	{
		hgeMain();
	}
	catch(std::exception & ex)
	{
		MessageBox(0,ex.what(),"Error", MB_OK | MB_ICONEXCLAMATION);		
	}
	catch(...)
	{
		MessageBox(0,"Unknown exception","Error", MB_OK | MB_ICONEXCLAMATION);		
	}	
	return 0;
}
#endif