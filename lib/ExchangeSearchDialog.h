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

#ifndef EXCHANGE_SEARCH_DIALOG_HPP
#define EXCHANGE_SEARCH_DIALOG_HPP

#include <QListWidget>
#include <QComboBox>

#include "ExchangeDataBase.h"
#include "Dialog.h"

class ExchangeSearchDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalDone (QString);

  public:
    ExchangeSearchDialog (QWidget *);
    void createGUI ();
    void loadSettings ();
    void saveSettings ();
    QString exchange ();

  public slots:
    void done ();
    void selectionChanged ();
    void search ();

  private:
    ExchangeDataBase _db;
    QListWidget *_list;
    QComboBox *_country;
    QComboBox *_city;
    QComboBox *_exchange;
};

#endif