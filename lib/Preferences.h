/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <qcolor.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qtabdialog.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include "ColorButton.h"
#include "FontButton.h"


class Preferences : public QTabDialog
{
  Q_OBJECT

  signals:
    void signalMenubar (bool);
    void signalBackgroundColor (QColor);
    void signalBorderColor (QColor);
    void signalGridColor (QColor);
    void signalPlotFont (QFont);
    void signalAppFont (QFont);
    void signalLoadChart ();

  public:
    Preferences (QWidget *);
    ~Preferences ();
    void createGeneralPage ();
    void createColorPage ();
    void createFontPage ();
    void loadSettings ();

  public slots:
    void slotHelp ();
    void slotSave ();

  private:
    QString helpFile;
    bool menubar;
    int ps1Button;
    int ps2Button;
    int ps3Button;
    QColor backgroundColor;
    QColor borderColor;
    QColor gridColor;
    QFont plotFont;
    QFont appFont;
    QCheckBox *menubarCheck;
    QSpinBox *bs1Spinner;
    QSpinBox *bs2Spinner;
    QSpinBox *bs3Spinner;
    ColorButton *backgroundColorButton;
    ColorButton *borderColorButton;
    ColorButton *gridColorButton;
    FontButton *plotFontButton;
    FontButton *appFontButton;
};

#endif
