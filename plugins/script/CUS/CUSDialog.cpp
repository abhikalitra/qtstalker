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

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

CUSDialog::CUSDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
}

void CUSDialog::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  setLayout(form);

  // command
  _command = new QLineEdit(_settings->data("COMMAND"));
  _command->setToolTip(tr("The interpreter command line and any switches required.\neg. perl -l -T"));
  form->addRow(tr("Command"), _command);

  // file
  QStringList l;
  l << _settings->data("SCRIPT");
  _file = new FileButton(this);
  _file->setFiles(l);
  _file->setToolTip(tr("The script location"));
  form->addRow(tr("Script"), _file);
}

void CUSDialog::save ()
{
  QStringList l = _file->files();
  if (l.count())
    _settings->setData("SCRIPT", l.at(0));
  
  _settings->setData("COMMAND", _command->text());
}