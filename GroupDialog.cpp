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
#include "insert.xpm"
#include "delete.xpm"
#include <qfiledialog.h>
#include <qtooltip.h>

GroupDialog::GroupDialog (Config *c) : EditDialog (c)
{
  flag = FALSE;

  setCaption(tr("Qtstalker: Edit Group"));

  list->hide();

  list2 = new QListBox(this);
  list2->setSelectionMode(QListBox::Multi);
  list2->setMinimumWidth(200);
  topBox->addWidget(list2);

  toolbar->expand(1, 5);

  insertButton = new QToolButton(this);
  QToolTip::add(insertButton, tr("Insert"));
  insertButton->setPixmap(QPixmap(insert));
  connect(insertButton, SIGNAL(clicked()), this, SLOT(insertItem()));
  insertButton->setMaximumWidth(30);
  insertButton->setAutoRaise(TRUE);
  toolbar->addWidget(insertButton, 0, 2);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 3);
}

GroupDialog::~GroupDialog ()
{
}

void GroupDialog::setGroup (QStringList l)
{
  group = l;
  list2->clear();
  list2->insertStringList(l, -1);
}

QStringList GroupDialog::getGroup ()
{
  return group;
}

void GroupDialog::insertItem ()
{
  QStringList files(QFileDialog::getOpenFileNames("*", config->getData(Config::DataPath)));
  if (files.count())
  {
    int loop;
    for (loop = 0; loop < (int) files.count(); loop++)
    {
      QStringList l2(QStringList::split("/", files[loop], FALSE));
      list2->insertItem(l2[l2.count() - 1], -1);
      group.append(l2[l2.count() - 1]);
      flag = TRUE;
    }

    list2->sort(TRUE);
    group.sort();
  }
}

void GroupDialog::deleteItem ()
{
  if (list2->currentItem() != -1)
  {
    int loop;
    for (loop = 0; loop < (int) list2->count(); loop++)
    {
      if (list2->isSelected(loop))
      {
	group.remove(list2->text(loop));
        list2->removeItem(loop);
        flag = TRUE;
      }
    }
  }
}

bool GroupDialog::getFlag ()
{
  return flag;
}


