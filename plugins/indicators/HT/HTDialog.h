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

#ifndef HT_DIALOG_HPP
#define HT_DIALOG_HPP

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

#include "ColorButton.h"
#include "IndicatorPluginDialog.h"

class HTDialog : public IndicatorPluginDialog
{
  Q_OBJECT

  public:
    HTDialog (Indicator &);
    void createGeneralPage ();
    void createPhasorPage ();
    void createSinePage ();

  public slots:
    void done ();

  private:
    ColorButton *_color;
    ColorButton *_phaseColor;
    ColorButton *_quadColor;
    ColorButton *_sineColor;
    ColorButton *_leadColor;
    QComboBox *_plotStyle;
    QComboBox *_phasePlotStyle;
    QComboBox *_quadPlotStyle;
    QComboBox *_sinePlotStyle;
    QComboBox *_leadPlotStyle;
    QComboBox *_input;
    QComboBox *_method;
    QLineEdit *_label;
    QLineEdit *_phaseLabel;
    QLineEdit *_quadLabel;
    QLineEdit *_sineLabel;
    QLineEdit *_leadLabel;
};

#endif
