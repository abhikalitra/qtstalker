/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "ExScript.h"
#include "PrefDialog.h"
#include <QWaitCondition>
#include <QMutex>
#include <QByteArray>
#include <QtDebug>



ExScript::ExScript ()
{
  proc = 0;
}

ExScript::~ExScript ()
{
  if (proc)
    delete proc;
}

void ExScript::calculate (BarData *bd, IndicatorParms &parms)
{
  // clean up if needed
  if (proc)
  {
    delete proc;
    proc = 0;
  }

  lines.clear();

  data = bd;

  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromOutput()));

  // get the script path
  QString scriptPath;
  QString s = tr("Script Path");
  parms.getData(s, scriptPath);

  // start the process
  proc->start(scriptPath, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! proc->waitForStarted(30000))
  {
    qDebug() << "ExScript::calculate: error starting script";
    delete proc;
    proc = 0;
    return;
  }

//  wakeup();

  while (proc->state() == QProcess::Running)
  {
    QWaitCondition w;
    QMutex sleepmutex;
    sleepmutex.lock();
    w.wait(&sleepmutex, 100);
    sleepmutex.unlock();
//    wakeup();
  }
}

void ExScript::sendBarData (QString &)
{
  int loop;
  QStringList l;
  for (loop = 0; loop < data->count(); loop++)
  {
    QString s = QString::number(data->getClose(loop));
    l.append(s);
  }

  QString s = l.join("/n");
  QByteArray ba;
  ba.append(s);
  proc->write(ba);
}

void ExScript::readFromStdout ()
{
  QByteArray ba = proc->readAllStandardOutput();
  QString s(ba);

  if (s.contains(","))
  {
    plot(s);
    return;
  }

  if (s == "Close")
    sendBarData(s);
}

void ExScript::plot (QString &s)
{
  QStringList l = s.split(",");
  if (l.count() < 3)
    return;

  PlotLine *line = new PlotLine();
  line->setLabel(l[0]);
  line->setColor(l[1]);
  line->setType(l[2]);

  int loop;
  for (loop = 3; loop < l.count(); loop++)
    line->append(l[loop].toDouble());

  lines.append(line);
}

void prefDialog (IndicatorParms &parms, QStringList &)
{
  QString spl = QObject::tr("Script Path");
//  QString dl = QObject::tr("Date");
//  QString ol = QObject::tr("Open");
//  QString hl = QObject::tr("High");
//  QString lol = QObject::tr("Low");
//  QString cll = QObject::tr("Close");
//  QString vl = QObject::tr("Volume");
//  QString oil = QObject::tr("Open Interest");

    PrefDialog *dialog = new PrefDialog(0);
  dialog->setWindowTitle(QObject::tr("ExScript"));

  QString s;
  parms.getData(spl, s);
  QStringList l;
  l.append(s);
  dialog->addFileItem(spl, l, s);

/*
  parms.getData(dl, s);
  dialog->addCheckItem(dl, s.toInt());

  parms.getData(ol, s);
  dialog->addCheckItem(ol, s.toInt());

  parms.getData(hl, s);
  dialog->addCheckItem(hl, s.toInt());

  parms.getData(lol, s);
  dialog->addCheckItem(lol, s.toInt());

  parms.getData(cll, s);
  dialog->addCheckItem(cll, s.toInt());

  parms.getData(vl, s);
  dialog->addCheckItem(vl, s.toInt());

  parms.getData(oil, s);
  dialog->addCheckItem(oil, s.toInt());
*/
  
  int rc = dialog->exec();
  
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  dialog->getFile(spl, l);
  if (l.count())
    parms.setData(spl, l[0]);

/*
  s = QString::number(dialog->getCheck(dl));
  parms.setData(dl, s);

  s = QString::number(dialog->getCheck(ol));
  parms.setData(ol, s);

  s = QString::number(dialog->getCheck(hl));
  parms.setData(hl, s);

  s = QString::number(dialog->getCheck(lol));
  parms.setData(lol, s);

  s = QString::number(dialog->getCheck(cll));
  parms.setData(cll, s);

  s = QString::number(dialog->getCheck(vl));
  parms.setData(vl, s);

  s = QString::number(dialog->getCheck(oil));
  parms.setData(oil, s);
*/
  
  delete dialog;
}

