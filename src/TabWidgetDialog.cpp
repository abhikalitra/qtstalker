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
#include <QLayout>
#include <QLabel>

TabWidgetDialog::TabWidgetDialog (QString id) : QDialog (0, 0)
{
  _posList << tr("North") << tr("South") << tr("West") << tr("East");

  _id = id;
  
  setAttribute(Qt::WA_DeleteOnClose);
  
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Chart Tab Settings"));

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(5);
  setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // position
  QLabel *label = new QLabel(tr("Position"));
  grid->addWidget(label, row, col++);

  Config config;
  QString s;
  config.getData(_id, s);
  _ttp = s.toInt();

  _position = new QComboBox;
  _position->addItems(_posList);
  _position->setCurrentIndex(_ttp);
  grid->addWidget(_position, row++, col--);

  
  // north south height
  label = new QLabel(tr("North / South Height"));
  grid->addWidget(label, row, col++);

  QString key = _id + "NSH";
  QString d;
  config.getData(key, d);
  _tnsh = d.toInt();

  _nsh = new QSpinBox;
  _nsh->setRange(1, 100);
  _nsh->setValue(_tnsh);
  grid->addWidget(_nsh, row++, col--);

  // north south width
  label = new QLabel(tr("North / South Width"));
  grid->addWidget(label, row, col++);

  key = _id + "NSW";
  config.getData(key, d);
  _tnsw = d.toInt();

  _nsw = new QSpinBox;
  _nsw->setRange(1, 100);
  _nsw->setValue(_tnsw);
  grid->addWidget(_nsw, row++, col--);

  // east west height
  label = new QLabel(tr("East / West Height"));
  grid->addWidget(label, row, col++);

  key = _id + "EWH";
  config.getData(key, d);
  _tewh = d.toInt();

  _ewh = new QSpinBox;
  _ewh->setRange(1, 100);
  _ewh->setValue(_tewh);
  grid->addWidget(_ewh, row++, col--);

  // east west width
  label = new QLabel(tr("East / West Width"));
  grid->addWidget(label, row, col++);

  key = _id + "EWW";
  config.getData(key, d);
  _teww = d.toInt();

  _eww = new QSpinBox;
  _eww->setRange(1, 100);
  _eww->setValue(_teww);
  grid->addWidget(_eww, row++, col--);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(_buttonBox);

  vbox->addStretch(1);
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

