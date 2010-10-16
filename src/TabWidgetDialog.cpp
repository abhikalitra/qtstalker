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
#include "Config.h"
#include "Globals.h"

#include <QtDebug>
#include <QFormLayout>

TabWidgetDialog::TabWidgetDialog (QString id)
{
  _id = id;
  _posList << tr("North") << tr("South") << tr("West") << tr("East");

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
  Config config;
  QString s;
  config.getData(_id, s);
  _ttp = s.toInt();

  _position = new QComboBox;
  _position->addItems(_posList);
  _position->setCurrentIndex(_ttp);
  form->addRow(tr("Position"), _position);
  
  // north south height
  QString key = _id + "NSH";
  QString d;
  config.getData(key, d);
  _tnsh = d.toInt();

  _nsh = new QSpinBox;
  _nsh->setRange(1, 100);
  _nsh->setValue(_tnsh);
  form->addRow(tr("North / South Height"), _nsh);

  // north south width
  key = _id + "NSW";
  config.getData(key, d);
  _tnsw = d.toInt();

  _nsw = new QSpinBox;
  _nsw->setRange(1, 100);
  _nsw->setValue(_tnsw);
  form->addRow(tr("North / South Width"), _nsw);

  // east west height
  key = _id + "EWH";
  config.getData(key, d);
  _tewh = d.toInt();

  _ewh = new QSpinBox;
  _ewh->setRange(1, 100);
  _ewh->setValue(_tewh);
  form->addRow(tr("East / West Height"), _ewh);

  // east west width
  key = _id + "EWW";
  config.getData(key, d);
  _teww = d.toInt();

  _eww = new QSpinBox;
  _eww->setRange(1, 100);
  _eww->setValue(_teww);
  form->addRow(tr("East / West Width"), _eww);

  _tabs->addTab(w, tr("Tab Settings"));
}

void TabWidgetDialog::done ()
{
  Config config;
  config.transaction();

  int flag = 0;
  QString d, key;
  
  int index = _position->currentIndex();
  if (index != _ttp)
  {
    d = QString::number(index);
    config.setData(_id, d);
    flag = 1;
  }

  if (_tnsh != _nsh->value())
  {
    key = _id + "NSH";
    d = QString::number(_nsh->value());
    config.setData(key, d);
    flag = 1;
  }

  if (_tnsw != _nsw->value())
  {
    key = _id + "NSW";
    d = QString::number(_nsw->value());
    config.setData(key, d);
    flag = 1;
  }

  if (_tewh != _ewh->value())
  {
    key = _id + "EWH";
    d = QString::number(_ewh->value());
    config.setData(key, d);
    flag = 1;
  }

  if (_teww != _eww->value())
  {
    key = _id + "EWW";
    d = QString::number(_eww->value());
    config.setData(key, d);
    flag = 1;
  }

  config.commit();

  if (flag)
    emit signalChanged();

  accept();
}

