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
#include <QDialogButtonBox>
#include <QLayout>
#include <QStringList>
#include <QSettings>
#include <QMessageBox>

MessageDialog::MessageDialog (Command *c)
{
  _command = c;

  QStringList l;
  l << "QtStalker" + g_session << ":" << _command->parm("TITLE");
  setWindowTitle(l.join(" "));

  createGUI();

  _message->setText(_command->parm("MESSAGE"));

  loadSettings();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void MessageDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  vbox->addLayout(hbox);

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
  _message = new QLabel;
  QFont font = _message->font();
  font.setBold(TRUE);
  _message->setFont(font);
  tvbox->addWidget(_message);

  hbox->addStretch();

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox;
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
}

void MessageDialog::done ()
{
  _command->setReturnCode("0");
  saveSettings();
  accept();
}

void MessageDialog::loadSettings ()
{
  QSettings settings(g_globalSettings);

  QSize sz = settings.value("message_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("message_dialog_window_position").toPoint();
  if (! p.isNull())
    move(p);
}

void MessageDialog::saveSettings ()
{
  QSettings settings(g_globalSettings);
  settings.setValue("message_dialog_window_size", size());
  settings.setValue("message_dialog_window_position", pos());
  settings.sync();
}
