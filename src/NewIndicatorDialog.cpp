/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "NewIndicatorDialog.h"
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#define BUTTON_SIZE 24

NewIndicatorDialog::NewIndicatorDialog (Config *c) : QDialog (0, "NewIndicatorDialog", TRUE)
{
  config = c;

  setMinimumWidth(300);
  setMinimumHeight(200);

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(10);

  QGridLayout *grid = new QGridLayout(vbox, 3, 2);

  QLabel *label = new QLabel(tr("Indicator"), this);
  grid->addWidget(label, 0, 0);

  indicator = new QComboBox(this);
  indicator->insertStringList(config->getIndicatorPlugins(), -1);
  grid->addWidget(indicator, 0, 1);

  label = new QLabel(tr("Name"), this);
  grid->addWidget(label, 1, 0);

  name = new QLineEdit(this);
  name->setText(tr("New Indicator"));
  grid->addWidget(name, 1, 1);

  tabFlag = new QCheckBox(tr("Create Tab"), this);
  tabFlag->setChecked(TRUE);
  vbox->addWidget(tabFlag);

  vbox->insertStretch(2, 1);

  QHBoxLayout *hbox = new QHBoxLayout(vbox);

  QPushButton *button = new QPushButton(tr("OK"), this);
  hbox->addWidget(button);
  connect(button, SIGNAL(clicked()), this, SLOT(check()));

  button = new QPushButton(tr("Cancel"), this);
  hbox->addWidget(button);
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));
}

NewIndicatorDialog::~NewIndicatorDialog ()
{
}

QString NewIndicatorDialog::getIndicator ()
{
  return indicator->currentText();
}

QString NewIndicatorDialog::getName ()
{
  return name->text();
}

bool NewIndicatorDialog::getTabFlag ()
{
  return tabFlag->isChecked();
}

void NewIndicatorDialog::check ()
{
  if (! name->text().length())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Indicator name missing."));
    return;
  }

  QStringList l = config->getIndicator(name->text());
  if (l.count())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate indicator name."));
    return;
  }

  accept();
}


