/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#ifndef PLUGIN_OHLC_DIALOG_HPP
#define PLUGIN_OHLC_DIALOG_HPP

#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QTabWidget>

#include "Dialog.h"
#include "ColorButton.h"
#include "Setting.h"

class OHLCDialog : public Dialog
{
  Q_OBJECT

  public:
    OHLCDialog (QWidget *, Setting *);
    void createGeneralPage ();
    void createMA1Page ();
    void createMA2Page ();
    void createMA3Page ();

  public slots:
    void done ();

  private:
    QStringList _styleList;
    Setting *_settings;
    QTabWidget *_tabs;
    ColorButton *_upColor;
    ColorButton *_downColor;
    ColorButton *_neutralColor;
    QComboBox *_style;
    QSpinBox *_ma1Period;
    ColorButton *_ma1Color;
    QComboBox *_ma1Style;
    QComboBox *_ma1Type;
    QComboBox *_ma1Input;
    QGroupBox *_ma1Check;
    QSpinBox *_ma2Period;
    ColorButton *_ma2Color;
    QComboBox *_ma2Style;
    QComboBox *_ma2Type;
    QComboBox *_ma2Input;
    QGroupBox *_ma2Check;
    QSpinBox *_ma3Period;
    ColorButton *_ma3Color;
    QComboBox *_ma3Style;
    QComboBox *_ma3Type;
    QComboBox *_ma3Input;
    QGroupBox *_ma3Check;
};

#endif
