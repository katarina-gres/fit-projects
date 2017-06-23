
/*
 * Second project for lecture GUX -- Julia Set.
 * File: DrawingArea.h
 * Author: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 * Date: 26.11.2016
 */

#include <QtGui>
#include <QWidget>
#include <complex>
#include <QPainter>

using std::complex;

class DrawingArea : public QWidget
{
   Q_OBJECT
   Q_PROPERTY(int value READ GetCompleted WRITE SetCompleted NOTIFY valueChanged)

public:
   DrawingArea(QWidget *parent = 0);
   ~DrawingArea();

   void DrawPixmap();
   void AdjustPixmapSize();

   void SetZoom(double zoom) { this->zoom = zoom; }
   void SetMaxIter(int iter) { this->maxIter = iter; }
   void SetC(complex<double> c) { this->c = c; }
   void SetStartColor(QColor c) { this->startColor = c; }
   void SetFinalColor(QColor c) { this->finalColor = c; }
   void SetRainbow(bool val) { this->rainbow = val; }

   int GetCompleted() const { return this->completed; }
   void Clear();
   void SavePixmap(QString filename);

public slots:
   void SetCompleted(int value)
   {
      if(this->completed != value)
      {
         this->completed = value;
         emit valueChanged(this->completed);
      }
   }

signals:
   void valueChanged(int value);

private:
   QPixmap * pixmap;

   int pixWidth;
   int pixHeight;
   double zoom;
   complex<double> c;
   int maxIter;
   QColor startColor;
   QColor finalColor;
   bool rainbow;

   int completed;
   bool computing;


protected:
   void paintEvent(QPaintEvent *);
   void drawJuliaSet(QPainter & painter);

};
