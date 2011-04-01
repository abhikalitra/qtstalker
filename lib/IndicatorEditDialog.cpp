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

#include <QtDebug>
#include <QSettings>
#include <QInputDialog>

IndicatorEditDialog::IndicatorEditDialog (QWidget *p, IndicatorSettings *set) : Dialog (p)
{
  _settings = set;
  _keySize = "indicator_edit_dialog_window_size";
  _keyPos = "indicator_edit_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();
}

void IndicatorEditDialog::createGUI ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  int loop = 0;
  for (; loop < _settings->count(); loop++)
  {
    Setting *set = _settings->settings(loop);
    if (! set)
      continue;

    addTab(set);
  }

  // add button
  QPushButton *b = _buttonBox->addButton(QString("&Add"), QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addIndicator()));

  // delete button
  b = _buttonBox->addButton(QString("&Delete"), QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked(bool)), this, SLOT(deleteIndicator()));
}

void IndicatorEditDialog::done ()
{
  // each tab finalize their changes to _settings
  emit signalSave();
  
  _settings->save();
  
  QSettings set(g_localSettings);
  QStringList l = set.value("indicators").toStringList();
  l.append(_settings->data("NAME"));
  l.removeDuplicates();
  set.setValue("indicators", l);
  set.sync();

  saveSettings();

  emit signalDone();

  accept();
}

void IndicatorEditDialog::addIndicator ()
{
  QStringList l;
  QSettings set(g_globalSettings);
  l = set.value("indicator_plugins").toStringList();
  l.sort();
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
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

  Setting *set = new Setting;
  plug->defaults(set);
  delete plug;

  _settings->addSettings(set);

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

  _settings->removeSettings(index);
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

  connect(this, SIGNAL(signalSave()), w, SLOT(save()));
  _tabs->addTab(w, set->data("PLUGIN"));

  delete plug;
}
