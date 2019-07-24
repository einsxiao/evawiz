/****************************/

#include"eva_headers.h"
#include<QtGui/QtGui>


#include"GuisModule.h" 

using namespace std;
using namespace evawiz;
/****************************/
__DEBUG_MAIN__("input.ev");
ModuleInitialize
{

}

ModuleFinalize
{

}

#include"notepad.h"
DefineFunction(Notepad,"Launch a Notepad Qt application.")
{
  QApplication app( EvaKernel->argc, EvaKernel->argv);
  Notepad notepad;
  notepad.show();
  app.exec();
  ReturnNull;
}

#include"runmod.h"
DefineFunction(Runmod,"Launch a Runmod Qt application.")
{
  QApplication app( EvaKernel->argc, EvaKernel->argv);
  Runmod runmod;
  runmod.show();
  app.exec();
  ReturnNull;
}




