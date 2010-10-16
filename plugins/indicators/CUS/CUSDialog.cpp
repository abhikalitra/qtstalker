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

#include "CUSDialog.h"
#include "CUS.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

CUSDialog::CUSDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void CUSDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // method
  QString d;
  _settings.getData(CUS::Command, d);

  _command = new QLineEdit(d);
  _command->setToolTip(tr("The interpreter command line and any switches required.\neg. perl -l -T"));
  form->addRow(tr("Command"), _command);

  // file
  _settings.getData(CUS::Script, d);

  _file = new FileButton(this, d);
  _file->setToolTip(tr("The script location"));
  form->addRow(tr("Script"), _file);

  _tabs->addTab(w, tr("General"));
}

void CUSDialog::done ()
{
  _settings.setData(CUS::Script, _file->getFile());
  _settings.setData(CUS::Command, _command->text());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

