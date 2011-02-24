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

#include "NewDialog.h"
#include "Globals.h"
#include "Doc.h"

#include <QtDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>

NewDialog::NewDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("New") << _command->parm("TITLE");
  setWindowTitle(l.join(" "));

  createGUI();

  _items = _command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  _name->clear();
  _name->addItems(_items);
  _name->clearEditText();

  loadSettings();

  nameChanged(QString());

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void NewDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QLabel *label = new QLabel(tr("Enter new") + " " + _command->parm("TITLE"));
  vbox->addWidget(label);

  _name = new QComboBox;
  _name->setEditable(TRUE);
  connect(_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(nameChanged(const QString &)));
  vbox->addWidget(_name);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void NewDialog::nameChanged (const QString &text)
{
  int status = 0;
  if (text.length())
    status = 1;

  _okButton->setEnabled(status);
}

void NewDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void NewDialog::done ()
{
  QString name = _name->lineEdit()->text();
  if (_items.contains(name))
  {
    _message->setText(name + " " + tr("already exists"));
    return;
  }

  saveSettings();

  _command->setReturnData("NEW_DIALOG_NAME", name);

  _command->setReturnCode("0");

  accept();
}

void NewDialog::cancel ()
{
  saveSettings();
  reject();
}

void NewDialog::loadSettings ()
{
  QSettings settings(g_globalSettings);

  QSize sz = settings.value("new_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("new_dialog_window_position").toPoint();
  if (! p.isNull())
    move(p);
}

void NewDialog::saveSettings ()
{
  QSettings settings(g_globalSettings);
  settings.setValue("new_dialog_window_size", size());
  settings.setValue("new_dialog_window_position", pos());
  settings.sync();
}
