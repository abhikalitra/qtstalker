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
#include "EditDialog.h"
#include "download.xpm"
#include "canceldownload.xpm"
#include "newchart.xpm"
#include "stop.xpm"
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qtooltip.h>

QuoteDialog::QuoteDialog (Config *c) : QDialog (0, "QuoteDialog", TRUE)
{
  config = c;
  settings = 0;
  lib = 0;
  plug = 0;
  
  setCaption (tr("Qtstalker: Quotes"));

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  toolbar = new QGridLayout(vbox, 1, 5);
  toolbar->setSpacing(1);

  cancelButton = new QToolButton(this);
  QToolTip::add(cancelButton, tr("Cancel"));
  cancelButton->setPixmap(QPixmap(stop));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  cancelButton->setMaximumWidth(30);
  cancelButton->setAutoRaise(TRUE);
  toolbar->addWidget(cancelButton, 0, 0);

  downloadButton = new QToolButton(this);
  QToolTip::add(downloadButton, tr("Update"));
  downloadButton->setPixmap(QPixmap(download));
  connect(downloadButton, SIGNAL(clicked()), this, SLOT(getQuotes()));
  downloadButton->setMaximumWidth(30);
  downloadButton->setAutoRaise(TRUE);
  toolbar->addWidget(downloadButton, 0, 1);

  cancelDownloadButton = new QToolButton(this);
  QToolTip::add(cancelDownloadButton, tr("Cancel Update"));
  cancelDownloadButton->setPixmap(QPixmap(canceldownload));
  connect(cancelDownloadButton, SIGNAL(clicked()), this, SLOT(cancelDownload()));
  cancelDownloadButton->setMaximumWidth(30);
  toolbar->addWidget(cancelDownloadButton, 0, 2);
  cancelDownloadButton->setEnabled(FALSE);
  cancelDownloadButton->setAutoRaise(TRUE);

  newButton = new QToolButton(this);
  QToolTip::add(newButton, tr("New Chart"));
  newButton->setPixmap(QPixmap(newchart));
  connect(newButton, SIGNAL(clicked()), this, SLOT(newChart()));
  newButton->setMaximumWidth(30);
  toolbar->addWidget(newButton, 0, 3);
  newButton->setEnabled(FALSE);
  newButton->setAutoRaise(TRUE);

  ruleCombo = new QComboBox(this);
  ruleCombo->insertStringList(config->getQuotePlugins(), -1);
  connect (ruleCombo, SIGNAL(activated(int)), this, SLOT(ruleChanged(int)));
  vbox->insertWidget(1, ruleCombo, 0, 0);

  list = new SettingView (this, config->getData(Config::DataPath));
  vbox->addWidget(list);

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
  
  list->updateSettings();
  
  QStringList l = settings->getKeyList();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    plug->setData(l[loop], settings->getData(l[loop]));
    plug->setList(l[loop], settings->getList(l[loop]));
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
    
  if (! ruleCombo->count())
  {
    downloadButton->setEnabled(FALSE);
    newButton->setEnabled(FALSE);
    return;
  }

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
  connect (plug, SIGNAL(message(QString)), this, SLOT(printMessage(QString)));

  plug->setDataPath(config->getData(Config::DataPath));
  
  if (plug->getCreateFlag())
    newButton->setEnabled(TRUE);
  else
    newButton->setEnabled(FALSE);

  settings = new Setting;
  QStringList l = plug->getKeyList();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    settings->set(l[loop], plug->getData(l[loop]), plug->getType(l[loop]));
    settings->setList(l[loop], plug->getList(l[loop]));
  }

  list->setItems(settings);
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

void QuoteDialog::printMessage (QString d)
{
  emit message(d);
}


