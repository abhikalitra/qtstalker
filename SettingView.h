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
#include <qlistview.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qlayout.h>
#include <qtoolbutton.h>

#include "Setting.h"
#include "Navigator.h"

class SettingView : public QWidget
{
  Q_OBJECT

  public:
    SettingView (QWidget *, QString);
    ~SettingView ();
    void setItems (Setting *);
    void makeSettings ();
    void colorDialog ();
    void dateDialog ();
    void textDialog ();
    void intDialog ();
    void floatDialog ();
    void fileDialog ();
    void symbolDialog ();
    void checkDialog ();
    void listDialog ();
    QList<QListViewItem> getList();
    void clear ();

  public slots:
    void cellSelected (QListViewItem *);

  protected:
    QString dataPath;
    Setting *settings;
    QListView *list;
    QListViewItem *item;
};

class DateDialog : public QDialog
{
  Q_OBJECT

  public:
    DateDialog ();
    ~DateDialog ();
    void setDate (QDate);
    QDate getDate ();

  private:
    QDateEdit *date;
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
