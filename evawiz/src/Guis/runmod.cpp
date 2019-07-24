/****************************/
#include<evawiz>          

#include<QtGui/QtGui>

#include"runmod.h"
#include"runmod.h.moc"

using namespace std;
using namespace evawiz;

FieldType::FieldType(int in_type, QPlainTextEdit *in_edit,Object in_obj)
{
  type = in_type;
  edit = in_edit;
  obj  = in_obj;
}

template<class type>
void dumpInput2Stream(Object &input,type&ofs)
{
  ofs<< "#!/usr/bin/env evawiz"<<endl;
  for ( u_int i=1; i<= input.Size(); i++ ){
    if ( not input[i].NullQ() )
      ofs<< input[i].ToString().c_str() <<endl;
  }
  ofs.flush();
}

bool Runmod::Object2TextParts( Object &obj, string parts[3], FieldType *ft)
{
  if ( obj.AtomQ() or obj.Empty() )
    return false;
  if ( obj.ListQ("Tooltip") ){
    if ( obj.Size() != 1 or not obj[1].StringQ() ){
      Warning("Tooltip","Tooltip requires a String.");
      return false;
    }
    parts[0] = "Tooltip";
    parts[1] = obj[1].Key();
  }else if ( obj.ListQ( "GetModule" ) ){
    if ( obj.Size() != 1 or not obj[1].StringQ() ){
      Warning("GetModule","GetModule does not get a right value.");
      return false;
    }
    moduleName = obj[1].ToString();
    parts[0] = "GetModule[";
    parts[1] =  moduleName;
    parts[2] = "]";
    if ( ft != NULL ){
      ft->type = FieldType::Total;
      ft->obj = obj[1];
    }
  }else if ( obj.ListQ( "Set" ) ){
    if ( obj.Size() != 2 )
      return false;
    parts[0] =  (obj[1].ToString()+" =") ;
    parts[1] =  obj[2].ToString() ;
    parts[2] =  "";
    if ( ft != NULL ){
      ft->type = FieldType::Total;
      ft->obj = obj[2];
    }
  }else if ( obj.ListQ( "Conjunct" ) ){ // operation like Conjunct or any other write directly to expression
    parts[0] =  obj[1].ToString()+"." ;
    parts[1] =  obj[2].ToString();
    parts[2] =  "";
    if ( ft != NULL ){
      ft->type = FieldType::Total;
      ft->obj = obj[2];
    }
  }else if ( obj.ListQ( "List" ) ){
    string str;
    for ( u_int j = 1; j <= obj.Size(); j++ ){
      str += obj[j].ToString();
      if ( j != obj.Size() )
        str += ",";
    }
    parts[0] =  "{";
    parts[1] =  str ;
    parts[2] =  "}";
    if ( ft != NULL ){
      ft->type = FieldType::Part;
      ft->obj = obj;
    }
  }else if ( obj.ListQ( "CompoundExpression" ) ){
    if ( obj.Size() == 0  )
      return false;
    if ( obj.Last().NullQ() ){
      if ( obj.Size() == 1 )
        return false;
      obj.PopBack();
    }
    dout << obj.Last().ToString() << endl;
    return Object2TextParts( obj.Last(), parts, ft);
  }else{
    parts[0] = obj[0].ToString()+"[";
    string str;
    for ( u_int j = 1; j <= obj.Size(); j++ ){
      str += obj[j].ToString();
      if ( j != obj.Size() )
        str += ",";
    }
    parts[1] =  str ;
    parts[2] =  "]";
    if ( ft != NULL ){
      ft->type = FieldType::Part;
      ft->obj = obj;
    }
  }
  return true;
}

Runmod::Runmod()
{
  // basic init
  input.SetList_List();
  defaultInput.SetList_List();

  // init init UI
  InitEmptyUI();

  // init input if input exist
  bool no_input = false;
  moduleName = "";
  if ( EvaKernel->argc > 1 ){
    if ( strcmp( EvaKernel->argv[0],"evawiz" ) == 0 ){
      if ( EvaKernel->argc > 2 ){
        moduleName = EvaKernel->argv[2];
      }else{
        no_input = true;
      }
    }else{
      moduleName = EvaKernel->argv[1];
    }
  }else{
    no_input = true;
  }
  dout<< moduleName<<","<<no_input<<endl;
  inputFileName = "input.ev";
  if ( no_input ){
    LoadInput(inputFileName,input);
  }else{
    loadModuleDefault();
  }

  dout<< EvaKernel->argc <<endl;
  for ( int i=0; i< EvaKernel->argc; i++ )
    dout<< EvaKernel->argv[i] <<endl;
  dout<<endl;
  dout<< "file name:" << inputFileName <<endl;

  // Init UI
  InitUI();

}

void Runmod::FlushUI()
{
  this->repaint();
  this->update();
}

void Runmod::InitEmptyUI()
{
  // main window setting and layout
  resize(800, 600);
  //setFixedWidth(600);
  groupBox_main = new QGroupBox( "" );
  boxLayout_main = new QVBoxLayout;

  groupBox_main = new QGroupBox( "" );
  boxLayout_main = new QVBoxLayout;

  groupBox_main->setLayout( boxLayout_main );
  setCentralWidget( groupBox_main);
 
  {// groupBox parameters
    groupBox_paras_out  = new QGroupBox( "Parameters" );
    boxLayout_main->addWidget( groupBox_paras_out );
    groupBox_paras_out->setFixedHeight(350);
    boxLayout_paras_out = new QVBoxLayout;
    groupBox_paras_out->setLayout( boxLayout_paras_out );
    groupBox_paras_scroll = new QScrollArea;
    boxLayout_paras_out->addWidget( groupBox_paras_scroll );
    groupBox_paras_scroll->setWidgetResizable(true);
    groupBox_paras_scroll->setFrameShape( QFrame::NoFrame );

  }

  {// way to execute the job
    QGroupBox *groupBox_exe  = new QGroupBox( "Execute" );
    groupBox_exe->setFixedHeight(100);
    QHBoxLayout *boxLayout_exe = new QHBoxLayout;
    groupBox_exe->setLayout(boxLayout_exe);
    boxLayout_main->addWidget( groupBox_exe );

    QGridLayout *layout_exe_button = new QGridLayout;
    boxLayout_exe->addLayout( layout_exe_button );
    QPushButton *button_run = new QPushButton("Run");
    connect( button_run, SIGNAL( clicked() ), this, SLOT( run() ) );
    layout_exe_button->addWidget( button_run,0,0);
    button_run->setFixedWidth( 80 );

    button = new QPushButton("Stop");
    connect( button, SIGNAL( clicked() ), this, SLOT( stopLocal() ) );
    layout_exe_button->addWidget( button,0,1);
    button->setFixedWidth( 80 );

    /*
    button = new QPushButton("Pause");
    connect( button, SIGNAL( clicked() ), this, SLOT( pauseLocal() ) );
    layout_exe_button->addWidget( button,1,0);
    button->setFixedWidth( 80 );

    button = new QPushButton("Resume");
    connect( button, SIGNAL( clicked() ), this, SLOT( resumeLocal() ) );
    layout_exe_button->addWidget( button,1,1);
    button->setFixedWidth( 80 );
    */

    checkBox_run = new QCheckBox("Run By PBS");
    //connect( checkBox_run, SIGNAL( stateChanged() ), SLOT( runCheck() ) );
    boxLayout_exe->addWidget( checkBox_run );

    QGroupBox *groupBox_pbs = new QGroupBox("PBS Parameters");
    QHBoxLayout *boxLayout_exe_pbs = new QHBoxLayout;
    groupBox_pbs->setLayout( boxLayout_exe_pbs );
    boxLayout_exe->addWidget( groupBox_pbs );

    boxLayout_exe_pbs->addWidget( new QLabel("queue =") );
    queue_type = new QPlainTextEdit("cpu");
    //queue_type->setFixedWidth(80);
    boxLayout_exe_pbs->addWidget(queue_type);

    boxLayout_exe_pbs->addWidget( new QLabel("nodes =") );
    spinBox_nodes = new QSpinBox;
    boxLayout_exe_pbs->addWidget( spinBox_nodes );
    spinBox_nodes->setValue(1);

    boxLayout_exe_pbs->addWidget( new QLabel("ppn =") );
    spinBox_ppns = new QSpinBox;
    boxLayout_exe_pbs->addWidget( spinBox_ppns );
    spinBox_ppns->setValue(1);

  }

  {//output log window
    QGroupBox *groupBox_log = new QGroupBox( "Output Log" );
    groupBox_log->setFixedHeight(250);
    QVBoxLayout *boxLayout_log = new QVBoxLayout;
    groupBox_log->setLayout(boxLayout_log);
    boxLayout_main->addWidget( groupBox_log);

    edit_log = new QTextEdit;
    boxLayout_log->addWidget( edit_log );
    edit_log->setReadOnly(true);
    edit_log->setText("");

    QGroupBox *groupBox_log_button = new QGroupBox("");
    boxLayout_log->addWidget( groupBox_log_button );
    QHBoxLayout *boxLayout_log_button = new QHBoxLayout;
    groupBox_log_button->setLayout( boxLayout_log_button );
    QPushButton *button_clear = new QPushButton("Clear");
    connect( button_clear, SIGNAL( clicked() ), this, SLOT( logClear() ) );
    boxLayout_log_button->addWidget( button_clear);
    button_clear->setFixedWidth( 80 );

    QPushButton *button_select_all = new QPushButton("Select All");
    connect( button_select_all, SIGNAL( clicked() ), this, SLOT( logSelectAll() ) );
    boxLayout_log_button->addWidget( button_select_all);
    button_select_all->setFixedWidth( 80 );

    QPushButton *button_copy = new QPushButton("Copy");
    connect( button_copy, SIGNAL( clicked() ), this, SLOT( logCopy() ) );
    boxLayout_log_button->addWidget( button_copy);
    button_copy->setFixedWidth( 80 );


  }



  // menu setting and layout
  loadAction = new QAction( tr("&Load from File") , this );
  loadModuleDefaultAction = new QAction( tr("&Load from Module Default Input") , this );
  saveAction = new QAction( tr("&Save Input") , this );
  runPbsAction = new QAction( tr("&Run By PBS") , this );
  runLocalAction = new QAction( tr("&Run Locally") , this );
  exitAction = new QAction( tr("&Exit") , this );

  connect(loadAction, SIGNAL( triggered() ), this, SLOT( load() ) );
  connect(loadModuleDefaultAction, SIGNAL( triggered() ), this, SLOT( loadModuleDefault() ) );
  connect(saveAction, SIGNAL( triggered() ), this, SLOT( save() ) );
  connect(runLocalAction, SIGNAL( triggered() ), this, SLOT( runLocal() ) );
  connect(runPbsAction, SIGNAL( triggered() ), this, SLOT( runPbs() ) );
  connect(exitAction, SIGNAL( triggered() ), this, SLOT( quit() ) );

  fileMenu = menuBar()->addMenu( tr("&Input") );
  fileMenu->addAction( loadAction );
  fileMenu->addAction( loadModuleDefaultAction );
  fileMenu->addAction( saveAction );
  fileMenu = menuBar()->addMenu( tr("&Operation") );
  fileMenu->addAction( runLocalAction );
  fileMenu->addAction( runPbsAction );
  fileMenu->addSeparator();
  //fileMenu->addAction( exitAction );
  
  setWindowTitle( tr("Run Module") );

}

void Runmod::InitUI()
{
  for ( u_int i=0; i< fields.size(); i++ ){
    if ( fields[i] != NULL ){
      delete fields[i];
      fields[i] = NULL;
    }
  }
  fields.clear();
  
  {
    QGroupBox   *groupBox_paras  = new QGroupBox( "" );
    QVBoxLayout *boxLayout_paras = new QVBoxLayout;
    groupBox_paras->setLayout( boxLayout_paras );
    groupBox_paras_scroll->setWidget( groupBox_paras );

    QSignalMapper *signalMapperSetDefault = new QSignalMapper(this);
    QSignalMapper *signalMapperSetOriginal = new QSignalMapper(this);
    string tooltip("");
    string parts[3];
    for ( size_t i = 1; i<= input.Size(); i++ ){
      FieldType *ft = new FieldType(0,NULL,input[i]);
      fields.push_back( ft );
      if ( input[i].NullQ() ) continue;
      if ( input[i].AtomQ() ) continue;
#define NORMAL_LAYOUT {                         \
        layout    = new QHBoxLayout;            \
        edit      = new QPlainTextEdit;         \
        label1    = new QLabel;                 \
        label2    = new QLabel;                 \
        edit->setFixedHeight( 30 );             \
        layout->addWidget( label1 );            \
        layout->addWidget( edit );              \
        layout->addWidget( label2 );            \
      }
      if ( Object2TextParts( input[i], parts, ft) ){
        if ( parts[0] == "Tooltip" ){
          dout<<"record tooltip "<<tooltip<<endl;
          tooltip = parts[1];
          continue;
        }
        NORMAL_LAYOUT;
        ft->edit = edit;
        label1->setText( parts[0].c_str() );
        //QToolTip::showText(pos , ToString( i ).c_str(), label1); 
        edit->setPlainText( parts[1].c_str() );
        label2->setText( parts[2].c_str() );
        if ( tooltip != "" ){
          dout<<"Set Tooltip '"<<tooltip<<"' for "<<parts[0]<<endl;
          label1->setToolTip( tooltip.c_str() );
          tooltip = "";
        }
      }else{
        dout<<"Clean tooltip. term not shown"<<endl;
        tooltip = "";
        continue;
      }

      button = new QPushButton( tr("Default") );
      layout->addWidget( button );
      connect( button, SIGNAL( clicked() ), signalMapperSetDefault , SLOT( map() ) );
      signalMapperSetDefault->setMapping( button, i );

      button = new QPushButton( tr("Original") );
      layout->addWidget( button );
      connect( button, SIGNAL( clicked() ), signalMapperSetOriginal, SLOT( map() ) );
      signalMapperSetOriginal->setMapping( button, i );

      // add to container and other opers.
      boxLayout_paras->addLayout( layout );

      //fields.push_back( FieldType(edit, ) );
    }
    connect( signalMapperSetDefault, SIGNAL( mapped(int) ), this, SLOT( setDefault(int) ) );
    connect( signalMapperSetOriginal, SIGNAL( mapped(int) ), this, SLOT( setOriginal(int) ) );
  }

  return;
 
}

void Runmod::LoadInput(string str_in, Object&input )
{
  input.SetList_List();
  ifstream fcin(str_in);
  if ( fcin ){
    ImportList importList(&fcin,false);
    while ( not importList.End() ){
      importList.GetList( input, 0 );
    }
    fcin.close();
  }else{
    Warning("Runmod","LoadInput","File '" + str_in + "' not found.");
  }
  dout<<input<<endl;
  return;
}

void Runmod::load()
{
  dout<< " load operation executed." <<endl;
  inputFileName = QFileDialog::getOpenFileName( this, tr("Load From File"), "./input.ev", tr("EV Files(*.ev);;All (*)") ).toStdString();
  if ( inputFileName != "" ){
    LoadInput( inputFileName ,input);
    InitUI();
  }
}

void Runmod::loadModuleDefault()
{
  dout<<" load Module Default executed"<<endl;
  moduleName = QInputDialog::getText(this, tr("Get Input from Module"),"Module Name = ",QLineEdit::Normal,moduleName.c_str() ).toStdString();
  if ( moduleName == "" )
    return;
  dout<<moduleName<<endl;
  defaultInputFileName = GetEnv("HOME_EVAWIZ")+"/"+moduleName+"/input.ev";
  dout<<defaultInputFileName<<endl;
  defaultInputFileName = QFileDialog::getOpenFileName( this, tr("Load From File"), defaultInputFileName.c_str(), tr("EV Files(*.ev);;All (*)") ).toStdString();
  LoadInput( defaultInputFileName, defaultInput );
  input.SetObject( defaultInput );
  dout<<input<<endl;
  dout<<defaultInput<<endl;

  //save input to input.ev
  if ( inputFileName == "" )
    inputFileName = "input.ev";
  ofstream ofs(inputFileName);
  dumpInput2Stream(input, ofs );
  ofs.close();

  InitUI();

  return;
}

void Runmod::UpdateInput(Object input)
{
  dout<<"step into UpdateInput"<<endl;
  for ( u_int i = 0; i < fields.size(); i++ ){
    if ( fields[i]->edit != NULL ){
      if ( fields[i]->type ==  FieldType::Total ){
        fields[i]->obj.SetObject( ImportList::ToExpression( (fields[i]->edit->toPlainText()).toStdString() ) );
      }else{
        string str="{" + fields[i]->edit->toPlainText().toStdString() + "}";
        Object obj = ImportList::ToExpression( str );
        fields[i]->obj.DeleteElements();
        dout<<obj<<endl;
        for ( u_int j=1; j<= obj.Size(); j++ ){
          fields[i]->obj.PushBack( obj[j] );
        }
      }
    }
  }
  return;
}

void Runmod::save()
{
  dout<<" save executed" <<endl;
  if ( input.Empty() ){
    QMessageBox::critical( this, tr("Error"), tr("Input not loaded.") );
    return;
  }

  //update Input without gui
  UpdateInput(input);

  QString fileName = QFileDialog::getSaveFileName( this, tr("Save Input"), "./input.ev", tr("EV Files (*.ev);;All (*)") );
  if ( fileName != "" ){
    QFile file( fileName );
    if ( !file.open( QIODevice::WriteOnly ) ){
      QMessageBox::critical( this, tr("Error"), tr("Could not open file to write") );
      return;
    }else{
      QTextStream stream(&file);

      dumpInput2Stream(input, stream);
      
      file.close();
    }
  }
  return;
  
}

void Runmod::run()
{
  dout<<" Run executed"<<endl;
  if ( input.Empty() or inputFileName == "" ){
    QMessageBox::critical( this, tr("Error"), tr("Input not specified.") );
  }

  //save input
  dout<<"inputFile name = "<<inputFileName<<endl;
  ofstream ofs("."+inputFileName);
  dumpInput2Stream(input, ofs );
  ofs.close();

  //run by option of pbs
  if ( checkBox_run->isChecked() ){
    runPbs();
  }else{
    if ( not isLocalRunning() ){
      runLocal();
    }else{
      QMessageBox::critical( this, tr("Error"), tr("Task is running locally.") );
    }
  
  }
  return;
}

void Runmod::appendOutput(QString str)
{
  edit_log->setTextColor( Qt::black );
  edit_log->moveCursor( QTextCursor::End );
  edit_log->insertPlainText( str );
  edit_log->moveCursor( QTextCursor::End );
  FlushUI();
}

void Runmod::appendError(QString str)
{
  edit_log->setTextColor( Qt::red );
  edit_log->moveCursor( QTextCursor::End );
  edit_log->insertPlainText( str );
  edit_log->moveCursor( QTextCursor::End );
  FlushUI();
}

void Runmod::dealOutput()
{
  dout<<"step into dealOutput"<<endl;
  QString strdata = proc->readAll();
  appendOutput( strdata );
  return;
}

void Runmod::dealError()
{
  dout<<"step into dealError"<<endl;
  QString strdata = proc->readAll();
  appendError( strdata );
  return;
}

void Runmod::runLocal()
{
  dout<<" Run Local executed"<<endl;

  proc = new QProcess();

  proc->start("/usr/bin/env", QStringList()<<"evawiz"<<QString( inputFileName.c_str() ) );

  connect(proc, SIGNAL( readyReadStandardOutput() ), this, SLOT(dealOutput() ) );
  connect(proc, SIGNAL( readyReadStandardError() ), this, SLOT(dealError() ) );

  return;
}

void Runmod::runPbs()
{
  dout<<" Run PBS executed"<<endl;
  //proc = new QProcess();
  
  ofstream ofs(".pbs_tmp");
  ofs<<"#PBS -N evawiz_Runmod_"<< moduleName<<"\n";
  ofs<<"#PBS -q "<< (queue_type->toPlainText()).toStdString() <<"\n";
  ofs<<"#PBS -l nodes="<<spinBox_nodes->value()<<":ppn="<<spinBox_ppns->value()<<"\n";
  ofs<<"cd $PBS_O_WORKDIR\n";
  ofs<<"evawiz "<< inputFileName <<" > pbs.outout\n"; 
  ofs.close();
  if ( not CommandExist("qsub") ){
    QMessageBox::critical( this, tr("Error"), tr("PBS system not avaliable.") );
    return;
  }
  appendOutput( "try submit task to PBS system." );
  pbsState = PbsState_Submit;
  string output = System("qsub .pbs_tmp ");
  if ( output == "" ){
    QMessageBox::critical( this, tr("Error"), tr("PBS not running.") );
    return;
  }
  appendOutput( output.c_str() );
  string job_id = output.substr( 0, output.find( '.', 0 ) );
  appendOutput( ("id = "+job_id).c_str() );
  
  return;
}

void Runmod::stopLocal()
{
  dout<<"stop local"<<endl;
  if ( isLocalRunning() ){
    proc->close();
  }

}

void Runmod::pauseLocal()
{
  dout<<"pause local"<<endl;
  if ( isLocalRunning() ){
  }
}

void Runmod::resumeLocal()
{
  dout<<"resume local"<<endl;
  if ( isLocalRunning() ){
  }
}

void Runmod::setDefault(int i)
{
  dout<< "Button "<< i<< " pressed"<<endl;
  if ( defaultInput.Empty() ){//load default first
    if ( defaultInputFileName != "" ){
      LoadInput( defaultInputFileName, defaultInput );
    }else{
      loadModuleDefault();
    }
  }

  if ( defaultInput.Empty() ){
    QMessageBox::critical( this, tr("Error"), tr("Default Input not loaded.") );
    return;
  }

  if ( input[i][0] != defaultInput[i][0] ){
    QMessageBox::critical( this, tr("Error"), tr("Default Input have different sstructure with current Input.") );
    return;
  }
  
  if ( fields[i-1]->edit != NULL ){
    string parts[3];
    Object2TextParts( defaultInput[i], parts, NULL );
    fields[i-1]->edit->setPlainText( parts[1].c_str() );
  }
  FlushUI();
  return;

}

void Runmod::setOriginal(int i)
{
  dout<<"setOriginal clicked."<<endl;
  if ( i >= (int)fields.size() )
    return;
  if ( fields[i-1]->edit != NULL ){
    string parts[3];
    Object2TextParts( input[i], parts, NULL );
    fields[i-1]->edit->setPlainText( parts[1].c_str() );
  }
  FlushUI();
  return;
}

bool Runmod::isLocalRunning()
{
  if ( proc != NULL ){
    if ( proc->state() == QProcess::Running ){
      return true;
    }
  }
  return false;
}

void Runmod::quit()
{
  if ( isLocalRunning() ){
    QMessageBox messageBox;
    messageBox.setWindowTitle( tr("Sub-process still running") );
    messageBox.setText( tr("The sub process is still running. Kill it?") );
    messageBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    messageBox.setDefaultButton( QMessageBox::No );
    if ( messageBox.exec() == QMessageBox::Yes ){
      proc->close();
    }else{
      return;
    }
  }

  qApp->quit();
  return;
}

void Runmod::logClear()
{
  edit_log->setText( "" );

  FlushUI();
  return;
}

void Runmod::logSelectAll()
{
  edit_log->selectAll();
  return;
}

void Runmod::logCopy()
{
  edit_log->copy();
  return;
}
