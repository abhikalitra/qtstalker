/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "Plugin.h"
#include "download.xpm"
#include "canceldownload.xpm"
#include "configure.xpm"
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qtooltip.h>

QuoteDialog::QuoteDialog (Config *c) : QTabDialog (0, "QuoteDialog", TRUE)
{
  config = c;
  
  setCaption (tr("Qtstalker: Quotes"));
  
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setSpacing(2);
  vbox->setMargin(5);
  
  toolbar = new Toolbar(w, 30, 30);
  vbox->addWidget(toolbar);

  toolbar->addButton("update", download, tr("Update"));
  QObject::connect(toolbar->getButton("update"), SIGNAL(pressed()), this, SLOT(getQuotes()));
  
  toolbar->addButton("cancelDownload", canceldownload, tr("Cancel Update"));
  QObject::connect(toolbar->getButton("cancelDownload"), SIGNAL(pressed()), this, SLOT(cancelDownload()));
  toolbar->setButtonStatus("cancelDownload", FALSE);
  
  vbox->addSpacing(5);
    
  QLabel *label = new QLabel(tr("Quote Plugins:"), w);
  vbox->addWidget(label);
  
  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setSpacing(5);
  grid->setColStretch(0, 1);
  
  ruleCombo = new QComboBox(w);
  ruleCombo->insertStringList(config->getQuotePlugins(), -1);
  connect (ruleCombo, SIGNAL(activated(int)), this, SLOT(ruleChanged(int)));
  grid->addWidget(ruleCombo, 0, 0);
  
  settingButton = new QPushButton(tr("Settings..."), w);
  QObject::connect(settingButton, SIGNAL(pressed()), this, SLOT(pluginSettings()));
  QToolTip::add(settingButton, tr("Settings"));
  settingButton->setPixmap(configure);
  grid->addWidget(settingButton, 0, 1);
  
  vbox->addSpacing(10);
  
  label = new QLabel(tr("Download Status:"), w);
  vbox->addWidget(label);
  
  statusLog = new QMultiLineEdit(w);
  statusLog->setReadOnly(TRUE);
  vbox->addWidget(statusLog);
  
  addTab(w, tr("Quotes"));

  // create the data page
    
  w = new QWidget(this);
  
  vbox = new QVBoxLayout(w);
  vbox->setMargin(10);
  vbox->setSpacing(2);
  
  label = new QLabel(tr("Download Data:"), w);
  vbox->addWidget(label);
  
  dataLog = new QMultiLineEdit(w);
  dataLog->setReadOnly(TRUE);
  vbox->addWidget(dataLog);

  addTab(w, tr("Data"));
  
  setOkButton(tr("Done"));
   
  ruleChanged(0);
  
  resize(350, 350);
}

QuoteDialog::~QuoteDialog ()
{
}

void QuoteDialog::getQuotes ()
{
  statusLog->clear();
  dataLog->clear();
  
  printStatusLogMessage(tr("Starting update..."));

  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::getQuotes - could not open plugin");
    return;
  }

  disableGUI();

  plug->update();
}

void QuoteDialog::ruleChanged (int)
{
  if (! ruleCombo->count())
  {
    settingButton->setEnabled(FALSE);
    toolbar->setButtonStatus("update", FALSE);
    toolbar->setButtonStatus("cancelDownload", FALSE);
    return;
  }

  if (plugin.length())
    config->closePlugin(plugin);
  plugin = ruleCombo->currentText();

  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::ruleChanged - could not open plugin");
    return;
  }

  connect (plug, SIGNAL(done()), this, SLOT(downloadComplete()));
  connect (plug, SIGNAL(statusLogMessage(QString)), this, SLOT(printStatusLogMessage(QString)));
  connect (plug, SIGNAL(dataLogMessage(QString)), this, SLOT(printDataLogMessage(QString)));

  plug->setDataPath(config->getData(Config::DataPath));
}

void QuoteDialog::downloadComplete ()
{
  enableGUI();
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
  printStatusLogMessage(tr("Update cancelled."));
}

void QuoteDialog::enableGUI ()
{
  ruleCombo->setEnabled(TRUE);
  settingButton->setEnabled(TRUE);
  toolbar->setButtonStatus("update", TRUE);
  toolbar->setButtonStatus("cancelDownload", FALSE);

  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::enableGUI - could not open plugin");
    return;
  }
}

void QuoteDialog::disableGUI ()
{
  ruleCombo->setEnabled(FALSE);
  settingButton->setEnabled(FALSE);
  toolbar->setButtonStatus("update", FALSE);
  toolbar->setButtonStatus("cancelDownload", TRUE);
}

void QuoteDialog::printStatusLogMessage (QString d)
{
  statusLog->append(d);
  emit message(QString());
}

void QuoteDialog::printDataLogMessage (QString d)
{
  dataLog->append(d);
  emit message(QString());
}

void QuoteDialog::pluginSettings ()
{
  Plugin *plug = config->getPlugin(Config::QuotePluginPath, ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::pluginSettings - could not open plugin");
    return;
  }
  
  plug->prefDialog();
}

