#
# Second project for lecture GUX -- Julia Set.
# File: JuliaSet.pro
# Author: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
# Date: 26.11.2016
#

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JuliaSet
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -Wall

SOURCES += JuliaSet.cpp\
            DrawingArea.cpp\
            MainWindow.cpp

HEADERS  += DrawingArea.h\
            MainWindow.h
