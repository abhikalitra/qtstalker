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

#ifndef EDITDIALOG_HPP
#define EDITDIALOG_HPP

#include <qdialog.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qlayout.h>
#include <qdatetimeedit.h>
#include <qtable.h>
#include <qvalidator.h>

#include "Setting.h"
#include "Config.h"

class EditDialog : public QDialog
{
  Q_OBJECT

  public:
    EditDialog (Config *);
    ~EditDialog ();
    void setItems (Setting *);
    void addColorButton ();
    void dateDialog (int);
    void addCheck ();
    void symbolDialog (int);
    void colorDialog (int);
    void addList ();
    void compositeDialog (int);
    void fileDialog (int);

  public slots:
    void cellSelected (int, int);
    void valueChanged (int, int);

  protected:
    QTable *table;
    Config *config;
    Setting *settings;
    QVBoxLayout *baseBox;
    QVBoxLayout *topBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGridLayout *grid;
    SettingItem *settingItem;
    QIntValidator *ivalid;
    QDoubleValidator *dvalid;
};

class DateDialog : public QDialog
{
  Q_OBJECT

  public:
    DateDialog ();
    ~DateDialog ();
    void setDate (QDateTime);
    QDateTime getDate ();

  private:
    QDateTimeEdit *date;
};

#endif
