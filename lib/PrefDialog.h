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

#include "ColorButton.h"
#include "FontButton.h"
#include "FileButton.h"
#include "SymbolButton.h"
#include <qtabdialog.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qdict.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfont.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qstring.h>

class PrefDialog : public QTabDialog
{
  public:
    PrefDialog ();
    ~PrefDialog ();
    void createPage (QString);
    void addColorItem (QString, int, QColor);
    QColor getColor (QString);
    void addFloatItem (QString, int, double, double, double);
    void addFloatItem (QString, int, double);
    double getFloat (QString);
    void addIntItem (QString, int, int);
    void addIntItem (QString, int, int, int, int);
    int getInt (QString);
    void addCheckItem (QString, int, bool);
    void addCheckItem (QString, int, QString);
    bool getCheck (QString);
    QString getCheckString (QString);
    void addFontItem (QString, int, QFont);
    QFont getFont (QString);
    void addTextItem (QString, int, QString);
    QString getText (QString);
    void addComboItem (QString, int, QStringList, QString);
    QString getCombo (QString);
    void addDateItem (QString, int, QDateTime);
    QDateTime getDate (QString);
    void addFileItem (QString, int);
    QStringList getFile (QString);
    void addSymbolItem (QString, int, QString, QString);
    QString getSymbol (QString);

  private:
    QList<QWidget> widgetList;
    QList<QGridLayout> gridList;
    QDict<ColorButton> colorButtonList;
    QDict<QSpinBox> intList;
    QDict<QLineEdit> floatList;
    QDict<QCheckBox> checkList;
    QDict<FontButton> fontButtonList;
    QDict<QLineEdit> textList;
    QDict<QComboBox> comboList;
    QDict<QDateEdit> dateList;
    QDict<FileButton> fileList;
    QDict<SymbolButton> symbolList;
    QDict<QDoubleValidator> dvList;
};

#endif
