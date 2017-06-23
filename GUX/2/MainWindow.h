
/*
 * Second project for lecture GUX -- Julia Set.
 * File: MainWindow.h
 * Author: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 * Date: 26.11.2016
 */

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMainWindow>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QPalette>
#include <QScrollArea>
#include <QSlider>
#include <QColorDialog>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>

#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include <sstream>
#include <iostream>

#include "DrawingArea.h"

class MainWindow : public QMainWindow
{
   Q_OBJECT

private:
   DrawingArea *setArea;
   QScrollArea *setScroll;

   QPushButton *drawButton;
   QHBoxLayout *layout;
   QWidget *centralWidget;

   QLineEdit * pixmapHeightEdit;
   QLineEdit * pixmapWidthEdit;
   QFormLayout * pixmapLayout;
   QVBoxLayout *panelLayout;
   QGroupBox *pixmapGroupBox;
   QCheckBox * automaticSizeBox;
   QWidget * rightWidget;

   QFormLayout * juliaLayout;
   QGroupBox *juliaGroupBox;
   QLineEdit * constantCEdit;
   QLineEdit * iterEdit;

   QFormLayout *viewLayout;
   QGroupBox *viewGroupBox;
   QSlider * zoomSlider;
   QPushButton * startColorButton;
   QPushButton *finalColorButton;
   QCheckBox * rainboxBox;
   QProgressBar *drawProgressBar;

   /**
    * Menu
    */
   QMenuBar * menuBar;
   QMenu* fileMenu;
   QMenu* helpMenu;
   QAction * saveAsImage;
   QAction * clearAction;
   QAction * exitAction;
   QAction *aboutAction;

private slots:
    void drawButtonClick();
    void startColorClick();
    void finalColorClick();
    void completeChanged(int val);

    void exitActionEvent();
    void clearActionEvent();
    void saveAsActionEvent();
    void aboutActionEvent();

public:
   MainWindow();
   ~MainWindow();

};
