/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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
#include "Plugin.h"
#include "download.xpm"
#include "canceldownload.xpm"
#include "newchart.xpm"
#include <qstringlist.h>
#include <qmessagebox.h>

QuoteDialog::QuoteDialog (Config *c) : EditDialog (c)
{
  settings = 0;

  setCaption (tr("Qtstalker: Quotes"));

  setButtonStatus(0, FALSE);

  setButton(QPixmap(download), tr("Update"), 2);
  connect(getButton(2), SIGNAL(clicked()), this, SLOT(getQuotes()));

  setButton(QPixmap(canceldownload), tr("Cancel Update"), 3);
  connect(getButton(3), SIGNAL(clicked()), this, SLOT(cancelDownload()));
  setButtonStatus(3, FALSE);

  setButton(QPixmap(newchart), tr("New Chart"), 4);
  connect(getButton(4), SIGNAL(clicked()), this, SLOT(newChart()));
  setButtonStatus(4, FALSE);

  ruleCombo = new QComboBox(this);
  ruleCombo->insertStringList(config->getQuotePlugins(), -1);
  connect (ruleCombo, SIGNAL(activated(int)), this, SLOT(ruleChanged(int)));
  basebox->insertWidget(1, ruleCombo, 0, 0);

  ruleChanged(0);
}

QuoteDialog::~QuoteDialog ()
{
  config->closePlugins();
}

void QuoteDialog::getQuotes ()
{
  emit message(tr("Starting update..."));

  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::getQuotes - could not open plugin");
    return;
  }

  disableGUI();

  setButtonStatus(3, TRUE);

  list->updateSettings();

  plug->merge(settings->getStringList());

  // refresh any lists 
  QStringList key = settings->getKeyList();
  int loop;
  for(loop = 0; loop < (int) key.count(); loop++)
    plug->setList(key[loop], settings->getList(key[loop]));

  plug->update();
}

void QuoteDialog::ruleChanged (int)
{
  if (settings)
    delete settings;

  if (! ruleCombo->count())
  {
    setButtonStatus(2, FALSE);
    setButtonStatus(4, FALSE);
    return;
  }

  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::ruleChanged - could not open plugin");
    return;
  }

  connect (plug, SIGNAL(done()), this, SLOT(downloadComplete()));
  connect (plug, SIGNAL(message(QString)), this, SLOT(printMessage(QString)));

  plug->setDataPath(config->getData(Config::DataPath));

  if (plug->getCreateFlag())
    setButtonStatus(4, TRUE);
  else
    setButtonStatus(4, FALSE);

  settings = plug->getPluginSettings();

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
  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::cancelDownload - could not open plugin");
    return;
  }

  plug->cancelUpdate();
  enableGUI();
  emit message(tr("Update cancelled."));
}

void QuoteDialog::enableGUI ()
{
  list->setEnabled(TRUE);
  ruleCombo->setEnabled(TRUE);

  setButtonStatus(1, TRUE);
  setButtonStatus(2, TRUE);
  setButtonStatus(3, FALSE);

  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::enableGUI - could not open plugin");
    return;
  }

  if (plug->getCreateFlag())
    setButtonStatus(4, TRUE);
}

void QuoteDialog::disableGUI ()
{
  list->setEnabled(FALSE);
  ruleCombo->setEnabled(FALSE);

  setButtonStatus(1, FALSE);
  setButtonStatus(2, FALSE);
  setButtonStatus(3, TRUE);
  setButtonStatus(4, FALSE);
}

void QuoteDialog::newChart ()
{
  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::newChart - could not open plugin");
    return;
  }

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


