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

#include "TabWidgetDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QFormLayout>
#include <QSettings>

TabWidgetDialog::TabWidgetDialog (QString id)
{
  _id = id;
  _posList << tr("North") << tr("South") << tr("West") << tr("East");
  _helpFile = "main.html";

  setWindowTitle("Qtstalker" + g_session + ": " + tr("Chart Tab Settings"));

  createMainPage();
}

void TabWidgetDialog::createMainPage ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // position
  QSettings settings;
  settings.beginGroup("main" + g_session);

  _ttp = settings.value(_id).toInt();

  _position = new QComboBox;
  _position->addItems(_posList);
  _position->setCurrentIndex(_ttp);
  form->addRow(tr("Position"), _position);
  
  // north south height
  QString key = _id + "_NSH";
  _tnsh = settings.value(key).toInt();

  _nsh = new QSpinBox;
  _nsh->setRange(1, 100);
  _nsh->setValue(_tnsh);
  form->addRow(tr("North / South Height"), _nsh);

  // north south width
  key = _id + "_NSW";
  _tnsw = settings.value(key).toInt();

  _nsw = new QSpinBox;
  _nsw->setRange(1, 100);
  _nsw->setValue(_tnsw);
  form->addRow(tr("North / South Width"), _nsw);

  // east west height
  key = _id + "_EWH";
  _tewh = settings.value(key).toInt();

  _ewh = new QSpinBox;
  _ewh->setRange(1, 100);
  _ewh->setValue(_tewh);
  form->addRow(tr("East / West Height"), _ewh);

  // east west width
  key = _id + "_EWW";
  _teww = settings.value(key).toInt();

  _eww = new QSpinBox;
  _eww->setRange(1, 100);
  _eww->setValue(_teww);
  form->addRow(tr("East / West Width"), _eww);

  _tabs->addTab(w, tr("Tab Settings"));
}

void TabWidgetDialog::done ()
{
  QSettings settings;
  settings.beginGroup("main" + g_session);

  int flag = 0;
  QString d, key;
  
  int index = _position->currentIndex();
  if (index != _ttp)
  {
    settings.setValue(_id, index);
    flag = 1;
  }

  if (_tnsh != _nsh->value())
  {
    key = _id + "_NSH";
    settings.setValue(key, _nsh->value());
    flag = 1;
  }

  if (_tnsw != _nsw->value())
  {
    key = _id + "_NSW";
    settings.setValue(key, _nsw->value());
    flag = 1;
  }

  if (_tewh != _ewh->value())
  {
    key = _id + "_EWH";
    settings.setValue(key, _ewh->value());
    flag = 1;
  }

  if (_teww != _eww->value())
  {
    key = _id + "_EWW";
    settings.setValue(key, _eww->value());
    flag = 1;
  }

  settings.sync();

  if (flag)
    emit signalChanged();

  accept();
}
