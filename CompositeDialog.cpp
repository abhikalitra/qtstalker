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

#include "CompositeDialog.h"
#include "insert.xpm"
#include "delete.xpm"
#include <qfiledialog.h>
#include <qtooltip.h>
#include <qinputdialog.h>

CompositeDialog::CompositeDialog (Config *c) : EditDialog (c)
{
  toolbar->expand(1, 5);
  
  insertButton = new QToolButton(this);
  QToolTip::add(insertButton, tr("Insert"));
  insertButton->setPixmap(QPixmap(insert));
  connect(insertButton, SIGNAL(clicked()), this, SLOT(newComposite()));
  insertButton->setMaximumWidth(30);
  insertButton->setAutoRaise(TRUE);
  toolbar->addWidget(insertButton, 0, 2);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteComposite()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 3);
}

CompositeDialog::~CompositeDialog ()
{
}

void CompositeDialog::newComposite ()
{
  QString selection = QFileDialog::getOpenFileName(config->getData(Config::DataPath), "*", this, "file dialog");
  if (! selection.isNull())
  {
    list->clear();

    QStringList l = QStringList::split("/", selection, FALSE);
    QString symbol = l[l.count() - 1];
    settings->set(symbol, symbol, Setting::Symbol);

    QString s = symbol;
    s.append(tr(" Weight"));
    settings->set(s, "1", Setting::Float);

    makeSettings();
  }
}

void CompositeDialog::deleteComposite ()
{
  if (settings->count() == 0)
    return;

  QStringList l = settings->getKeyList();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (l[loop].contains(tr("Weight")))
      l.remove(l[loop]);
  }

  bool ok = FALSE;
  QString selection = QInputDialog::getItem(tr("Delete Composite"),
  					    tr("Select a composite item to delete."),
  					    l, 0, FALSE, &ok, this);
  if (ok == FALSE)
    return;

  settings->remove(selection);

  QString s = selection;
  s.append(tr(" Weight"));
  settings->remove(s);
  
  list->clear();

  makeSettings();
}

void CompositeDialog::setInsertButton (bool d)
{
  insertButton->setEnabled(d);
}

void CompositeDialog::setDeleteButton (bool d)
{
  deleteButton->setEnabled(d);
}

