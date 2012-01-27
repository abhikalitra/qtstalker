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

#include "IndicatorRemove.h"
#include "DialogConfirm.h"
#include "GlobalPlotGroup.h"
#include "EAVDataBase.h"

#include <QtDebug>
#include <QSettings>
#include <QStringList>

IndicatorRemove::IndicatorRemove (QObject *p, QString indicator) : QObject (p)
{
  _indicator = indicator;
}

void IndicatorRemove::run ()
{
  QStringList mess;
  mess << tr("Confirm indicator removal") + "\n";
  mess << _indicator;

  DialogConfirm *dialog = new DialogConfirm(0, QString(), Entity());
  dialog->setMessage(mess.join("\n"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(remove2()));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  dialog->show();
}

void IndicatorRemove::remove2 ()
{
  Plot *p = g_plotGroup->plot(_indicator);
  if (! p)
  {
    done();
    return;
  }

  QStringList l;
  l << _indicator;
  
  EAVDataBase db("indicators");
  db.transaction();
  if (db.remove(l))
  {
    done();
    return;
  }
  db.commit();

  g_plotGroup->removePlot(_indicator);

  done();
}

void IndicatorRemove::done ()
{
  deleteLater();
}
