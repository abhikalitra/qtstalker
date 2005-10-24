/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include "MyComboBox.h"
#include "MySpinBox.h"
#include "MyLineEdit.h"
#include "MySlider.h"
#include "Macro.h"


class ChartToolbar : public QToolBar
{
  Q_OBJECT
  
  signals:
    void signalCompressionChanged (int);
    void signalChartTypeChanged ();
    void signalPixelspaceChanged (int);
    void signalSliderChanged (int);
    void signalKeyPressed (int, int, int, int, QString);
    void signalBarsChanged (int);
    
  public:
  
    enum MenuAction
    {
      ToolbarFocus,
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
    void runMacro (Macro *);
    void barsChanged ();
  
  private:
    MyComboBox *compressionCombo;
    MyComboBox *chartTypeCombo;
    MySpinBox *pixelspace;
    MyLineEdit *barCount;
    MySlider *slider;
    bool keyFlag;
    MenuAction focusFlag;
    Macro *macro;
    bool macroFlag;
};

#endif
