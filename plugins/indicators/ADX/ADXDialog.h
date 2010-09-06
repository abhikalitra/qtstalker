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

#ifndef ADX_DIALOG_HPP
#define ADX_DIALOG_HPP

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QString>
#include <QCheckBox>

#include "ColorButton.h"
#include "IndicatorPluginDialog.h"

class ADXDialog : public IndicatorPluginDialog
{
  Q_OBJECT

  public:
    ADXDialog (Indicator &);
    void createGeneralPage ();
    void createMDIPage ();
    void createPDIPage ();
    void createADXPage ();
    void createADXRPage ();

  public slots:
    void done ();

  private:
    QSpinBox *_period;
    ColorButton *_mdiColor;
    ColorButton *_pdiColor;
    ColorButton *_adxColor;
    ColorButton *_adxrColor;
    QComboBox *_mdiPlotStyle;
    QComboBox *_pdiPlotStyle;
    QComboBox *_adxPlotStyle;
    QComboBox *_adxrPlotStyle;
    QLineEdit *_mdiLabel;
    QLineEdit *_pdiLabel;
    QLineEdit *_adxLabel;
    QLineEdit *_adxrLabel;
    QCheckBox *_mdiShow;
    QCheckBox *_pdiShow;
    QCheckBox *_adxShow;
    QCheckBox *_adxrShow;
};

#endif
