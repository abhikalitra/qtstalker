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

#ifndef PREF_DIALOG_HPP
#define PREF_DIALOG_HPP

#include "ColorButton.h"
#include "FontButton.h"
#include "Indicator.h"
#include "FileButton.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QLayout>
#include <QWidget>
#include <QPushButton>
#include <QHash>
#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QFont>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QStringList>
#include <QString>
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <QTextEdit>

class PrefDialog : public QDialog
{
  Q_OBJECT

  public:
    PrefDialog (QWidget *);
    PrefDialog ();
    void init ();
    void addPage (int page, QString &title);

    void addColorItem (int key, int page, QString name, QString &color);
    void addColorItem (int key, int page, QString, QColor &);
    void getColor (int key, QColor &);
    void addDoubleItem (int key, int page, QString, double, double, double);
    void addDoubleItem (int key, int page, QString, double);
    double getDouble (int key);
    void addIntItem (int key, int page, QString, int);
    void addIntItem (int key, int page, QString, int, int, int);
    int getInt (int key);
    void addCheckItem (int key, int page, QString, int);
    void addCheckItem (int key, int page, QString, QString &);
    int getCheck (int key);
    void getCheckString (int key, QString &);
    void addFontItem (int key, int page, QString, QFont &);
    void getFont (int key, QFont &);
    void addTextItem (int key, int page, QString, QString &);
    void getText (int key, QString &);
    void addComboItem (int key, int page, QString, QStringList &, QString &);
    void addComboItem (int key, int page, QString, QStringList &, int);
    void getCombo (int key, QString &);
    int getComboIndex (int key);
    QComboBox * getComboWidget (int key);
    void addDateItem (int key, int page, QString, QDateTime &);
    void getDate (int key, QDateTime &);
    void addTimeItem (int key, int page, QString, QDateTime &);
    void getTime (int key, QDateTime &);
    void getItem (int key, QString &);
    void getKeyList (QList<int> &);
    void addTextEditItem (int key, int page, QString, QString &);
    void getTextEdit (int key, QString &);
    void addFileItem (int key, int page, QString, QString &);
    void getFile (int key, QString &);

//  public slots:

  private:
    QHash<int, ColorButton *> colorButtonList;
    QHash<int, QSpinBox *> intList;
    QHash<int, QDoubleSpinBox *> doubleList;
    QHash<int, QCheckBox *> checkList;
    QHash<int, FontButton *> fontButtonList;
    QHash<int, QLineEdit *> textList;
    QHash<int, QComboBox *> comboList;
    QHash<int, QDateEdit *> dateList;
    QHash<int, QLabel *> labelList;
    QHash<int, QTimeEdit *> timeList;
    QHash<int, QTextEdit *> textEditList;
    QHash<int, QGridLayout *> gridList;
    QHash<int, FileButton *> fileButtonList;
    QDialogButtonBox *buttonBox;
    QGridLayout *grid;
    QTabWidget *tabs;
    QList<int> keys;
};

#endif
