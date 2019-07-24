#ifndef TemplateModule_Evawiz_H
#define TemplateModule_Evawiz_H

namespace evawiz{
  class TemplateModule:public Module{
    DeclareModule(Template);
  public:
    DeclareFunction(TestFunction);
  };
};
#endif
