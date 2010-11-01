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
#include "Config.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPlugin.h"
#include "Globals.h"
#include "Strip.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

IndicatorDialog::IndicatorDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Indicator"));

  createMainPage();
}

void IndicatorDialog::createMainPage ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // create a unique default name
  IndicatorDataBase db;
  QStringList l;
  db.getIndicatorList(l);
  int loop = -1;
  QString d;
  do
  {
    loop++;
    d = tr("Indicator") + QString::number(loop);
  }
  while (l.indexOf(d) > -1);

  _name = new QLineEdit(d);
  _name->setToolTip(tr("Indicator name"));
  form->addRow(tr("Name"), _name);

  // indicator
  Config config;
  config.getData(Config::IndicatorPluginList, l);
  config.getData(Config::LastNewIndicator, d);
  
  _indicator = new QComboBox;
  _indicator->addItems(l);
  _indicator->setCurrentIndex(_indicator->findText(d));
  form->addRow(tr("Indicator"), _indicator);
  
  // tab row
  _row = new QSpinBox;
  _row->setRange(1, 3);
  form->addRow(tr("Tab Row"), _row);
  
  // tab column
  _col = new QSpinBox;
  _col->setRange(1, 3);
  form->addRow(tr("Tab Column"), _col);

  _tabs->addTab(w, tr("New Indicator"));
}

void IndicatorDialog::done ()
{
  QString name = _name->text();

  Strip strip;
  strip.verifyText(name);

  if (name.isEmpty())
  {
    setMessage(tr("Name missing."));
    return;
  }

  // check is name already exists
  IndicatorDataBase db;
  QStringList l;
  db.getIndicatorList(l);
  if (l.indexOf(name) != -1)
  {
    setMessage(tr("This indicator already exists."));
    return;
  }

  Config config;
  config.transaction();
  QString s = _indicator->currentText();
  config.setData(Config::LastNewIndicator, s);
  config.commit();

  Indicator i;
  i.setEnable(1);
  i.setTabRow(_row->value());
  i.setColumn(_col->value());
  i.setName(name);

  s = _indicator->currentText();
  i.setIndicator(s);

  emit signalDone(i);

  accept();
}
