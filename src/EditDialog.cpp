/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "EditDialog.h"

EditDialog::EditDialog (Config *c) : QDialog (0, "EditDialog", TRUE)
{
  config = c;
  settings = 0;
  item = 0;

  basebox = new QVBoxLayout(this);
  basebox->setMargin(5);
  basebox->setSpacing(0);

  toolbar = new Toolbar(this, 30, 30);
  basebox->addWidget(toolbar);
  basebox->addSpacing(10);
  
  tabs = new QTabWidget(this);
  basebox->addWidget(tabs);
  basebox->addSpacing(5);

  list = new SettingView (this, config->getData(Config::DataPath));
  basebox->addWidget(list);
  basebox->addSpacing(10);
  
  QHBoxLayout *hbox = new QHBoxLayout(basebox);
  hbox->setSpacing(5);
  hbox->addStretch(1);
  
  okButton = new QPushButton(tr("OK"), this);
  connect(okButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
  hbox->addWidget(okButton);
  
  cancelButton = new QPushButton(tr("Cancel"), this);
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  hbox->addWidget(cancelButton);
}

EditDialog::~EditDialog ()
{
}

void EditDialog::setItems (Setting *set)
{
  settings = set;
  list->setItems(set);
}

void EditDialog::saveSettings ()
{
  list->updateSettings();
  accept();
}

void EditDialog::setButton (QString name, QPixmap pix, QString tt)
{
  toolbar->addButton(name, pix, tt);
}

QPushButton * EditDialog::getButton (QString name)
{
  return toolbar->getButton(name);
}

void EditDialog::setButtonStatus (QString name, bool status)
{
  toolbar->setButtonStatus(name, status);
}

void EditDialog::hideSettingView (bool d)
{
  if (d)
    list->hide();
  else
    list->show();
}

void EditDialog::hideTabs (bool d)
{
  if (d)
    tabs->hide();
  else
    tabs->show();
}

void EditDialog::hideToolbar (bool d)
{
  if (d)
    toolbar->hide();
  else
    toolbar->show();
}

