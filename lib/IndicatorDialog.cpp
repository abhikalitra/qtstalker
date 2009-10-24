/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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
#include "Config.h"

#include <QLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QtDebug>
#include <QFileDialog>


IndicatorDialog::IndicatorDialog (QWidget *w) : QDialog (w, 0)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // indicator name
  QLabel *label = new QLabel(tr("Name"));
  grid->addWidget(label, row, col++);
  
  name = new QLabel;
  name->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(name, row++, col--);
  
  // indicator tab row
  label = new QLabel(tr("Tab Row"));
  grid->addWidget(label, row, col++);
  
  Config config;
  tabRow = new QSpinBox;
  tabRow->setRange(1, config.getInt(Config::IndicatorTabRows));
  grid->addWidget(tabRow, row++, col--);
  
  // indicator script command
  label = new QLabel(tr("Script Command"));
  grid->addWidget(label, row, col++);
  
  scriptCommand = new QLineEdit;
  grid->addWidget(scriptCommand, row++, col--);
  
  // indicator script path
  label = new QLabel(tr("Script File"));
  grid->addWidget(label, row, col++);
  
  path = new QLabel;
  path->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(path, row, col++);
  
  pathButton = new QToolButton;
  pathButton->setText("...");
  connect(pathButton, SIGNAL(clicked()), this, SLOT(slotPathButtonPressed()));
  grid->addWidget(pathButton, row++, col--);
  
  grid->setRowStretch(row, 2);
  
  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(buttonBox);
}

void IndicatorDialog::slotPathButtonPressed ()
{
  Config config;
  QString dirPath;
  config.getData(Config::IndicatorScriptPath, dirPath);
  QString f = QFileDialog::getOpenFileName(0,
                                           QString(tr("Script File...")),
                                           dirPath,
                                           QString(),
                                           0,
                                           0);
                                           
  if (f.isEmpty())
    return;
  
  path->setText(f);  
}

void IndicatorDialog::setName (QString &s)
{
  name->setText(s);
}

void IndicatorDialog::setTabRow (QString &s)
{
  tabRow->setValue(s.toInt());
}

void IndicatorDialog::getTabRow (QString &s)
{
  s = QString::number(tabRow->value());
}

void IndicatorDialog::setCommand (QString &s)
{
  scriptCommand->setText(s);
}

void IndicatorDialog::getCommand (QString &s)
{
  s = scriptCommand->text();
}

void IndicatorDialog::setFile (QString &s)
{
  path->setText(s);
}

void IndicatorDialog::getFile (QString &s)
{
  s = path->text();
}

