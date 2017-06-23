
/*
 * Second project for lecture GUX -- Julia Set.
 * File: MainWindow.cpp
 * Author: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 * Date: 26.11.2016
 */

#include "MainWindow.h"

MainWindow::MainWindow()
{
   QPalette palette;
   palette.setColor(QPalette::Base, Qt::white);

   setWindowTitle(tr("Julia Set - GUX"));

   this->menuBar = new QMenuBar(this);
   setMenuBar(this->menuBar);
   this->fileMenu = new QMenu(tr("File"), this);
   this->menuBar->addMenu(this->fileMenu);

   this->clearAction = new QAction(tr("Clear"), this);
   this->fileMenu->addAction(this->clearAction);
   connect(this->clearAction, SIGNAL(triggered()), this, SLOT(clearActionEvent()));
   this->saveAsImage = new QAction(tr("Save as Image"), this);
   this->fileMenu->addAction(this->saveAsImage);
   connect(this->saveAsImage, SIGNAL(triggered()), this, SLOT(saveAsActionEvent()));
   this->exitAction = new QAction(tr("Exit"), this);
   this->fileMenu->addAction(this->exitAction);
   connect(this->exitAction, SIGNAL(triggered()), this, SLOT(exitActionEvent()));

   this->helpMenu = new QMenu(tr("Help"), this);
   this->menuBar->addMenu(this->helpMenu);
   this->aboutAction = new QAction(tr("About"), this);
   this->helpMenu->addAction(this->aboutAction);
   connect(this->aboutAction, SIGNAL(triggered()), this, SLOT(aboutActionEvent()));

   /**
    * Basic Panels and Widgets
    */
   this->centralWidget = new QWidget(this);
   this->layout = new QHBoxLayout(this->centralWidget);
   this->setScroll = new QScrollArea(this);
   this->setArea = new DrawingArea(this->setScroll);
   this->setArea->resize(500, 500);
   this->setScroll->setWidgetResizable(true);
   this->setScroll->setWidget(this->setArea);

   this->drawButton = new QPushButton(tr("Draw Julia Set"), this->centralWidget);

   this->rightWidget = new QWidget(this->centralWidget);
   this->panelLayout = new QVBoxLayout(this->rightWidget);

   /**
    * Pixmap parameters
    */
   this->pixmapGroupBox = new QGroupBox(tr("Pixmap"), this->centralWidget);
   this->pixmapWidthEdit = new QLineEdit(tr("500"), this->pixmapGroupBox);
   this->pixmapWidthEdit->setPalette(palette);
   this->pixmapHeightEdit = new QLineEdit(tr("500"), this->pixmapGroupBox);
   this->pixmapHeightEdit->setPalette(palette);
   this->automaticSizeBox = new QCheckBox(this->pixmapGroupBox);
   this->automaticSizeBox->setCheckState(Qt::Checked);

   this->pixmapLayout = new QFormLayout(this->pixmapGroupBox);

   this->pixmapLayout->addRow(new QLabel(tr("Pixmap width:"), this->pixmapGroupBox), this->pixmapWidthEdit);
   this->pixmapLayout->addRow(new QLabel(tr("Pixmap height:"), this->pixmapGroupBox), this->pixmapHeightEdit);
   this->pixmapLayout->addRow(new QLabel(tr("Automatic size:"), this->pixmapGroupBox), this->automaticSizeBox);
   this->pixmapGroupBox->setLayout(this->pixmapLayout);

   /**
    * Julia Set parameters.
    */
   this->juliaGroupBox = new QGroupBox(tr("Julia set parameters"), this->centralWidget);
   this->constantCEdit = new QLineEdit(tr("-0.73, 0.19"), this->juliaGroupBox);
   this->constantCEdit->setPalette(palette);
   this->iterEdit = new QLineEdit(tr("200"), this->juliaGroupBox);
   this->iterEdit->setPalette(palette);

   this->juliaLayout = new QFormLayout(this->juliaGroupBox);
   this->juliaLayout->addRow(new QLabel(tr("Constant c:"), this->juliaGroupBox), this->constantCEdit);
   this->juliaLayout->addRow(new QLabel(tr("Iterations:"), this->juliaGroupBox), this->iterEdit);
   this->juliaGroupBox->setLayout(this->juliaLayout);

   /**
    * View parameters
    */
   this->viewGroupBox = new QGroupBox(tr("View parameters"), this->centralWidget);
   this->zoomSlider = new QSlider(Qt::Horizontal, this->viewGroupBox);
   this->zoomSlider->setRange(-9,9);

   this->startColorButton = new QPushButton(tr("Choose color"), this->viewGroupBox);
   QString qss = QString("border: 1px solid black;\n background-color: white;");
   this->startColorButton->setStyleSheet(qss);

   this->finalColorButton = new QPushButton("Choose color", this->viewGroupBox);
   qss = QString("border: 1px solid black;\n background-color: blue;");
   this->finalColorButton->setStyleSheet(qss);

   this->viewLayout = new QFormLayout(this->viewGroupBox);
   this->rainboxBox = new QCheckBox(this->viewGroupBox);
   this->viewLayout->addRow(new QLabel(tr("Zoom:"), this->viewGroupBox), this->zoomSlider);
   this->viewLayout->addRow(new QLabel(tr("Start color:"), this->viewGroupBox), this->startColorButton);
   this->viewLayout->addRow(new QLabel(tr("Final color:"), this->viewGroupBox), this->finalColorButton);
   this->viewLayout->addRow(new QLabel(tr("Rainbow:"), this->viewGroupBox), this->rainboxBox);
   this->viewGroupBox->setLayout(this->viewLayout);

   /**
    * Progress
    */
   this->drawProgressBar = new QProgressBar(this->centralWidget);
   this->drawProgressBar->setMinimum(0);
   this->drawProgressBar->setMaximum(100);

   /**
    * Right panel
    */
   this->panelLayout->addWidget(this->juliaGroupBox);
   this->panelLayout->addWidget(this->viewGroupBox);
   this->panelLayout->addWidget(this->pixmapGroupBox);
   this->panelLayout->addStretch();
   this->panelLayout->addWidget(this->drawProgressBar);
   this->panelLayout->addWidget(this->drawButton);

   this->rightWidget->setLayout(this->panelLayout);
   this->rightWidget->setMinimumWidth(250);

   this->layout->addWidget(this->setScroll);
   this->layout->addWidget(this->rightWidget);
   this->layout->setStretch(0, 3);
   this->layout->setStretch(1, 1);
   this->centralWidget->setLayout(this->layout);

   this->setCentralWidget(this->centralWidget);

   /**
    * Slots and signals
    */
   connect(this->drawButton, SIGNAL (released()), this, SLOT (drawButtonClick()));
   connect(this->startColorButton, SIGNAL (released()), this, SLOT (startColorClick()));
   connect(this->finalColorButton, SIGNAL (released()), this, SLOT (finalColorClick()));
   connect(this->setArea, SIGNAL (valueChanged(int)), this, SLOT (completeChanged(int)));
}

MainWindow::~MainWindow()
{
   delete this->pixmapWidthEdit;
   delete this->pixmapHeightEdit;
   delete this->automaticSizeBox;
   delete this->pixmapLayout;
   delete this->pixmapGroupBox;

   delete this->constantCEdit;
   delete this->iterEdit;
   delete this->juliaLayout;
   delete this->juliaGroupBox;

   /**
    * View parameters
    */
   delete this->zoomSlider;
   delete this->startColorButton;
   delete this->finalColorButton;
   delete this->rainboxBox;
   delete this->viewLayout;
   delete this->viewGroupBox;

   /**
    * Progress bar
    */
   delete this->drawProgressBar;

   delete this->setArea;
   delete this->setScroll;

   delete this->drawButton;
   delete this->layout;

   delete this->panelLayout;
   delete this->rightWidget;

   delete this->centralWidget;

   /**
    * Menu
    */
   delete this->aboutAction;
   delete this->exitAction;
   delete this->clearAction;
   delete this->saveAsImage;
   delete this->fileMenu;
   delete this->helpMenu;
   delete this->menuBar;
}

void MainWindow::exitActionEvent()
{
   close();
}

void MainWindow::clearActionEvent()
{
   this->setArea->Clear();
   this->setArea->update();
}

void MainWindow::saveAsActionEvent()
{
   QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), tr(""), tr("*.bmp"));
   if(!fileName.isNull())
   {
      this->setArea->SavePixmap(fileName);
   }
}

void MainWindow::aboutActionEvent()
{
   QMessageBox::about(this, tr("Help"),
      tr("Author: Vojtech Havlena, xhavle03\n Program demostrates the visualization of Julia Set. The constant c which define the Julia Set is in the format a,b (this represents the complex number a+bi). Other parameters can be set using the GUI."));

}

void MainWindow::startColorClick()
{
   QColor col = QColorDialog::getColor(Qt::white, this);
   if(col.isValid())
   {
      QString qss = QString("border: 1px solid black;\n background-color: %1;").arg(col.name());
      this->startColorButton->setStyleSheet(qss);
      this->setArea->SetStartColor(col);
   }
}

void MainWindow::finalColorClick()
{
   QColor col = QColorDialog::getColor(Qt::white, this);
   if(col.isValid())
   {
      QString qss = QString("border: 1px solid black;\n background-color: %1;").arg(col.name());
      this->finalColorButton->setStyleSheet(qss);
      this->setArea->SetFinalColor(col);
   }
}

void MainWindow::completeChanged(int val)
{
   this->drawProgressBar->setValue(val);
}

void MainWindow::drawButtonClick()
{
   double zoom = 1.0;
   int iter = 0;
   bool ok1 = false, ok2 = false;

   iter = this->iterEdit->text().toInt(&ok1);
   if(!ok1)
   {
      QMessageBox::warning(this, tr("Warning"), tr("Number of iterations must be a positive value."), QMessageBox::Ok);
      return;
   }

   std::istringstream is('(' + this->constantCEdit->text().toStdString() + ')');
   std::complex<float> c;
   is >> c;
   if(is.fail())
   {
      QMessageBox::warning(this, tr("Warning"), tr("Constant c must be in format \"a,b\". Then c = a+bi."), QMessageBox::Ok);
      return;
   }

   if(this->automaticSizeBox->isChecked())
   {
      this->setScroll->setWidgetResizable(true);
      //this->setArea->resize(this->setScroll->width(), this->setScroll->height());
   }
   else
   {
      this->setScroll->setWidgetResizable(false);
      int width = this->pixmapWidthEdit->text().toInt(&ok1);
      int height = this->pixmapHeightEdit->text().toInt(&ok2);
      if(!ok1 || !ok2)
      {
         QMessageBox::warning(this, tr("Warning"), tr("Pixmap size must be a positive value."), QMessageBox::Ok);
         return;
      }

      this->setArea->resize(width, height);
   }


   int zoomVal = this->zoomSlider->value();
   if(zoomVal < 0)
      zoom = 1 + zoomVal * 0.1;
   else
      zoom = 1 + zoomVal;


   this->setArea->SetRainbow(this->rainboxBox->isChecked());
   this->setArea->SetMaxIter(iter);
   this->setArea->SetC(c);
   this->setArea->SetZoom(zoom);
   this->setArea->AdjustPixmapSize();
   this->setArea->DrawPixmap();

   this->setScroll->setWidgetResizable(false);
}
