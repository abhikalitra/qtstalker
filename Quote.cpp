/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "Quote.h"
#include <qlayout.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qlabel.h>

QuoteDialog::QuoteDialog (Config *c) : EditDialog (c)
{
  lib = 0;
  plug = 0;
  settings = 0;

  setCaption (tr("Qtstalker: Quotes"));

  QHBoxLayout *hbox = new QHBoxLayout(topBox);
  hbox->setSpacing(5);

  QLabel *label = new QLabel(tr("Plugins"), this);
  hbox->addWidget(label);

  QStringList l = QStringList::split(",", config->getData(Config::QuotePlugin), FALSE);
  ruleCombo = new QComboBox(this);
  ruleCombo->insertStringList(l, -1);
  connect (ruleCombo, SIGNAL(activated(int)), this, SLOT(ruleChanged(int)));
  hbox->addWidget(ruleCombo, 1, 0);

  okButton->hide();
  cancelButton->hide();

  grid->expand(4, 1);

  downloadButton = new QPushButton (tr("Download"), this);
  grid->addWidget(downloadButton, 0, 0);
  connect (downloadButton, SIGNAL (clicked()), this, SLOT (getQuotes()));

  cancelDownloadButton = new QPushButton (tr("Cancel"), this);
  grid->addWidget(cancelDownloadButton, 1, 0);
  connect (cancelDownloadButton, SIGNAL (clicked()), this, SLOT (cancelDownload()));
  cancelDownloadButton->setEnabled(FALSE);

  doneButton = new QPushButton (tr("Done"), this);
  grid->addWidget(doneButton, 2, 0);
  connect (doneButton, SIGNAL (clicked()), this, SLOT (reject()));

  ruleChanged(0);
}

QuoteDialog::~QuoteDialog ()
{
  if (plug)
  {
    delete plug;
    delete lib;
  }

  if (settings)
    delete settings;
}

void QuoteDialog::getQuotes ()
{
  emit message(tr("Starting download..."));

  disableGUI();

  cancelDownloadButton->setEnabled(TRUE);

  QStringList l = settings->getKeyList();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    SettingItem *i = settings->getItem(l[loop]);
    plug->setData(i->key, i->data);
    plug->setList(i->key, i->list);
  }

  plug->download();
}

void QuoteDialog::ruleChanged (int)
{
  if (plug)
  {
    delete plug;
    plug = 0;
    delete lib;
    lib = 0;
  }

  if (settings)
    delete settings;

  int loop = table->numRows();
  for (; loop != -1; loop--)
    table->removeRow(loop);

  QString s = config->getData(Config::PluginPath);
  s.append("/");
  s.append(ruleCombo->currentText());
  s.append(".so");

  lib = new QLibrary(s);
  Plugin *(*so)() = 0;
  so = (Plugin *(*)()) lib->resolve("create");
  if (! so)
  {
    qDebug("Quote::Dll error\n");
    delete lib;
    lib = 0;
    return;
  }

  plug = (*so)();

  connect (plug, SIGNAL(done()), this, SLOT(downloadComplete()));

  plug->setDataPath(config->getData(Config::DataPath));
  plug->setIndexPath(config->getData(Config::IndexPath));

  Setting *set = new Setting;
  QStringList l = plug->getKeyList();
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    SettingItem *i = plug->getItem(l[loop]);
    set->set(i->key, i->data, (Setting::Type) i->type);
    set->setList(i->key, i->list);
  }

  setItems(set);
}

void QuoteDialog::downloadComplete ()
{
  enableGUI();
  emit message(tr("Download complete."));
  emit chartUpdated();
}

void QuoteDialog::cancelDownload ()
{
  plug->cancelDownload();
  enableGUI();
  emit message(tr("Download cancelled."));
}

void QuoteDialog::enableGUI ()
{
  table->setEnabled(TRUE);
  downloadButton->setEnabled(TRUE);
  ruleCombo->setEnabled(TRUE);
  cancelButton->setEnabled(TRUE);
  cancelDownloadButton->setEnabled(FALSE);
}

void QuoteDialog::disableGUI ()
{
  table->setEnabled(FALSE);
  downloadButton->setEnabled(FALSE);
  ruleCombo->setEnabled(FALSE);
  cancelButton->setEnabled(FALSE);
  cancelDownloadButton->setEnabled(TRUE);
}



