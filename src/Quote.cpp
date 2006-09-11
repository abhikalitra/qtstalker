/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

QuoteDialog::QuoteDialog (QWidget *pw) : QTabDialog (pw, "QuoteDialog", FALSE, WDestructiveClose)
{
  setCaption (tr("Qtstalker: Quotes"));
  
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setSpacing(2);
  vbox->setMargin(5);
  
  toolbar = new Toolbar(w, 30, 30, FALSE);
  vbox->addWidget(toolbar);

  QString s("update");
  QString s2(tr("Update"));
  toolbar->addButton(s, download, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(getQuotes()));
  toolbar->getButton(s)->setAccel(CTRL+Key_U);
  
  s = "cancelDownload";
  s2 = tr("Cancel Update");
  toolbar->addButton(s, canceldownload, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(cancelDownload()));
  toolbar->setButtonStatus(s, FALSE);
  toolbar->getButton(s)->setAccel(CTRL+Key_C);
  
  vbox->addSpacing(5);
    
  QLabel *label = new QLabel(tr("Quote Source:"), w);
  vbox->addWidget(label);
  
  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setSpacing(5);
  grid->setColStretch(0, 1);
  
  ruleCombo = new QComboBox(w);
  QStringList pl;
  config.getPluginList(Config::QuotePluginPath, pl);
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

  setOkButton(tr("&Done"));
  QObject::connect(this, SIGNAL(applyButtonPressed()), this, SLOT(accept()));
  
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
   
  config.getData(Config::LastQuotePlugin, s);
  int i = pl.findIndex(s);
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
  
  printStatusLogMessage(tr("Starting update..."));

  QString s = ruleCombo->currentText();
  QuotePlugin *plug = config.getQuotePlugin(s);
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
    QString s("update"); 
    toolbar->setButtonStatus(s, FALSE);
    s = "cancelDownload";
    toolbar->setButtonStatus(s, FALSE);
    return;
  }

  if (plugin.length())
    config.closePlugin(plugin);
  plugin = ruleCombo->currentText();

  QuotePlugin *plug = config.getQuotePlugin(plugin);
  if (! plug)
  {
    qDebug("QuoteDialog::ruleChanged - could not open plugin");
    return;
  }

  connect (plug, SIGNAL(done()), this, SLOT(downloadComplete()));
  connect (plug, SIGNAL(statusLogMessage(QString)), this, SLOT(printStatusLogMessage(QString)));
  
  config.setData(Config::LastQuotePlugin, plugin);
}

void QuoteDialog::downloadComplete ()
{
  enableGUI();
  emit chartUpdated();
}

void QuoteDialog::cancelDownload ()
{
  QuotePlugin *plug = config.getQuotePlugin(plugin);
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
  QString s("update"); 
  toolbar->setButtonStatus(s, TRUE);
  s = "cancelDownload";
  toolbar->setButtonStatus(s, FALSE);

  QuotePlugin *plug = config.getQuotePlugin(plugin);
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
  QString s("update"); 
  toolbar->setButtonStatus(s, FALSE);
  s = "cancelDownload";
  toolbar->setButtonStatus(s, TRUE);
}

void QuoteDialog::printStatusLogMessage (QString d)
{
  statusLog->append(d);
  emit message(QString());
}

void QuoteDialog::pluginSettings ()
{
  QuotePlugin *plug = config.getQuotePlugin(plugin);
  if (! plug)
  {
    qDebug("QuoteDialog::pluginSettings - could not open plugin");
    return;
  }
  
  plug->prefDialog(this);
}

void QuoteDialog::help ()
{
  QString s = "quotes.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

