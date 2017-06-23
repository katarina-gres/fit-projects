/*
 * Second project for lecture GUX -- Julia Set.
 * File: JuliaSet.cpp
 * Author: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 * Date: 26.11.2016
 */

#include <QApplication>

#include "MainWindow.h"

int main(int argv, char **args)
{
   QApplication app(argv, args);

   MainWindow window;
   window.show();

   return app.exec();
}
