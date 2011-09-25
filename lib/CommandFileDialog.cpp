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

#include "CommandFileDialog.h"
#include "Script.h"

#include <QtDebug>
#include <QFileDialog>

CommandFileDialog::CommandFileDialog (QObject *p) : Command (p)
{
  _type = "FILE_DIALOG";
}

int CommandFileDialog::runScript (Message *sg, Script *script)
{
  QStringList l;
  l << "QtStalker" + script->session() + ":" << tr("Select File");

  QStringList files = QFileDialog::getOpenFileNames(0,
                                                    tr("Select file(s)"),
                                                    QString(),
                                                    QString());

  if (! files.count())
    return _ERROR;

  sg->set("FILE_DIALOG", new SettingList(files));

  return _OK;
}
