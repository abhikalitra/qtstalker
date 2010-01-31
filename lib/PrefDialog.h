/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include "Indicator.h"

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


class PrefDialog : public QDialog
{
  Q_OBJECT

  public:
    PrefDialog (QWidget *);
    PrefDialog ();
    void init ();
    void addPage (int page, QString &title);
    void addColorItem (int page, QString &name, QString &color);
    void addColorItem (int page, QString &, QColor &);
    void addColorPrefItem (int page, QString &, QColor &);
    void getColor (QString &, QColor &);
    void addDoubleItem (int page, QString &, double, double, double);
    void addDoubleItem (int page, QString &, double);
    double getDouble (QString &);
    void addIntItem (int page, QString &, int);
    void addIntItem (int page, QString &, int, int, int);
    int getInt (QString &);
    void addCheckItem (int page, QString &, bool);
    void addCheckItem (int page, QString &, QString &);
    bool getCheck (QString &);
    void getCheckString (QString &, QString &);
    void addFontItem (int page, QString &, QFont &);
    void getFont (QString &, QFont &);
    void addTextItem (int page, QString &, QString &);
    void getText (QString &, QString &);
    void addComboItem (int page, QString &, QStringList &, QString &);
    void addComboItem (int page, QString &, QStringList &, int);
    void getCombo (QString &, QString &);
    int getComboIndex (QString &);
    QComboBox * getComboWidget (QString &);
    void addDateItem (int page, QString &, QDateTime &);
    void getDate (QString &, QDateTime &);
    void getItem (QString &, QString &);
    void addTimeItem (int page, QString &, QDateTime &);
    void getTime (QString &, QDateTime &);


  public slots:

  private:
    QHash<QString, ColorButton *> colorButtonList;
    QHash<QString, QSpinBox *> intList;
    QHash<QString, QDoubleSpinBox *> doubleList;
    QHash<QString, QCheckBox *> checkList;
    QHash<QString, FontButton *> fontButtonList;
    QHash<QString, QLineEdit *> textList;
    QHash<QString, QComboBox *> comboList;
    QHash<QString, QDateEdit *> dateList;
    QHash<QString, QLabel *> labelList;
    QHash<QString, QTimeEdit *> timeList;
    QHash<int, QGridLayout *> gridList;
    QDialogButtonBox *buttonBox;
    QGridLayout *grid;
    QTabWidget *tabs;
};

#endif
