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

#ifndef PLUGIN_STOCH_FAST_DIALOG_HPP
#define PLUGIN_STOCH_FAST_DIALOG_HPP

#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTabWidget>
#include <QLineEdit>

#include "ColorButton.h"
#include "Setting.h"

class STOCH_FASTDialog : public QWidget
{
  Q_OBJECT

  public:
    STOCH_FASTDialog (QWidget *, Setting *);
    void createKPage ();
    void createDPage ();
    void createRefPage ();
    void createRef2Page ();

  public slots:
    void save ();

  private:
    Setting *_settings;
    QTabWidget *_tabs;
    QSpinBox *_kperiod;
    QSpinBox *_dperiod;
    ColorButton *_kcolor;
    ColorButton *_dcolor;
    ColorButton *_refColor;
    ColorButton *_ref2Color;
    QComboBox *_kstyle;
    QComboBox *_dstyle;
    QComboBox *_maType;
    QDoubleSpinBox *_ref;
    QDoubleSpinBox *_ref2;
    QLineEdit *_koutput;
    QSpinBox *_zk;
    QLineEdit *_doutput;
    QSpinBox *_zd;
};

#endif