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
#include "PluginFactory.h"
#include "DataDataBase.h"
#include "NewDialog.h"

#include <QtDebug>
#include <QSettings>
#include <QInputDialog>

IndicatorEditDialog::IndicatorEditDialog (QWidget *p, IndicatorSettings *set) : Dialog (p)
{
  _settings = set;
  _keySize = "indicator_edit_dialog_window_size";
  _keyPos = "indicator_edit_dialog_window_position";
  _status = _NONE;
  _newFlag = 0;

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Indicator");
  if (_settings != 0)
    l << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  if (_settings == 0)
  {
    Indicator i;
    i.settings()->copy(&_tsettings);
  }
  else
    _settings->copy(&_tsettings);

  createGUI();

  loadSettings();
}

void IndicatorEditDialog::createGUI ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  int loop = 0;
  for (; loop < _tsettings.count(); loop++)
  {
    Setting *set = _tsettings.settings(loop);
    if (! set)
      continue;

    addTab(set);
  }

  // add button
  QPushButton *b = _buttonBox->addButton(QString("&Add"), QDialogButtonBox::ActionRole);
  b->setToolTip(tr("Add Indicator"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addIndicator()));

  // delete button
  b = _buttonBox->addButton(QString("&Delete"), QDialogButtonBox::ActionRole);
  b->setToolTip(tr("Delete Indicator"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(deleteIndicator()));

  // apply button
  b = _buttonBox->addButton(QString("A&pply"), QDialogButtonBox::ActionRole);
  b->setToolTip(tr("Apply Changes"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(apply()));
}

void IndicatorEditDialog::done ()
{
  _status = _DONE;
  if (applySave())
    return;
  saveSettings();
  accept();
}

void IndicatorEditDialog::addIndicator ()
{
  QStringList l;
  QSettings set(g_globalSettings);
  l = set.value("indicator_plugins").toStringList();
  l.sort();

  QSettings set2(g_localSettings);
  QString s = set2.value("indicator_edit_dialog_last_indicator", 0).toString();
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setTextValue(s);
  dialog->setLabelText(tr("Select indicator"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(addIndicator2(QString)));
  dialog->show();
}

void IndicatorEditDialog::addIndicator2 (QString d)
{
  PluginFactory fac;
  Plugin *plug = fac.plugin(d);
  if (! plug)
  {
    qDebug() << "IndicatorEditDialog::addIndicator2: no plugin" << d;
    return;
  }

  QSettings settings(g_localSettings);
  settings.setValue("indicator_edit_dialog_last_indicator", d);
  settings.sync();

  Setting *set = new Setting;
  plug->defaults(set);
  delete plug;

  _tsettings.addSettings(set);

  addTab(set);
}

void IndicatorEditDialog::deleteIndicator ()
{
  int index = _tabs->currentIndex();
  if (index == -1)
    return;

  QWidget *w = _tabs->currentWidget();
  if (! w)
    return;

  _tabs->removeTab(index);

  delete w;

  _tsettings.removeSettings(index);
}

void IndicatorEditDialog::addTab (Setting *set)
{
  PluginFactory fac;
  Plugin *plug = fac.plugin(set->data("PLUGIN"));
  if (! plug)
  {
    qDebug() << "IndicatorEditDialog::addTab: no plugin" << set->data("PLUGIN");
    return;
  }

  QWidget *w = plug->dialog(this, set);
  if (! w)
  {
    delete plug;
    return;
  }

  delete plug;
  
  connect(this, SIGNAL(signalSave()), w, SLOT(save()));

  _tabs->addTab(w, set->data("PLUGIN"));

  _tabs->setCurrentIndex(_tabs->count() - 1);
}

void IndicatorEditDialog::newDialog ()
{
  QStringList l;
  DataDataBase db("indicators");
  db.names(l);

  NewDialog *dialog = new NewDialog(this);
  dialog->setItems(l);
  dialog->setTitle(tr("Enter new indicator name"));
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(newDialog2(QString)));
  dialog->show();
}

void IndicatorEditDialog::newDialog2 (QString d)
{
  _tsettings.setData("NAME", d);

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Indicator") << _tsettings.data("NAME");
  setWindowTitle(l.join(" "));

  if (_status == _DONE)
    done();
  else if (_status == _APPLY)
    applySave();
}

void IndicatorEditDialog::apply ()
{
  _status = _APPLY;
  applySave();
}

int IndicatorEditDialog::applySave ()
{
  if (_tsettings.data("NAME").isEmpty())
  {
    newDialog();
    return 1;
  }

  // each tab finalize their changes to _settings
  emit signalSave();

  if (_settings == 0)
    _tsettings.save();
  else
  {
    _settings->clearAll();
    _tsettings.copy(_settings);
    _settings->save();
  }

  Indicator i;
  i.add(_tsettings.data("NAME"));

  if (_settings == 0 && ! _newFlag)
  {
    _newFlag = 1;
    g_middleMan->indicatorNew(_tsettings.data("NAME"));
  }

  emit signalDone();

  return 0;
}
