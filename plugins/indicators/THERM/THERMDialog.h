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

#ifndef THERM_DIALOG_HPP
#define THERM_DIALOG_HPP

#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include "ColorButton.h"
#include "IndicatorPluginDialog.h"

class THERMDialog : public IndicatorPluginDialog
{
  Q_OBJECT

  public:
    THERMDialog (Indicator &);
    void createGeneralPage ();
    void createMAPage ();

  public slots:
    void done ();

  private:
    ColorButton *_upColor;
    ColorButton *_downColor;
    ColorButton *_threshColor;
    ColorButton *_maColor;
    QComboBox *_maType;
    QComboBox *_maPlotStyle;
    QDoubleSpinBox *_threshold;
    QSpinBox *_maPeriod;
    QSpinBox *_smoothing;
    QComboBox *_smoothingType;
    QLineEdit *_label;
    QLineEdit *_maLabel;
};

#endif
