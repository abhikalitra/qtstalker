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

#include "IndicatorEditDialog.h"
#include "Globals.h"
#include "Doc.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>
#include <QFormLayout>
#include <QDir>

IndicatorEditDialog::IndicatorEditDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";

  _name = _command->parm("NAME");

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Editing Indicator") << _name;
  setWindowTitle(l.join(" "));

  createGUI();

  _indicator.setName(_name);
  _indicator.load();

  QString s = _indicator.command();
  if (! s.isEmpty())
    _com->setText(s);

  _file = _indicator.script();
  if (! _file.isEmpty())
  {
    QStringList tl;
    tl << _file;
    _fileButton->setFile(tl);
  }

  loadSettings();

  buttonStatus();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void IndicatorEditDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  vbox->addLayout(form);

  // command
  _com = new LineEdit;
  _com->setText("perl");
  _com->setToolTip(tr("Script command"));
  connect(_com, SIGNAL(textChanged(const QString &)), this, SLOT(buttonStatus()));
  form->addRow(tr("Command"), _com);

  // file
  QStringList tl;
  QString s;
  _fileButton = new FileButton(this, tl, s);
  connect(_fileButton, SIGNAL(signalSelectionChanged()), this, SLOT(buttonStatus()));
  form->addRow(tr("Script File"), _fileButton);

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

void IndicatorEditDialog::buttonStatus ()
{
  int status = 0;

  QStringList l;
  _fileButton->getFile(l);
  if (l.count())
  {
    _file = l.at(0);
    status++;
  }

  if (_com->text().length())
    status++;

  if (status ==  2)
    _okButton->setEnabled(TRUE);
  else
    _okButton->setEnabled(FALSE);
}

void IndicatorEditDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void IndicatorEditDialog::done ()
{
  QString com = _com->text();
  com.remove(",");
  if (com.isEmpty())
  {
    _message->setText(tr("Invalid script command."));
    return;
  }

  _indicator.setCommand(com);
  _indicator.setScript(_file);
  _indicator.save();

  _command->setReturnCode("0");

  saveSettings();

  accept();
}

void IndicatorEditDialog::cancel ()
{
  saveSettings();
  reject();
}

void IndicatorEditDialog::loadSettings ()
{
  QSettings settings(g_globalSettings);

  QSize sz = settings.value("indicator_edit_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("indicator_edit_dialog_window_position").toPoint();
  if (! p.isNull())
    move(p);

  if (_file.isEmpty())
  {
    _file = settings.value("indicator_edit_dialog_last_file", QDir::homePath()).toString();
    QStringList l;
    l << _file;
    _fileButton->setFile(l);
  }
}

void IndicatorEditDialog::saveSettings ()
{
  QSettings settings(g_globalSettings);
  settings.setValue("indicator_edit_dialog_window_size", size());
  settings.setValue("indicator_edit_dialog_window_position", pos());
  settings.setValue("indicator_edit_dialog_last_file", _file);
  settings.sync();
}
