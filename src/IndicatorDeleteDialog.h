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

#ifndef INDICATOR_DELETE_DIALOG_HPP
#define INDICATOR_DELETE_DIALOG_HPP

#include <QListWidget>

#include "Dialog.h"
#include "IndicatorDataBase.h"
#include "Indicator.h"

class IndicatorDeleteDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalDelete (Indicator);
  
  public:
    IndicatorDeleteDialog ();
    void createMainPage ();

  public slots:
    void done ();
    void done2 ();

  private:
    QListWidget *_list;
    IndicatorDataBase _db;
};

#endif
