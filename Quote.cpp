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
#include "download.xpm"
#include "canceldownload.xpm"
#include "newchart.xpm"
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

  toolbar->expand(1, 6);

  downloadButton = new QToolButton(this);
  QToolTip::add(downloadButton, tr("Update"));
  downloadButton->setPixmap(QPixmap(download));
  connect(downloadButton, SIGNAL(clicked()), this, SLOT(getQuotes()));
  downloadButton->setMaximumWidth(30);
  downloadButton->setAutoRaise(TRUE);
  toolbar->addWidget(downloadButton, 0, 2);

  cancelDownloadButton = new QToolButton(this);
  QToolTip::add(cancelDownloadButton, tr("Cancel Update"));
  cancelDownloadButton->setPixmap(QPixmap(canceldownload));
  connect(cancelDownloadButton, SIGNAL(clicked()), this, SLOT(cancelDownload()));
  cancelDownloadButton->setMaximumWidth(30);
  toolbar->addWidget(cancelDownloadButton, 0, 3);
  cancelDownloadButton->setEnabled(FALSE);
  cancelDownloadButton->setAutoRaise(TRUE);

  newButton = new QToolButton(this);
  QToolTip::add(newButton, tr("New Chart"));
  newButton->setPixmap(QPixmap(newchart));
  connect(newButton, SIGNAL(clicked()), this, SLOT(newChart()));
  newButton->setMaximumWidth(30);
  toolbar->addWidget(newButton, 0, 4);
  newButton->setEnabled(FALSE);
  newButton->setAutoRaise(TRUE);

  ruleCombo = new QComboBox(this);
  ruleCombo->insertStringList(config->getQuotePlugins(), -1);
  connect (ruleCombo, SIGNAL(activated(int)), this, SLOT(ruleChanged(int)));
  topBox->addWidget(ruleCombo, 1, 0);

  okButton->setEnabled(FALSE);
  
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
  emit message(tr("Starting update..."));

  disableGUI();

  cancelDownloadButton->setEnabled(TRUE);

  QListViewItemIterator it(list);
  for (; it.current(); ++it)
  {
    item = it.current();
    plug->setData(item->text(0), item->text(1));
    plug->setList(item->text(0), settings->getList(item->text(0)));
  }

  plug->update();
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

  QString s = config->getData(Config::QuotePluginPath);
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
  
  if (plug->getCreateFlag())
    newButton->setEnabled(TRUE);
  else
    newButton->setEnabled(FALSE);

  list->clear();

  Setting *set = new Setting;
  QStringList l = plug->getKeyList();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    set->set(l[loop], plug->getData(l[loop]), plug->getType(l[loop]));
    set->setList(l[loop], plug->getList(l[loop]));
  }

  setItems(set);
}

void QuoteDialog::downloadComplete ()
{
  enableGUI();
  emit message(tr("Update complete."));
  emit chartUpdated();
}

void QuoteDialog::cancelDownload ()
{
  plug->cancelUpdate();
  enableGUI();
  emit message(tr("Update cancelled."));
}

void QuoteDialog::enableGUI ()
{
  list->setEnabled(TRUE);
  downloadButton->setEnabled(TRUE);
  ruleCombo->setEnabled(TRUE);
  cancelButton->setEnabled(TRUE);
  cancelDownloadButton->setEnabled(FALSE);
  if (plug->getCreateFlag())
    newButton->setEnabled(TRUE);
}

void QuoteDialog::disableGUI ()
{
  list->setEnabled(FALSE);
  downloadButton->setEnabled(FALSE);
  ruleCombo->setEnabled(FALSE);
  cancelButton->setEnabled(FALSE);
  cancelDownloadButton->setEnabled(TRUE);
  newButton->setEnabled(FALSE);
}

void QuoteDialog::newChart ()
{
  Setting *details = plug->getCreateDetails();

  EditDialog *dialog = new EditDialog(config);

  dialog->setCaption(tr("Qtstalker: New Chart"));

  dialog->setItems(details);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
    plug->createChart(details);

  delete details;
  delete dialog;
}



