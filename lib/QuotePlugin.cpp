/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#include "QuotePlugin.h"
#include <qdir.h>
#include "HelpWindow.h"
#include "Config.h"
#include "../pics/download.xpm"
#include "../pics/canceldownload.xpm"
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <qtabwidget.h>

QuotePlugin::QuotePlugin () : QTabDialog (0, "QuoteDialog", FALSE, 0)
{
  saveFlag = FALSE;
  op = 0;
  chartIndex = 0;
  errorLoop = 0;
  stringDone = tr("Done");
  stringCanceled = tr("Canceled");
  
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));

  buildGui();
}

QuotePlugin::~QuotePlugin ()
{
  if (op)
  {
    op->stop();
    delete op;
  }

  delete timer;
}

void QuotePlugin::setChartIndex (DBIndex *d)
{
  chartIndex = d;
}

void QuotePlugin::buildGui ()
{
  baseWidget = new QWidget(this);
  
  vbox = new QVBoxLayout(baseWidget);
  vbox->setSpacing(2);
  vbox->setMargin(5);
  
  toolbar = new Toolbar(baseWidget, 30, 30, FALSE);
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

  // quote plugins insert their gui widget here

  grid = new QGridLayout(vbox, 1, 2);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  vbox->addSpacing(10);

  QTabWidget *tabs = new QTabWidget(baseWidget);
  vbox->addWidget(tabs);

  addTab(baseWidget, tr("Quotes"));

  setOkButton(tr("&Done"));
  QObject::connect(this, SIGNAL(applyButtonPressed()), this, SLOT(accept()));
  
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));

  // construct status page

  QWidget *w = new QWidget(baseWidget);
  
  QVBoxLayout *tvbox = new QVBoxLayout(w);
  tvbox->setSpacing(2);
  tvbox->setMargin(5);

  QLabel *label = new QLabel(tr("Download Status:"), w);
  tvbox->addWidget(label);
  
  statusLog = new QTextEdit(w);
  statusLog->setTextFormat(Qt::LogText);
  statusLog->setReadOnly(TRUE);
  tvbox->addWidget(statusLog);
  
  tabs->addTab(w, tr("Status"));

  // construct download parms page

  w = new QWidget(baseWidget);
  
  QGridLayout *tgrid = new QGridLayout(w, 3, 2);
  tgrid->setSpacing(2);
  tgrid->setMargin(5);
  tgrid->setColStretch(1, 1);

  label = new QLabel(tr("Retry"), w);
  tgrid->addWidget(label, 0, 0);
  
  retrySpin = new QSpinBox(0, 99, 1, w);
  retrySpin->setValue(3);
  tgrid->addWidget(retrySpin, 0, 1);

  label = new QLabel(tr("Timeout"), w);
  tgrid->addWidget(label, 1, 0);
  
  timeoutSpin = new QSpinBox(0, 99, 1, w);
  timeoutSpin->setValue(15);
  tgrid->addWidget(timeoutSpin, 1, 1);
  
  tabs->addTab(w, tr("Timeout"));
}

void QuotePlugin::stripJunk (QString &d, QString &s)
{
  s = d.stripWhiteSpace();

  while (1)
  {
    int p = s.find('"', 0, TRUE);
    if (p == -1)
      break;
    else
      s.remove(p, 1);
  }
}

bool QuotePlugin::setTFloat (QString &d, bool flag)
{
  QString s = d;
  
  while (s.contains("A"))
    s = s.remove(s.find("A", 0, TRUE), 1);
  
  while (s.contains("B"))
    s = s.remove(s.find("B", 0, TRUE), 1);

  while (s.contains("K"))
  {
    s = s.remove(s.find("K", 0, TRUE), 1);
    s.append("000");
  }

  if (flag)
  {
    while (s.contains(","))
      s = s.replace(s.find(",", 0, TRUE), 1, ".");
  }
  else
  {
    while (s.contains(","))
      s = s.remove(s.find(",", 0, TRUE), 1);
  }

  bool ok;
  tfloat = s.toFloat(&ok);
  if (! ok)
    return TRUE;
  else
    return FALSE;
}

void QuotePlugin::createDirectory (QString &d, QString &path)
{
  Config config;
  config.getData(Config::DataPath, path);
  
  QStringList l = QStringList::split("/", d, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    path.append("/");
    path.append(l[loop]);
    QDir dir(path);
    if (! dir.exists(path, TRUE))
    {
      if (! dir.mkdir(path, TRUE))
      {
        path.truncate(0);
        return;
      }
    }
  }
}

void QuotePlugin::getPluginName (QString &d)
{
  d = pluginName;
}

void QuotePlugin::getHelpFile (QString &d)
{
  d = helpFile;
}

void QuotePlugin::getFile (QString &url)
{
  if (op)
  {
    op->stop();
    delete op;
  }

  data.truncate(0);
  
  timer->start(timeoutSpin->value() * 1000, TRUE);
  
  op = new QUrlOperator(url);
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(getFileDone(QNetworkOperation *)));
  connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)), this, SLOT(dataReady(const QByteArray &, QNetworkOperation *)));
  op->get();
}

void QuotePlugin::copyFile (QString &url, QString &file)
{
  if (op)
  {
    op->stop();
    delete op;
  }
    
  timer->start(timeoutSpin->value() * 1000, TRUE);
  
  QDir dir(file);
  dir.remove(file);

  op = new QUrlOperator();
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(copyFileDone(QNetworkOperation *)));
  op->copy(url, file, FALSE, FALSE);
}

void QuotePlugin::getFileDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
  {
    timer->stop();
    emit signalGetFileDone(FALSE);
    return;
  }

  if (o->state() == QNetworkProtocol::StFailed)
  {
    timer->stop();
    emit signalGetFileDone(TRUE);
  }
}

void QuotePlugin::copyFileDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() != QNetworkProtocol::StDone)
    return;

  if (o->errorCode() != QNetworkProtocol::NoError)
  {
    timer->stop();
    QString s = QObject::tr("Download error: ") + o->protocolDetail();
    emit signalCopyFileDone(s);
    return;
  }
  
  QDir dir(file);
  if (! dir.exists(file, TRUE))
    return;
  
  timer->stop();

  emit signalCopyFileDone(QString());
}

void QuotePlugin::dataReady (const QByteArray &d, QNetworkOperation *)
{
  int loop;
  for (loop = 0; loop < (int) d.size(); loop++)
    data.append(d[loop]);
}

void QuotePlugin::slotTimeout ()
{
  timer->stop();
  if (op)
    op->stop();
  emit signalTimeout();
}

void QuotePlugin::getQuotes ()
{
  statusLog->clear();
  QString s(tr("Starting update..."));
  printStatusLogMessage(s);
  disableGUI();
  update();
}

void QuotePlugin::downloadComplete ()
{
  enableGUI();
  emit chartUpdated();
}

void QuotePlugin::cancelDownload ()
{
  QString s(tr("Update cancelled."));
  printStatusLogMessage(s);
  enableGUI();
}

void QuotePlugin::enableGUI ()
{
  QString s("update"); 
  toolbar->setButtonStatus(s, TRUE);
  s = "cancelDownload";
  toolbar->setButtonStatus(s, FALSE);
}

void QuotePlugin::disableGUI ()
{
  QString s("update"); 
  toolbar->setButtonStatus(s, FALSE);
  s = "cancelDownload";
  toolbar->setButtonStatus(s, TRUE);
}

void QuotePlugin::printStatusLogMessage (QString &d)
{
  statusLog->append(d);
}

void QuotePlugin::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

// virtual function
void QuotePlugin::update ()
{
}

void QuotePlugin::slotWakeup ()
{
  emit signalWakeup();
}

