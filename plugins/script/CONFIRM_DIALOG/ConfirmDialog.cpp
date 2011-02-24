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

#include "ConfirmDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QStringList>
#include <QSettings>

ConfirmDialog::ConfirmDialog (Command *c)
{
  _command = c;

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Confirm");
  setWindowTitle(l.join(" "));

  createGUI();

  _message->setText(_command->parm("MESSAGE"));

  loadSettings();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void ConfirmDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox;
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // yes button
  _yesButton = bbox->addButton(QDialogButtonBox::Yes);

  // no button
  _noButton = bbox->addButton(QDialogButtonBox::No);
  _noButton->setDefault(TRUE);
}

void ConfirmDialog::done ()
{
  _command->setReturnCode("0");
  saveSettings();
  accept();
}

void ConfirmDialog::cancel ()
{
  saveSettings();
  reject();
}

void ConfirmDialog::loadSettings ()
{
  QSettings settings(g_globalSettings);

  QSize sz = settings.value("confirm_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("confirm_dialog_window_position").toPoint();
  if (! p.isNull())
    move(p);
}

void ConfirmDialog::saveSettings ()
{
  QSettings settings(g_globalSettings);
  settings.setValue("confirm_dialog_window_size", size());
  settings.setValue("confirm_dialog_window_position", pos());
  settings.sync();
}
