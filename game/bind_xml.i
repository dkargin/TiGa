%module TiGa

%{
#include "stdafx.h"
%}

%nodefaultctor; // Disable constructor auto binding
%nodefaultdtor;	// Disable destructor auto binding 

%include bindMath.i
%include bindFx.i
%include bindDevices.i
%include bindObjects.i

%include bindHGE.i
%include bindWorld.i