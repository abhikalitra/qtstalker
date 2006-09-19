/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#include "UpgradeMessage.h"
#include <qlayout.h>
#include <qwidget.h>
#include <qstring.h>
#include <qtextedit.h>

UpgradeMessage::UpgradeMessage (int type) : QTabDialog (0, "UpgradeMessage", TRUE)
{
  switch (type)
  {
    default:
      createPage031();
      break;
  }
}

UpgradeMessage::~UpgradeMessage ()
{
}

void UpgradeMessage::createPage031 ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QString s = tr("This version of Qtstalker uses a new data format.");
  s.append(tr(" It can not read data files from previous versions."));
  s.append(tr(" This means that you will start with an empty workspace."));
  s.append(tr(" Your old data files have been preserved in $HOME/Qtstalker"));
  s.append(tr(" and can still be accessed by older Qtstalker versions."));
  s.append(tr(" If you don't intend to downgrade to a previous Qtstalker"));
  s.append(tr(" version, then you can remove that directory."));

  QTextEdit *message = new QTextEdit(w);
  message->setReadOnly(TRUE);
  message->setText(s);
  vbox->addWidget(message);

  check = new QCheckBox(tr("Do not show this message again."), w);
  vbox->addWidget(check);

  addTab(w, tr("Warning"));
  
  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
}

bool UpgradeMessage::getStatus ()
{
  return check->isChecked();
}


