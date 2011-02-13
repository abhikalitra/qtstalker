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

#include "IndicatorNewDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>
#include <QFormLayout>

IndicatorNewDialog::IndicatorNewDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("New Indicator");
  setWindowTitle(l.join(" "));

  createGUI();

  IndicatorDataBase db;
  l.clear();
  db.indicators(l);
  
  _name->addItems(l);
  _name->clearEditText();

  loadSettings();

  nameChanged(QString());

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void IndicatorNewDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  vbox->addLayout(form);

  _name = new QComboBox;
  _name->setEditable(TRUE);
  connect(_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(nameChanged(const QString &)));
  form->addRow(tr("Indicator Name"), _name);

/*  
  QStringList l;
  l << tr("NONE") << tr("BARS");

  _indicator = new QComboBox;
  _indicator->addItems(l);
  form->addRow(tr("Indicator"), _indicator);
*/

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

void IndicatorNewDialog::nameChanged (const QString &text)
{
  int status = 0;
  if (text.length())
    status = 1;

  _okButton->setEnabled(status);
}

void IndicatorNewDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void IndicatorNewDialog::done ()
{
  QString name = _name->lineEdit()->text();
  if (_items.contains(name))
  {
    _message->setText(name + " " + tr("already exists"));
    return;
  }

  Indicator i;
  i.setName(name);
//  i.setDialog(_indicator->currentText());
  i.save();

  saveSettings();

//  QStringList l;
//  l << name << _indicator->currentText();

  _command->setReturnData("INDICATOR_NEW_DIALOG_NAME", name);

  _command->setReturnCode("0");

  accept();
}

void IndicatorNewDialog::cancel ()
{
  saveSettings();
  reject();
}

void IndicatorNewDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("indicator_new_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("indicator_new_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void IndicatorNewDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("indicator_new_dialog_window_size", size());
  settings.setValue("indicator_new_dialog_window_position", pos());
  settings.sync();
}
