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
#include "quotes.xpm"
#include "stop.xpm"
#include "done.xpm"
#include <qlayout.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qtooltip.h>

QuoteDialog::QuoteDialog (Config *c) : EditDialog (c)
{
  lib = 0;
  plug = 0;
  settings = 0;

  setCaption (tr("Qtstalker: Quotes"));

  QGridLayout *toolbar = new QGridLayout(topBox, 1, 4);
  toolbar->setSpacing(0);

  downloadButton = new QToolButton(this);
  QToolTip::add(downloadButton, tr("Download"));
  downloadButton->setPixmap(QPixmap(quotes));
  connect(downloadButton, SIGNAL(clicked()), this, SLOT(getQuotes()));
  downloadButton->setMaximumWidth(30);
  toolbar->addWidget(downloadButton, 0, 0);

  cancelDownloadButton = new QToolButton(this);
  QToolTip::add(cancelDownloadButton, tr("Cancel"));
  cancelDownloadButton->setPixmap(QPixmap(stop));
  connect(cancelDownloadButton, SIGNAL(clicked()), this, SLOT(cancelDownload()));
  cancelDownloadButton->setMaximumWidth(30);
  toolbar->addWidget(cancelDownloadButton, 0, 1);
  cancelDownloadButton->setEnabled(FALSE);

  doneButton = new QToolButton(this);
  QToolTip::add(doneButton, tr("Done"));
  doneButton->setPixmap(QPixmap(finished));
  connect(doneButton, SIGNAL(clicked()), this, SLOT(reject()));
  doneButton->setMaximumWidth(30);
  toolbar->addWidget(doneButton, 0, 2);

  QStringList l = QStringList::split(",", config->getData(Config::QuotePlugin), FALSE);
  ruleCombo = new QComboBox(this);
  ruleCombo->insertStringList(l, -1);
  connect (ruleCombo, SIGNAL(activated(int)), this, SLOT(ruleChanged(int)));
  topBox->addWidget(ruleCombo, 1, 0);
  
  okButton->hide();
  cancelButton->hide();

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



