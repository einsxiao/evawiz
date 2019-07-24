#ifndef Runmod_GuisModule_EVAWIZ_H
#define Runmod_GuisModule_EVAWIZ_H

class FieldType{
public:
  const static int Total = 0;
  const static int Part  = 1;
  int              type;
  QPlainTextEdit  *edit;
  evawiz::Object   obj;
  FieldType(int,QPlainTextEdit*,evawiz::Object );
};
class Runmod: public QMainWindow
{
  Q_OBJECT
public:
  Runmod();
private:
  void InitEmptyUI();
  void InitUI();
  void LoadDefaultInput();
  void LoadInput(string,evawiz::Object&input);
  bool Object2TextParts(evawiz::Object&,string[3],FieldType*ft);
  void FlushUI();
  void UpdateInput(evawiz::Object intput);
    
private slots:
  void load();
  void loadModuleDefault();
  void save();
  void run();
  void runLocal();
  void stopLocal();
  void pauseLocal();
  void resumeLocal();
  void runPbs();
  void quit();
  void setDefault(int i);
  void setOriginal(int i);
  void logClear();
  void logSelectAll();
  void logCopy();
  void dealOutput();
  void dealError();
  void appendOutput(QString);
  void appendError(QString);
private:
  string moduleName;
  string inputFileName,defaultInputFileName;
  vector< FieldType* > fields;
  evawiz::Object defaultInput;
  evawiz::Object input;

private:
  QGroupBox      *groupBox_main;
  QVBoxLayout    *boxLayout_main;

  //paras
  QGroupBox      *groupBox_paras_out;
  QVBoxLayout    *boxLayout_paras_out;
  QScrollArea    *groupBox_paras_scroll;

  //run paras
  QCheckBox      *checkBox_run;
  QPlainTextEdit *queue_type;
  QSpinBox       *spinBox_nodes;
  QSpinBox       *spinBox_ppns;

  //temp 
  QHBoxLayout    *layout;
  QLabel         *label1;
  QPlainTextEdit *edit;
  QLabel         *label2;
  QPushButton    *button;

  // log part
  QTextEdit      *edit_log;

  QAction        *loadAction, *loadModuleDefaultAction, *saveAction, *runLocalAction, *runPbsAction, *exitAction;
  QMenu          *fileMenu;

  QProcess       *proc = NULL;
  bool            isLocalRunning();
  const int       PbsState_None=0; 
  const int       PbsState_Submit=1; 
  const int       PbsState_Running=2; 
  const int       PbsState_Exit=3; 
  const int       PbsState_Cancel=4; 
  int             pbsState=0;
};

#endif
