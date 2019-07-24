#ifndef Notepad_GuisModule_EVAWIZ_H
#define Notepad_GuisModule_EVAWIZ_H

class Notepad: public QMainWindow
{
  Q_OBJECT

public:
  Notepad();
private slots:
  void open();
  void save();
  void quit();
private:
  QTextEdit *textEdit;

  QAction   *openAction;
  QAction   *saveAction;
  QAction   *exitAction;

  QMenu     *fileMenu;
};

#endif
