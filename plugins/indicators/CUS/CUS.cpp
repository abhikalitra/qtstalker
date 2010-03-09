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
#include "Config.h"

#include <QtDebug>
#include <QList>

CUS::CUS ()
{
  indicator = "CUS";

  settings.setData(Script, "perl /usr/local/share/qtstalker/indicator/");
}

int CUS::getIndicator (Indicator &_ind, BarData *_data)
{
  Config config;
  QString ipp, dbpp;
  config.getData(Config::IndicatorPluginPath, ipp);
  config.getData(Config::DBPluginPath, dbpp);
  
  QString s;
  settings.getData(Script, s);

  ExScript script(ipp, dbpp);
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

int CUS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Script, d);
  dialog->addTextItem(Script, page, QObject::tr("Script"), d);

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


