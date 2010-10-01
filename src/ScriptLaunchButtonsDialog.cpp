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
#include <QLabel>
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

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(2, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // rows
  QLabel *label = new QLabel(tr("Rows"));
  grid->addWidget(label, row, col++);

  _rows = new QSpinBox;
  _rows->setRange(1, 5);
  grid->addWidget(_rows, row++, col--);

  // cols
  label = new QLabel(tr("Cols"));
  grid->addWidget(label, row, col++);

  _cols = new QSpinBox;
  _cols->setRange(1, 10);
  grid->addWidget(_cols, row++, col--);

  label = new QLabel(tr("Changes effective next startup"));
  vbox->addWidget(label);

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

