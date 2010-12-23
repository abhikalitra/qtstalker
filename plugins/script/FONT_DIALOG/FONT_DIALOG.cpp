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

#include "FONT_DIALOG.h"

#include <QtDebug>
#include <QFontDialog>

FONT_DIALOG::FONT_DIALOG ()
{
  _type = _DIALOG;
}

int FONT_DIALOG::command (Command *command)
{
  // FONT_DIALOG,FONT
  //      0       1

  _command = command;
  QFont font;
  if (command->count() == 2)
  {
    QStringList l = command->parm(1).split(";");
    font.fromString(l.join(","));
  }

  QFontDialog *dialog = new QFontDialog(font, 0);
  connect(dialog, SIGNAL(fontSelected(const QFont &)), this, SLOT(fontSelected(QFont)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(rejected()), this, SIGNAL(signalResume()));
  dialog->show();

  return 0;
}

void FONT_DIALOG::fontSelected (QFont font)
{
  QStringList l = font.toString().split(",", QString::SkipEmptyParts);
  _command->setReturnData(l.join(";"));
  emit signalResume();
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  FONT_DIALOG *o = new FONT_DIALOG;
  return ((ScriptPlugin *) o);
}
