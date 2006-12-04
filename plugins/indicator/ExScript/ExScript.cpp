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

#include "ExScript.h"
#include "PrefDialog.h"
#include "ExScriptThread.h"
#include <qdict.h>
#include <qcstring.h>

ExScript::ExScript ()
{
  pluginName = "ExScript";
  helpFile = "exscript.html";
  colorLabel = "color";
  labelLabel = "label";	
  lineTypeLabel = "lineType";	
  scriptPathLabel = "scriptPath";	
  comlineParmsLabel = "comlineParms";	
  pluginLabel = "plugin";

  formatList.append(FormatString);
  formatList.append(FormatString);

  proc = 0;

  setDefaults();
}

ExScript::~ExScript ()
{
  if (proc)
    delete proc;
}

void ExScript::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  label = pluginName;
}

void ExScript::calculate ()
{
  if (proc)
  {
    delete proc;
    proc = 0;
  }

  if (! scriptPath.length())
  {
    qDebug("ExScript::calculate: no script path");
    return;
  }

  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStdout()), this, SLOT(readFromStdout()));

  proc->addArgument(scriptPath);

  QStringList l = QStringList::split(" ", comlineParms, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    proc->addArgument(l[loop]);

  buffer.truncate(0);

  if (! proc->start())
  {
    qDebug("ExScriptThread::calculate: error starting script");
    delete proc;
    proc = 0;
    return;
  }

  ExScriptThread t;
  while (proc->isRunning())
  {
    t.run();
    t.wait();
  }

  createOutput();
}

void ExScript::createOutput ()
{
  if (proc)
  {
    delete proc;
    proc = 0;
  }

  if (! buffer.length())
    return;

  QStringList l = QStringList::split(",", buffer, FALSE);
  PlotLine *line = new PlotLine();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    line->append(l[loop].toDouble());

  line->setColor(color);
  line->setType(lineType);
  line->setLabel(label);
  output->addLine(line);
}

void ExScript::readFromStdout ()
{
  QByteArray ba = proc->readStdout();
  int loop;
  for (loop = 0; loop < (int) ba.size(); loop++)
    buffer.append(ba[loop]);
}

int ExScript::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString spl = QObject::tr("Script Path");
  QString clsl = QObject::tr("Switches");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("ExScript Indicator"));
  dialog->setHelpFile(helpFile);

  dialog->createPage (pl);
  QStringList l;
  dialog->addFileItem(spl, pl, l, scriptPath);
  dialog->addTextItem(clsl, pl, comlineParms);
  dialog->addColorItem(cl, pl, color);
  dialog->addTextItem(ll, pl, label);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getFile(spl, l);
    if (l.count())
      scriptPath = l[0];
    dialog->getText(clsl, comlineParms);
    dialog->getColor(cl, color);
    dialog->getText(ll, label);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void ExScript::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s;
  dict.getData(colorLabel, s);
  if (s.length())
    color.setNamedColor(s);

  dict.getData(labelLabel, s);
  if (s.length())
    label = s;

  dict.getData(lineTypeLabel, s);
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  dict.getData(scriptPathLabel, s);
  if (s.length())
    scriptPath = s;

  dict.getData(comlineParmsLabel, s);
  if (s.length())
    comlineParms = s;
}

void ExScript::getIndicatorSettings (Setting &dict)
{
  QString ts = color.name();
  dict.setData(colorLabel, ts);
  dict.setData(labelLabel, label);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  dict.setData(scriptPathLabel, scriptPath);
  dict.setData(comlineParmsLabel, comlineParms);
  dict.setData(pluginLabel, pluginName);
}

PlotLine * ExScript::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: SCRIPT_PATH, COMMAND_LINE_SWITCHES

  if (checkFormat(p, d, 2, 2))
    return 0;

  scriptPath = formatStringList[0];
  comlineParms = formatStringList[1];

  clearOutput();
  calculate();
  return output->getLine(0);
}

void ExScript::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);
  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString spl = QObject::tr("Script Path");
  QString clsl = QObject::tr("Switches");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("ExScript Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);
  QStringList l;
  dialog->addFileItem(spl, pl, l, scriptPath);
  dialog->addTextItem(clsl, pl, comlineParms);

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    dialog->getFile(spl, l);
    if (l.count())
      rs = l[0];
    dialog->getText(clsl, comlineParms);
    rs.append("," + comlineParms);
  }

  delete dialog;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ExScript *o = new ExScript;
  return ((IndicatorPlugin *) o);
}


