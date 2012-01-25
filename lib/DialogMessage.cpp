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

#include "DialogMessage.h"
#include "WindowTitle.h"

#include <QtDebug>
#include <QGroupBox>

DialogMessage::DialogMessage (QWidget *p) : Dialog (p)
{
  _keySize = "message_dialog_window_size";
  _keyPos = "message_dialog_window_position";

  createGUI();

  loadSettings();
  
  WindowTitle wt;
  setWindowTitle(wt.title(tr("Message"), QString()));
}

void DialogMessage::createGUI ()
{
  QGroupBox *gbox = new QGroupBox(tr("Messages"));
  _vbox->insertWidget(0, gbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  gbox->setLayout(vbox);

  _messages = new QTextEdit;
  _messages->setReadOnly(TRUE);
  vbox->addWidget(_messages);

  // hide uneeded base class widget to save some space
  _message->hide();
}

void DialogMessage::setMessage (QString d)
{
  QDateTime dt = QDateTime::currentDateTime();
  QString s = dt.toString(Qt::ISODate) + "\n\n" + d;
  _messages->setText(s);
}
