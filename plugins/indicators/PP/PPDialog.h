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

#ifndef PP_DIALOG_HPP
#define PP_DIALOG_HPP

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

#include "ColorButton.h"
#include "IndicatorPluginDialog.h"

class PPDialog : public IndicatorPluginDialog
{
  Q_OBJECT

  public:
    PPDialog (Indicator &);
    void createR1Page ();
    void createR2Page ();
    void createR3Page ();
    void createS1Page ();
    void createS2Page ();
    void createS3Page ();

  public slots:
    void done ();

  private:
    ColorButton *_r1Color;
    ColorButton *_r2Color;
    ColorButton *_r3Color;
    ColorButton *_s1Color;
    ColorButton *_s2Color;
    ColorButton *_s3Color;
    QLineEdit *_r1Label;
    QLineEdit *_r2Label;
    QLineEdit *_r3Label;
    QLineEdit *_s1Label;
    QLineEdit *_s2Label;
    QLineEdit *_s3Label;
    QCheckBox *_r1Check;
    QCheckBox *_r2Check;
    QCheckBox *_r3Check;
    QCheckBox *_s1Check;
    QCheckBox *_s2Check;
    QCheckBox *_s3Check;
};

#endif
