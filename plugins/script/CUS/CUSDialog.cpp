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

#include "CUSDialog.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

CUSDialog::CUSDialog (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void CUSDialog::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // command
  _command = new LineEdit;
  _command->setToolTip(tr("The interpreter command line and any switches required.\neg. perl -l -T"));
  form->addRow(tr("Command"), _command);

  // file
  _file = new FileButton(this);
  _file->setToolTip(tr("The script location"));
  form->addRow(tr("Script"), _file);
}

void CUSDialog::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "CUS")
    return;

  _command->setText(command.parm("COMMAND"));

  QStringList l;
  l << command.parm("SCRIPT");
  _file->setFiles(l);
}

void CUSDialog::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=CUS";
  cl << "COMMAND=" + _command->text();

  QStringList fl = _file->files();
  if (fl.count())
    cl << "SCRIPT=" + fl.at(0);
  
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
