/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "CUS.h"
#include "ExScript.h"

#include <QtDebug>
#include <QList>

CUS::CUS ()
{
  indicator = "CUS";
  commandKey = QObject::tr("Command");

  QString d;
  d = "perl /usr/local/share/qtstalker/indicator/";
  settings.setData(commandKey, d);
}

int CUS::getIndicator (Indicator &_ind, BarData *_data)
{
  QString s;
  settings.getData(commandKey, s);

  ExScript script;
  script.setBarData(_data);
  int rc = script.calculate(s);
  if (! rc)
  {
    QList<PlotLine *> lines;
    script.getLines(lines);
    _ind.setLines(lines);
  }

  return rc;
}

int CUS::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(commandKey, d);
  dialog->addTextItem(page, commandKey, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(commandKey, d);
  settings.setData(commandKey, d);

  delete dialog;
  return rc;
}


