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
#include "FormulaInput.h"
#include <qtabdialog.h>
#include <qlayout.h>
#include <qwidget.h>
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
#include <qlabel.h>

class PrefDialog : public QTabDialog
{
  Q_OBJECT

  public:
    PrefDialog (QWidget *);
    PrefDialog ();
    ~PrefDialog ();
    void init ();
    void createPage (QString &);
    void deletePage (QString &);
    void setHelpFile (QString &);
    void addColorItem (QString &, QString &, QColor &);
    QColor getColor (QString &);
    void addFloatItem (QString &, QString &, double, double, double);
    void addFloatItem (QString &, QString &, double);
    double getFloat (QString &);
    void addIntItem (QString &, QString &, int);
    void addIntItem (QString &, QString &, int, int, int);
    int getInt (QString &);
    void addCheckItem (QString &, QString &, bool);
    void addCheckItem (QString &, QString &, QString &);
    bool getCheck (QString &);
    QString getCheckString (QString &);
    void addFontItem (QString &, QString &, QFont &);
    QFont getFont (QString &);
    void addTextItem (QString &, QString &, QString &);
    QString getText (QString &);
    void addComboItem (QString &, QString &, QStringList &, QString &);
    void addComboItem (QString &, QString &, QStringList &, int);
    QString getCombo (QString &);
    int getComboIndex (QString &);
    QComboBox * getComboWidget (QString &);
    void addDateItem (QString &, QString &, QDateTime &);
    QDateTime getDate (QString &);
    void addFileItem (QString &, QString &, QStringList &, QString &);
    QStringList getFile (QString &);
    void addSymbolItem (QString &, QString &, QString &, QString &);
    QString getSymbol (QString &);
    void addFormulaInputItem (QString &, QString &, bool, QString &);
    QString getFormulaInput (QString &);
    void addLabelItem (QString &, QString &, QString &);
    
  public slots:
    void help ();

  private:
    QDict<QWidget> widgetList;
    QDict<QGridLayout> gridList;
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
    QDict<FormulaInput> formulaInputList;
    QDict<QLabel> labelList;
    QString helpFile;
};

#endif
