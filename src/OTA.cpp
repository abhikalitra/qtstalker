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

#include <QtDebug>
#include <QApplication>
#include <QSettings>
#include <QStatusBar>
#include <QInputDialog>
#include <QToolButton>
#include <QProcess>
#include <QFileDialog>

#include "OTA.h"
#include "PluginFactory.h"
#include "Setup.h"
#include "Global.h"
#include "LaunchPluginDialog.h"
#include "Widget.h"
#include "Doc.h"

#include "../pics/qtstalker.xpm"
#include "../pics/script.xpm"
#include "../pics/new.xpm"

OTA::OTA (QString session, QString plugin)
{
  _helpFile = "main.html";
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutDown()));

  setWindowIcon(QIcon(qtstalker_xpm));

  QCoreApplication::setOrganizationName("OTA");
  QCoreApplication::setOrganizationDomain("OTA.com");
  QCoreApplication::setApplicationName("OTA");

  Setup setup;
  setup.setup(session);


  QSettings settings(g_settings);
  QDir dir = QDir(settings.value("plugin_directory").toString());

  qDebug() << "plugin dir:" << settings.value("plugin_directory").toString();

  if (!dir.exists()) {
      qDebug("plugin scan failed as dir not found!");
      QString plugindir = QFileDialog::getExistingDirectory(this, "Choose plugin directory", ".", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
      settings.setValue("plugin_directory", plugindir);
      settings.sync();
      qDebug() << "New plugin dir:" << plugindir;
  }

  setup.scanPlugins();

  createGUI();

  loadSettings();

  setWindowTitle(g_title);

  QString tplugin = plugin;
  if (tplugin.isEmpty())
  {
    QSettings settings(g_settings);
    settings.beginGroup(g_session);
    tplugin = settings.value("plugin").toString();

    if (tplugin.isEmpty()) {
        tplugin = "MACD";
    }
  }

  qDebug() << "Loading:" << tplugin;

  loadPlugin(tplugin);
}

void
OTA::shutDown ()
{
  save();
}

void
OTA::createGUI ()
{
  statusBar()->showMessage(tr("Ready"), 10000);
  
  // add buttons to statusbar
  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(script_xpm));
  b->setToolTip(tr("Load Plugin"));
  b->setStatusTip(tr("Load Plugin"));
  connect(b, SIGNAL(clicked()), this, SLOT(pluginDialog()));
  statusBar()->addPermanentWidget(b, 0);
  b->setMaximumSize(QSize(15, 15));
  
  b = new QToolButton;
  b->setIcon(QIcon(new_xpm));
  b->setToolTip(tr("New Session"));
  b->setStatusTip(tr("New Session"));
  connect(b, SIGNAL(clicked()), this, SLOT(newPluginDialog()));
  statusBar()->addPermanentWidget(b, 0);
  b->setMaximumSize(QSize(15, 15));
  
  b = new QToolButton;
  b->setIcon(QIcon(script_xpm));
  b->setToolTip(tr("Run Session"));
  b->setStatusTip(tr("Run Session"));
  connect(b, SIGNAL(clicked()), this, SLOT(runSession()));
  statusBar()->addPermanentWidget(b, 0);
  b->setMaximumSize(QSize(15, 15));

  setUnifiedTitleAndToolBarOnMac(TRUE);
}

void
OTA::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  
  restoreGeometry(settings.value("main_window_geometry").toByteArray());
  
  restoreState(settings.value("main_window_state").toByteArray());

  // restore the size of the app
  QSize sz = settings.value("main_window_size", QSize(500,300)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("main_window_pos", QPoint(0,0)).toPoint();
  move(p);
}

void
OTA::save()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  
  settings.setValue("main_window_geometry", saveGeometry());
  settings.setValue("main_window_state", saveState());
  settings.setValue("main_window_size", size());
  settings.setValue("main_window_pos", pos());
}

void
OTA::statusMessage (QString d)
{
  // update the status bar with a new message from somewhere
  statusBar()->showMessage(d, 0);
  wakeup();
}

void
OTA::wakeup ()
{
  // force app to process the event que so we keep from any blocking
  qApp->processEvents();
}

void
OTA::newPluginDialog ()
{
  LaunchPluginDialog *dialog = new LaunchPluginDialog(this);
  dialog->show();
}

void
OTA::pluginDialog ()
{
  QSettings settings(g_settings);
  
  settings.beginGroup("plugins");
  QStringList l = settings.value("gui").toStringList();
  settings.endGroup();
  
  settings.beginGroup(g_session);
  QString plugin = settings.value("plugin").toString();

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setComboBoxEditable(FALSE);
  dialog->setLabelText(tr("Select plugin to open"));
  dialog->setTextValue(plugin);
  connect(dialog, SIGNAL(textValueSelected(QString)), this, SLOT(pluginDialog2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void
OTA::pluginDialog2 (QString d)
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("plugin", d);
  settings.sync();
  
  loadPlugin(d);
}

void
OTA::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

int
OTA::loadPlugin (QString d)
{
  if (d.isEmpty())
    return 0;
  
  PluginFactory fac;
  Plugin *plugin = fac.load(d);
  if (! plugin)
    return 0;

  Widget *cw = (Widget *) centralWidget();
  if (cw)
  {
    QToolBar *tb = cw->toolbar();
    if (tb)
    {
      removeToolBar(cw->toolbar());
      delete tb;
    }
    
    delete cw;
  }

  PluginData pd;
  pd.command = QString("gui");
  if (plugin->command(&pd))
  {
    setCentralWidget(pd.gui);
    
//    connect(_pluginGUI, SIGNAL(signalEnable(bool)), _toolbar, SLOT(setEnabled(bool)));
    connect(pd.gui, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
    connect(pd.gui, SIGNAL(signalTitle(QString)), this, SLOT(setWindowTitle(QString)));
    connect(pd.gui, SIGNAL(signalHelp()), this, SLOT(help()));
    
    QToolBar *tb = pd.gui->toolbar();
    if (tb)
      addToolBar(tb);
  }

  return 1;
}

void
OTA::runSession ()
{
  QSettings settings(g_settings);
  QStringList l = settings.childGroups();
  if (! l.size())
    return;
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setComboBoxEditable(FALSE);
  dialog->setLabelText(tr("Select session to run"));
  dialog->setTextValue(l.at(0));
  connect(dialog, SIGNAL(textValueSelected(QString)), this, SLOT(runSession2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void
OTA::runSession2 (QString session)
{
  QStringList tl;
  tl << "ota" << "-session" << session;
  
  if (! QProcess::startDetached(tl.join(" ")))
    qDebug() << "OTA::runSession2: error launching process" << tl;
}
