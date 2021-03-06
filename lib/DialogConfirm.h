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

#ifndef DIALOG_CONFIRM_HPP
#define DIALOG_CONFIRM_HPP

#include <QTextEdit>

#include "Dialog.h"

class DialogConfirm : public Dialog
{
  Q_OBJECT

  signals:
    void signalDone (int);

  public:
    DialogConfirm (QWidget *, QString id, Entity);
    void createGUI ();
    void setMessage (QString);

  public slots:
    void done ();
    
  private:
    QTextEdit *_text;
};

#endif
