/*
 * Second project for lecture GUX -- Julia Set.
 * File: DrawingArea.cpp
 * Author: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 * Date: 26.11.2016
 */


#include "DrawingArea.h"

DrawingArea::DrawingArea(QWidget *parent)
   : QWidget(parent)
{
   this->pixHeight = 500;
   this->pixWidth = 500;
   this->zoom = 1;
   this->maxIter = 200;
   this->startColor = Qt::white;
   this->finalColor = Qt::blue;
   this->rainbow = false;
   this->completed = 0;
   this->computing = false;
   this->c = complex<double>(-0.73, 0.19);

   this->pixmap = new QPixmap(this->pixWidth, this->pixHeight);
   this->pixmap->fill(Qt::white);

   QPalette Pal(palette());
   Pal.setColor(QPalette::Background, Qt::white);
   this->setAutoFillBackground(true);
   this->setPalette(Pal);
}

DrawingArea::~DrawingArea()
{
   delete this->pixmap;
}

void DrawingArea::AdjustPixmapSize()
{
   this->pixHeight = this->height();
   this->pixWidth = this->width();
   delete this->pixmap;

   this->pixmap = new QPixmap(this->pixWidth, this->pixHeight);
   this->pixmap->fill(Qt::white);
}

void DrawingArea::paintEvent(QPaintEvent *)
{
   QPainter painter(this);
   if(!this->computing)
   {
      painter.drawPixmap(0,0, *(this->pixmap));
   }
}

void DrawingArea::Clear()
{
   this->pixmap->fill(Qt::white);
}

void DrawingArea::SavePixmap(QString filename)
{
   this->pixmap->save(filename, "BMP");
}

void DrawingArea::DrawPixmap()
{
   QPainter painter(this->pixmap);
   this->drawJuliaSet(painter);
   this->update();
}

void DrawingArea::drawJuliaSet(QPainter & painter)
{
   complex<double> zn;
   double real = 0.0;
   double im = 0.0;

   double rQuot = (this->finalColor.red() - this->startColor.red());
   double gQuot = (this->finalColor.green() - this->startColor.green());
   double bQuot = (this->finalColor.blue() - this->startColor.blue());

   this->SetCompleted(0);
   this->computing = true;

   for(int x = 0; x < this->pixWidth; x++)
   {
      for(int y = 0; y < this->pixHeight; y++)
      {
         real = (x - this->pixWidth / 2.0) / (0.5 * (this->zoom)* this->pixWidth);
         im = (this->pixHeight / 2.0 - y) / (0.5 * (this->zoom) * this->pixHeight);
         zn = complex<double>(real, im);

         int i = 0;
         for(i = 0; i < this->maxIter; i++)
         {
            zn = zn*zn + this->c;
            if(std::abs(zn) > 2.0)
            {
               break;
            }
         }

         int r = this->startColor.red() + (i/(double)this->maxIter)* rQuot;
         int g = this->startColor.green() + (i/(double)this->maxIter) * gQuot;
         int b = this->startColor.blue() + (i/(double)this->maxIter) * bQuot;

         QColor act(r, g, b);
         if(this->rainbow)
         {
            act.setHsv(this->startColor.hue() + i/(double)this->maxIter * ( this->finalColor.hue() - this->startColor.hue()), 255, 255);
         }
         painter.setPen(QPen(act));

         painter.drawPoint(x,y);
      }
      this->SetCompleted((100.0/this->pixWidth)*x);
   }
   this->computing = false;
   this->SetCompleted(100);
}
