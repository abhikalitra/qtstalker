/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "GroupDialog.h"
#include <qfiledialog.h>

GroupDialog::GroupDialog (Config *c) : EditDialog (c)
{
  setCaption(tr("Qtstalker: Edit Group"));

  table->hide();

  list = new QListBox(this);
  list->setSelectionMode(QListBox::Multi);
  list->setMinimumWidth(200);
  topBox->addWidget(list);

  grid->expand(5, 1);

  insertButton = new QPushButton (tr("Insert"), this);
  connect (insertButton, SIGNAL (clicked()), this, SLOT (insertItem()));
  grid->addWidget(insertButton, 2, 0);

  deleteButton = new QPushButton (tr("Delete"), this);
  connect (deleteButton, SIGNAL (clicked()), this, SLOT (deleteItem()));
  grid->addWidget(deleteButton, 3, 0);
}

GroupDialog::~GroupDialog ()
{
}

void GroupDialog::setItems (Setting *d)
{
  settings = d;
  list->clear();
  QStringList l = QStringList::split(" ", settings->getData("Group"));
  list->insertStringList(l, -1);
}

void GroupDialog::insertItem ()
{
  QStringList files(QFileDialog::getOpenFileNames("*", config->getData(Config::DataPath)));
  if (files.count())
  {
    int loop;
    QStringList l = QStringList::split(" ", settings->getData("Group"));
    for (loop = 0; loop < (int) files.count(); loop++)
    {
      QStringList l2(QStringList::split("/", files[loop], FALSE));
      list->insertItem(l2[l2.count() - 1], -1);
      l.append(l2[l2.count() - 1]);
    }

    list->sort(TRUE);

    l.sort();
    settings->set("Group", l.join(" "), Setting::Text);
  }
}

void GroupDialog::deleteItem ()
{
  if (list->currentItem() != -1)
  {
    int loop;
    QStringList l;
    for (loop = 0; loop < (int) list->count(); loop++)
    {
      if (list->isSelected(loop))
        list->removeItem(loop);
      else
        l.append(list->text(loop));
    }

    settings->set("Group", l.join(" "), Setting::Text);
  }
}

