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

#ifndef DIALOG_SELECT_HPP
#define DIALOG_SELECT_HPP

#include <QListWidget>
#include <QStringList>

#include "Dialog.h"

class DialogSelect : public Dialog
{
  Q_OBJECT

  signals:
    void signalDone (QStringList);

  public:
    DialogSelect (QWidget *, QString id, Entity);
    void createGUI ();
    void setItems (QStringList);
    void setMode (int);
    void setTitle (QString);
    QStringList selected ();

  public slots:
    void done ();
    void selectionChanged ();

  private:
    QListWidget *_list;
    QLabel *_title;
    QStringList _selected;
};

#endif
