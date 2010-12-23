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

#include "COLOR_DIALOG.h"

#include <QtDebug>
#include <QColorDialog>

COLOR_DIALOG::COLOR_DIALOG ()
{
  _type = _DIALOG;
}

int COLOR_DIALOG::command (Command *command)
{
  // COLOR_DIALOG,<COLOR>
  //      0          1

  _command = command;
  QColor c(Qt::white);
  if (command->count() == 2)
  {
    c.setNamedColor(command->parm(1));
    if (! c.isValid())
    {
      qDebug() << "COLOR_DIALOG::command: invalid color" << command->parm(1);
      return 1;
    }
  }

  QColorDialog *dialog = new QColorDialog(c, 0);
  connect(dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(colorSelected(QColor)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(rejected()), this, SIGNAL(signalResume()));
  dialog->show();

  return 0;
}

void COLOR_DIALOG::colorSelected (QColor c)
{
  _command->setReturnData(c.name());
  emit signalResume();
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  COLOR_DIALOG *o = new COLOR_DIALOG;
  return ((ScriptPlugin *) o);
}
