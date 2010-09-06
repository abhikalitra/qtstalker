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

#ifndef MACD_DIALOG_HPP
#define MACD_DIALOG_HPP

#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include "ColorButton.h"
#include "IndicatorPluginDialog.h"

class MACDDialog : public IndicatorPluginDialog
{
  Q_OBJECT

  public:
    MACDDialog (Indicator &);
    void createGeneralPage ();
    void createMACDPage ();
    void createSignalPage ();
    void createHistPage ();

  public slots:
    void done ();

  private:
    QComboBox *_input;
    ColorButton *_macdColor;
    ColorButton *_signalColor;
    ColorButton *_histColor;
    QComboBox *_fastMAType;
    QComboBox *_slowMAType;
    QComboBox *_macdPlotStyle;
    QComboBox *_signalPlotStyle;
    QComboBox *_histPlotStyle;
    QComboBox *_signalMAType;
    QSpinBox *_fast;
    QSpinBox *_slow;
    QSpinBox *_signalPeriod;
    QLineEdit *_macdLabel;
    QLineEdit *_signalLabel;
    QLineEdit *_histLabel;
    QDoubleSpinBox *_upDev;
};

#endif
