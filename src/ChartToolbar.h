/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
 *  USA.
 */

#ifndef CHARTTOOLBAR_HPP
#define CHARTTOOLBAR_HPP

#include <qstring.h>
#include <qtoolbar.h>
#include <qmainwindow.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qslider.h>


class MyCombo : public QComboBox
{
  Q_OBJECT
  
  signals:
    void signalKeyPressed (int, int, int, int, QString);
  
  public:
    MyCombo (QWidget *);
    ~MyCombo ();
    void setKeyFlag (bool);   
    void doKeyPress (QKeyEvent *); 
    
  protected:
    virtual void keyPressEvent (QKeyEvent *);
    
  private:
    bool keyFlag;
};

class MySpinner : public QSpinBox
{
  Q_OBJECT
  
  signals:
    void signalKeyPressed (int, int, int, int, QString);
  
  public:
    MySpinner (QWidget *);
    ~MySpinner ();
    void setKeyFlag (bool);   
    void doKeyPress (QKeyEvent *); 
    
  protected:
    virtual void keyPressEvent (QKeyEvent *);
    
  private:
    bool keyFlag;
};

class MySlider : public QSlider
{
  Q_OBJECT
  
  signals:
    void signalKeyPressed (int, int, int, int, QString);
  
  public:
    MySlider (QWidget *);
    ~MySlider ();
    void setKeyFlag (bool);   
    void doKeyPress (QKeyEvent *); 
    
  protected:
    virtual void keyPressEvent (QKeyEvent *);
    
  private:
    bool keyFlag;
};


class ChartToolbar : public QToolBar
{
  Q_OBJECT
  
  signals:
    void signalCompressionChanged (int);
    void signalChartTypeChanged (int);
    void signalPixelspaceChanged (int);
    void signalSliderChanged (int);
    void signalKeyPressed (int, int, int, int, QString);
    
  public:
  
    enum MenuAction
    {
      CompressionFocus,
      ChartTypeFocus,
      BarSpacingFocus,
      BarsLoadedFocus,
      ChartPannerFocus
    };
  
    ChartToolbar(QMainWindow *);
    ~ChartToolbar();
    int getBars ();
    void enableSlider (bool);
    void setMinPixelspace (int);
    void setPixelspace (int, int);
    int getPixelspace ();
    int getCompressionInt ();
    QString getCompression ();
    QString getChartType ();
    int getSlider ();
    int setSliderStart (int ov, bool flag, int width, int records);
    void saveSettings ();
    
  public slots:
    void setFocus ();
    void setKeyFlag (bool);
    void slotAccel (int);
    void doKeyPress (QKeyEvent *);
  
  private:
    MyCombo *compressionCombo;
    MyCombo *chartTypeCombo;
    MySpinner *pixelspace;
    MySpinner *barCount;
    MySlider *slider;
    bool keyFlag;
    MenuAction focusFlag;
};

#endif
