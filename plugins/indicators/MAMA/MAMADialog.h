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

#ifndef MAMA_DIALOG_HPP
#define MAMA_DIALOG_HPP

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

#include "ColorButton.h"
#include "IndicatorPluginDialog.h"

class MAMADialog : public IndicatorPluginDialog
{
  Q_OBJECT

  public:
    MAMADialog (Indicator &);
    void createGeneralPage ();
    void createMAMAPage ();
    void createFAMAPage ();
    void createOSCPage ();

  public slots:
    void done ();

  private:
    QSpinBox *_period;
    ColorButton *_mamaColor;
    ColorButton *_famaColor;
    ColorButton *_oscColor;
    QComboBox *_mamaPlotStyle;
    QComboBox *_famaPlotStyle;
    QComboBox *_oscPlotStyle;
    QCheckBox *_check;
    QLineEdit *_mamaLabel;
    QLineEdit *_famaLabel;
    QLineEdit *_oscLabel;
    QComboBox *_input;
    QDoubleSpinBox *_fast;
    QDoubleSpinBox *_slow;
};

#endif
