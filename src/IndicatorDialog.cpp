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

#include "IndicatorDialog.h"
#include "IndicatorDataBase.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPlugin.h"
#include "Config.h"
#include "PrefDialog.h"

#include <QtDebug>
#include <QObject>
#include <QMessageBox>

IndicatorDialog::IndicatorDialog ()
{
}

int IndicatorDialog::newDialog ()
{
  Config config;
  QStringList l;
  config.getData(Config::IndicatorPluginList, l);

  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("New Indicator"));
  QString s = QObject::tr("Main");
  int page = 0;
  int pos = 0;
  dialog->addPage(page, s);

  // create a unique default name
  QStringList nl;
  IndicatorDataBase db;
  db.getIndicatorList(nl);
  int loop = -1;
  do
  {
    loop++;
    _name = QObject::tr("Indicator") + QString::number(loop);
  }
  while (nl.indexOf(_name) > -1);

  s = QObject::tr("Name");
  dialog->addTextItem(pos++, page, s, _name, QObject::tr("Indicator name"));

  s = QObject::tr("Indicator");
  QString indicator;
  dialog->addComboItem(pos++, page, s, l, 0);

  s = QObject::tr("Tab Row");
  dialog->addIntItem(pos++, page, s, 1, 1, 3);

  s = QObject::tr("Tab Column");
  dialog->addIntItem(pos++, page, s, 1, 1, 3);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return 1;
  }

  pos = 0;
  dialog->getItem(pos++, _name);
  if (_name.isEmpty())
  {
    QMessageBox::information(0, QObject::tr("Qtstalker: Error"), QObject::tr("Name missing."));
    delete dialog;
    return 1;
  }

  dialog->getItem(pos++, indicator);
  int tabRow = dialog->getInt(pos++);
  int column = dialog->getInt(pos++);

  delete dialog;

  // check is name already exists
  nl.clear();
  db.getIndicatorList(nl);
  if (nl.indexOf(_name) > -1)
  {
    QMessageBox::information(0, QObject::tr("Qtstalker: Error"), QObject::tr("This indicator already exists."));
    return 1;
  }

  Indicator i;
  i.setEnable(1);
  i.setName(_name);
  i.setTabRow(tabRow);
  i.setColumn(column);

  IndicatorPluginFactory fac;
  IndicatorPlugin *ip = fac.plugin(indicator);
  if (! ip)
    return 1;

  rc = ip->dialog(0);
  if (rc == QDialog::Rejected)
    return 1;

  ip->settings(i);

  db.setIndicator(i);

  return 0;
}

int IndicatorDialog::dialog (QString name)
{
  _name = name;
  Indicator i;
  i.setName(name);

  IndicatorDataBase db;
  db.getIndicator(i);
  QString indicator;
  indicator = i.indicator();

  IndicatorPluginFactory fac;
  IndicatorPlugin *ip = fac.plugin(indicator);
  if (! ip)
    return 1;

  ip->setSettings(i);

  int rc = ip->dialog(0);
  if (rc == QDialog::Rejected)
    return 1;

  ip->settings(i);

  db.setIndicator(i);

  return 0;
}

int IndicatorDialog::moveDialog (Indicator &i)
{
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Move Indicator"));
  QString s = QObject::tr("Main");
  int page = 0;
  int pos = 0;
  dialog->addPage(page, s);

  s = QObject::tr("Tab Row");
  dialog->addIntItem(pos++, page, s, i.tabRow(), 1, 3);

  s = QObject::tr("Tab Column");
  dialog->addIntItem(pos++, page, s, i.column(), 1, 3);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return 1;
  }

  pos = 0;
  i.setTabRow(dialog->getInt(pos++));
  i.setColumn(dialog->getInt(pos++));

  delete dialog;

  return 0;
}

QString & IndicatorDialog::name ()
{
  return _name;
}

