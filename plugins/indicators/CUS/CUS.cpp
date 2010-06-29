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

#include "CUS.h"
#include "ExScript.h"

#include <QtDebug>
#include <QList>

CUS::CUS ()
{
  _indicator = "CUS";

  _settings.setData(Command, "perl");
  _settings.setData(Script, "/usr/local/share/qtstalker/indicator/");
}

int CUS::getIndicator (Indicator &ind, BarData &data)
{
  QString s, s2;
  _settings.getData(Command, s);
  _settings.getData(Script, s2);
  s.append(" " + s2);

  ExScript script;
  script.setBarData(data);
  script.setIndicator(ind);
  int rc = script.calculate(s);
  if (! rc)
    ind = script.indicator();

  return rc;
}

int CUS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Command, d);
  dialog->addTextItem(Command, page, QObject::tr("Command"), d,
                      QObject::tr("The interpreter command line and any switches required.\neg. perl -l -T"));

  _settings.getData(Script, d);
  dialog->addFileItem(Script, page, QObject::tr("Script"), d,
                      QObject::tr("The script location."));

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  getDialogSettings(dialog);

  delete dialog;
  return rc;
}


//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CUS *o = new CUS;
  return ((IndicatorPlugin *) o);
}


