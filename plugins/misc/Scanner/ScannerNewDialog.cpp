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

#include "ScannerNewDialog.h"
#include "Globals.h"
#include "ScannerDataBase.h"
#include "ScannerEditDialog.h"

#include <QtDebug>
#include <QLineEdit>

ScannerNewDialog::ScannerNewDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Scanner"));

  ScannerDataBase db;
  db.getScanners(_list);
  setList(_list);
}

void ScannerNewDialog::done ()
{
  QString name = _name->lineEdit()->text();

  // remove any forbidden sql characters
  name = name.remove(QString("'"), Qt::CaseSensitive);

  if (_list.contains(name))
  {
    setMessage(tr("Duplicate scanner name. Enter a unique name."));
    _name->lineEdit()->selectAll();
    _name->setFocus();
    return;
  }

  ScannerEditDialog *dialog = new ScannerEditDialog(name);
  dialog->show();

  accept();
}

