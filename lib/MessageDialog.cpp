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

#include "MessageDialog.h"

#include <QtDebug>
#include <QGroupBox>

MessageDialog::MessageDialog (QWidget *p) : Dialog (p)
{
  _keySize = "message_dialog_window_size";
  _keyPos = "message_dialog_window_position";

  createGUI();

  loadSettings();
}

void MessageDialog::createGUI ()
{
  QGroupBox *gbox = new QGroupBox(tr("Messages"));
  _vbox->insertWidget(0, gbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  gbox->setLayout(vbox);

  _messages = new QTextEdit;
  vbox->addWidget(_messages);

  // hide uneeded base class widget to save some space
  _message->hide();
}

void MessageDialog::setMessage (QString d)
{
  _messages->setText(d);
}
