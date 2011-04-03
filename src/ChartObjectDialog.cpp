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

#include "ChartObjectDialog.h"
#include "Globals.h"
#include "PluginFactory.h"

#include <QtDebug>
#include <QSettings>

ChartObjectDialog::ChartObjectDialog (QWidget *p, ChartObject *set) : Dialog (p)
{
  _co = set;
  _keySize = "chart_object_dialog_window_size";
  _keyPos = "chart_object_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit") << _co->settings()->data("TYPE");
  setWindowTitle(l.join(" "));

  createGUI();
  loadSettings();
}

void ChartObjectDialog::createGUI ()
{
  PluginFactory fac;
  Plugin *plug = fac.plugin(_co->settings()->data("PLUGIN"));
  if (! plug)
  {
    qDebug() << "ChartObjectDialog::createGUI: no plugin";
    return;
  }

  QWidget *w = plug->dialog(this, _co->settings());
  if (! w)
  {
    qDebug() << "ChartObjectDialog::createGUI: no widget";
    delete plug;
    return;
  }
  
  delete plug;

  connect(this, SIGNAL(signalSave()), w, SLOT(save()));

  _vbox->insertWidget(0, w);
}

void ChartObjectDialog::done ()
{
  emit signalSave();
  _co->save();
  saveSettings();
  accept();
}
