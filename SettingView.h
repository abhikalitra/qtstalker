/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#ifndef SETTINGVIEW_HPP
#define SETTINGVIEW_HPP

#include <qdialog.h>
#include <qwidget.h>
#include <qtable.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qspinbox.h>

#include "Setting.h"
#include "Navigator.h"

class ColorItem : public QTableItem
{
  public:
    ColorItem (QTable *, EditType, QString);
    ~ColorItem ();
    void paint (QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );
    void setColor (QString);
    QString getColor ();

  private:
    QString color;
};

class IntegerItem : public QTableItem
{
  public:
    IntegerItem (QTable *, EditType, int);
    ~IntegerItem ();
    QWidget * createEditor () const;
    QString text () const;

  private:
    QSpinBox *spinner;
    int val;
};

class DateItem : public QTableItem
{
  public:
    DateItem (QTable *, EditType, QString);
    ~DateItem ();
    QWidget * createEditor () const;
    QString text () const;

  private:
    QDateEdit *dateEdit;
    QString startDate;
};

class SettingView : public QWidget
{
  Q_OBJECT

  public:
    SettingView (QWidget *, QString);
    ~SettingView ();
    void setItems (Setting *);
    void makeSettings ();
    void colorDialog (int);
    void floatChanged (int);
    void fileDialog (int);
    void symbolDialog (int);
    void fontDialog (int);
    void textChanged (int);
    void clear ();
    void clearRows ();
    void updateSettings ();

  public slots:
    void doubleClick (int, int, int, const QPoint &);
    void itemChanged (int, int);

  protected:
    QString dataPath;
    Setting *settings;
    QTable *list;
};

class SymbolDialog : public QDialog
{
  Q_OBJECT

  public:
    SymbolDialog (QString);
    ~SymbolDialog ();
    QString getSymbol ();
    
  public slots:
    void checkDoubleClick (QString);
    void upDirectory ();

  private:
    QGridLayout *toolbar;
    QToolButton *okButton;
    QToolButton *cancelButton;
    QToolButton *upButton;
    Navigator *navigator;
};

#endif

