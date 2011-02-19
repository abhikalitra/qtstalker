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

#include "InfoPanel.h"
#include "Globals.h"
#include "Strip.h"

#include "../pics/configure.xpm"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <cmath>
#include <QSettings>
#include <QInputDialog>

InfoPanel::InfoPanel ()
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  _lockStatus = TRUE;
  setReadOnly(TRUE);

  _menu = new QMenu(this);
  _menu->addAction(QPixmap(configure_xpm), tr("Number &Precision"), this, SLOT(dialog()), Qt::ALT+Qt::Key_P);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));
}

void InfoPanel::showInfo (Setting d)
{
  // list bar values first
  QStringList l;
  l << "D" << "T" << "O" << "H" << "L" << "C";
  
  QString s;
  QStringList vl;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    s = d.data(l.at(loop));
    if (s.length())
    {
      vl << l.at(loop) + " " + s;
      d.remove(l.at(loop));
    }
  }

  d.keyList(l);
  l.sort();

  Strip strip;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    s = d.data(l.at(loop));
    
    bool ok;
    double sn = s.toDouble(&ok);
    if (ok)
      strip.strip(sn, _precision, s);

    vl << l.at(loop) + " " + s;
  }

  setText(vl.join("\n"));
}

void InfoPanel::loadSettings ()
{
  QSettings settings(g_settingsFile);
  _precision = settings.value("info_panel_precision", 4).toInt();
  _lockStatus = settings.value("info_panel_lock_status", TRUE).toBool();
  emit signalLockStatus(_lockStatus);
}

void InfoPanel::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("info_panel_lock_status", _lockStatus);
}

void InfoPanel::setLockStatus (bool status)
{
  _lockStatus = status;
}

void InfoPanel::contextMenu ()
{
  _menu->exec(QCursor::pos());
}

void InfoPanel::dialog ()
{
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Info Panel Number Precision"));
  dialog->setLabelText(tr("Precision"));
  dialog->setInputMode(QInputDialog::IntInput);
  dialog->setIntRange(1, 4);
  dialog->setIntStep(1);
  dialog->setIntValue(_precision);
  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(setPrecision(int)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void InfoPanel::setPrecision (int d)
{
  _precision = d;

  QSettings settings(g_settingsFile);
  settings.setValue("info_panel_precision", _precision);
  settings.sync();
}
