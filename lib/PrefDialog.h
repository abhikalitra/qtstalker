/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#ifndef PREFDIALOG_HPP
#define PREFDIALOG_HPP

#include <qtabdialog.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qdict.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcheckbox.h>
#include <qfont.h>
#include <qcombobox.h>

class ColorButton : public QPushButton
{
  Q_OBJECT

  public:
    ColorButton (QWidget *, QColor);
    ~ColorButton ();
    QColor getColor ();
    void setColorButton ();

  public slots:
    void colorDialog ();
        
  private:
    QColor color;
    QPixmap pix;
};

class FontButton : public QPushButton
{
  Q_OBJECT

  public:
    FontButton (QWidget *, QFont);
    ~FontButton ();
    QFont getFont ();
    void setFontButton ();

  public slots:
    void fontDialog ();
        
  private:
    QFont font;
};

class PrefDialog : public QTabDialog
{
  Q_OBJECT

  public:
    PrefDialog ();
    ~PrefDialog ();
    void createPage (QString);
    void addColorItem (QString, int, QColor);
    QColor getColor (QString);
    void addFloatItem (QString, int, double);
    double getFloat (QString);
    void addIntItem (QString, int, int);
    int getInt (QString);
    void addCheckItem (QString, int, bool);
    bool getCheck (QString);
    void addFontItem (QString, int, QFont);
    QFont getFont (QString);
    void addTextItem (QString, int, QString);
    QString getText (QString);
    void addComboItem (QString, int, QStringList);
    QString getCombo (QString);

  public slots:

  private:
    QDoubleValidator *dv;
    QList<QWidget> widgetList;
    QList<QGridLayout> gridList;
    QDict<ColorButton> colorButtonList;
    QDict<QSpinBox> intList;
    QDict<QLineEdit> floatList;
    QDict<QCheckBox> checkList;
    QDict<FontButton> fontButtonList;
    QDict<QLineEdit> textList;
    QDict<QComboBox> comboList;
};

#endif
