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
#include "Globals.h"

#include <QtDebug>
#include <QMessageBox>

MessageDialog::MessageDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _keySize = "message_dialog_window_size";
  _keyPos = "message_dialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session << ":" << _command->parm("TITLE");
  setWindowTitle(l.join(" "));

  createGUI();

  _message->setText(_command->parm("MESSAGE"));

  loadSettings();
}

void MessageDialog::createGUI ()
{
  int pos = 0;
  QHBoxLayout *hbox = new QHBoxLayout;
  _vbox->insertLayout(pos++, hbox);

  // icon area
  QVBoxLayout *tvbox = new QVBoxLayout;
  hbox->addLayout(tvbox);

  QMessageBox mbox;
  mbox.setIcon(QMessageBox::Information);

  QLabel *label = new QLabel;
  label->setPixmap(mbox.iconPixmap());
  tvbox->addWidget(label);

  tvbox->addStretch();

  // message area
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  hbox->addLayout(tvbox);

  // status message
  delete _message;
  _message = new QLabel;
  QFont font = _message->font();
  font.setBold(TRUE);
  _message->setFont(font);
  tvbox->addWidget(_message);

  hbox->addStretch();
}

void MessageDialog::done ()
{
  _command->setReturnCode("0");
  saveSettings();
  accept();
}
