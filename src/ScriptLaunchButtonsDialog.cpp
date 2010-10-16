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

#include "ScriptLaunchButtonsDialog.h"
#include "Globals.h"
#include "Config.h"

#include <QtDebug>
#include <QFormLayout>
#include <QString>
#include <QIcon>

ScriptLaunchButtonsDialog::ScriptLaunchButtonsDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("Configure Script Panel Launch Buttons"));

  createMainPage();

  loadSettings();
}

void ScriptLaunchButtonsDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // rows
  _rows = new QSpinBox;
  _rows->setRange(1, 5);
  form->addRow(tr("Rows"), _rows);

  // cols
  _cols = new QSpinBox;
  _cols->setRange(1, 10);
  form->addRow(tr("Cols"), _cols);

  _tabs->addTab(w, tr("Settings"));
}

void ScriptLaunchButtonsDialog::loadSettings ()
{
  Config config;
  _rows->setValue(config.getInt(Config::ScriptLaunchButtonRows));
  _cols->setValue(config.getInt(Config::ScriptLaunchButtonCols));
}

void ScriptLaunchButtonsDialog::done ()
{
  Config config;
  config.setData(Config::ScriptLaunchButtonRows, _rows->value());
  config.setData(Config::ScriptLaunchButtonCols, _cols->value());

  accept();
}

