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
#include "QuotePlugin.h"
#include "HelpWindow.h"
#include "../pics/download.xpm"
#include "../pics/canceldownload.xpm"
#include "../pics/configure.xpm"
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qtooltip.h>

QuoteDialog::QuoteDialog () : QTabDialog (0, "QuoteDialog", FALSE, WDestructiveClose)
{
  setCaption (tr("Qtstalker: Quotes"));
  
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setSpacing(2);
  vbox->setMargin(5);
  
  toolbar = new Toolbar(w, 30, 30, FALSE);
  vbox->addWidget(toolbar);

  toolbar->addButton("update", download, tr("Update"));
  QObject::connect(toolbar->getButton("update"), SIGNAL(clicked()), this, SLOT(getQuotes()));
  toolbar->getButton("update")->setAccel(CTRL+Key_U);
  
  toolbar->addButton("cancelDownload", canceldownload, tr("Cancel Update"));
  QObject::connect(toolbar->getButton("cancelDownload"), SIGNAL(clicked()), this, SLOT(cancelDownload()));
  toolbar->setButtonStatus("cancelDownload", FALSE);
  toolbar->getButton("cancelDownload")->setAccel(CTRL+Key_C);
  
  vbox->addSpacing(5);
    
  QLabel *label = new QLabel(tr("Quote Plugins:"), w);
  vbox->addWidget(label);
  
  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setSpacing(5);
  grid->setColStretch(0, 1);
  
  ruleCombo = new QComboBox(w);
  QStringList pl = config.getPluginList(Config::QuotePluginPath);
  ruleCombo->insertStringList(pl, -1);
  connect (ruleCombo, SIGNAL(activated(int)), this, SLOT(ruleChanged(int)));
  grid->addWidget(ruleCombo, 0, 0);
  
  settingButton = new QPushButton(tr("Settings..."), w);
  QObject::connect(settingButton, SIGNAL(clicked()), this, SLOT(pluginSettings()));
  QToolTip::add(settingButton, tr("Settings"));
  settingButton->setPixmap(configure);
  grid->addWidget(settingButton, 0, 1);
  settingButton->setAccel(CTRL+Key_S);
  
  vbox->addSpacing(10);
  
  label = new QLabel(tr("Download Status:"), w);
  vbox->addWidget(label);
  
  statusLog = new QTextEdit(w);
  statusLog->setTextFormat(Qt::LogText);
  statusLog->setReadOnly(TRUE);
  vbox->addWidget(statusLog);
  
  addTab(w, tr("Quotes"));

  // create the data page
    
  w = new QWidget(this);
  
  vbox = new QVBoxLayout(w);
  vbox->setMargin(10);
  vbox->setSpacing(2);
  
  showData = new QCheckBox(tr("Show Data Log"), w);
  vbox->addWidget(showData);
  vbox->addSpacing(10);
  
  label = new QLabel(tr("Download Data:"), w);
  vbox->addWidget(label);
  
  dataLog = new QTextEdit(w);
  dataLog->setTextFormat(Qt::LogText);
  dataLog->setReadOnly(TRUE);
  vbox->addWidget(dataLog);

  addTab(w, tr("Data"));
  
  setOkButton(tr("&Done"));
  QObject::connect(this, SIGNAL(applyButtonPressed()), this, SLOT(accept()));
  
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
   
  int i = pl.findIndex(config.getData(Config::LastQuotePlugin));
  if (i != -1)
    ruleCombo->setCurrentItem(i);
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

  QuotePlugin *plug = config.getQuotePlugin(ruleCombo->currentText());
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
    config.closePlugin(plugin);
  plugin = ruleCombo->currentText();

  QuotePlugin *plug = config.getQuotePlugin(ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::ruleChanged - could not open plugin");
    return;
  }

  connect (plug, SIGNAL(done()), this, SLOT(downloadComplete()));
  connect (plug, SIGNAL(statusLogMessage(QString)), this, SLOT(printStatusLogMessage(QString)));
  connect (plug, SIGNAL(dataLogMessage(QString)), this, SLOT(printDataLogMessage(QString)));
  
  config.setData(Config::LastQuotePlugin, ruleCombo->currentText());
}

void QuoteDialog::downloadComplete ()
{
  enableGUI();
  emit chartUpdated();
}

void QuoteDialog::cancelDownload ()
{
  QuotePlugin *plug = config.getQuotePlugin(ruleCombo->currentText());
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

  QuotePlugin *plug = config.getQuotePlugin(ruleCombo->currentText());
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
  if (! showData->isChecked())
    return;
    
  dataLog->append(d);
  emit message(QString());
}

void QuoteDialog::pluginSettings ()
{
  QuotePlugin *plug = config.getQuotePlugin(ruleCombo->currentText());
  if (! plug)
  {
    qDebug("QuoteDialog::pluginSettings - could not open plugin");
    return;
  }
  
  plug->prefDialog(this);
}

void QuoteDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, "quotes.html");
  hw->show();
}

