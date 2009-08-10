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
#include "ta_libc.h"

#include <QWaitCondition>
#include <QMutex>
#include <QByteArray>
#include <QtDebug>
#include <QCoreApplication>



ExScript::ExScript ()
{
  proc = 0;
}

void ExScript::calculate (BarData *bd, IndicatorParms &parms)
{
  // clean up if needed
  if (proc)
    delete proc;

  lines.clear();

  data = bd;

  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));

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

  QCoreApplication::processEvents(QEventLoop::AllEvents);

  while (proc->state() == QProcess::Running)
  {
    QWaitCondition w;
    QMutex sleepmutex;
    sleepmutex.lock();
    w.wait(&sleepmutex, 100);
    sleepmutex.unlock();
    QCoreApplication::processEvents(QEventLoop::AllEvents);
  }

  delete proc;
  proc = 0;
}

void ExScript::sendBarData (QString &s)
{
  QString ts = s;
  ts.remove("getBarData(", Qt::CaseSensitive);
  ts.remove(")", Qt::CaseSensitive);
 
  QStringList l = ts.split(",");

  int loop;
  QStringList l2;
  for (loop = 0; loop < data->count(); loop++)
  {
    int loop2;
    QStringList l3;
    for (loop2 = 0; loop2 < l.count(); loop2++)
    {
      if (l[loop2] == "D")
      {
        QString ts2;
        data->getDateTimeString(loop, ts2);
        l3.append(ts2);
        continue;
      }

      if (l[loop2] == "O")
      {
        l3.append(QString::number(data->getOpen(loop)));
        continue;
      }

      if (l[loop2] == "H")
      {
        l3.append(QString::number(data->getHigh(loop)));
        continue;
      }

      if (l[loop2] == "L")
      {
        l3.append(QString::number(data->getLow(loop)));
        continue;
      }

      if (l[loop2] == "C")
      {
        l3.append(QString::number(data->getClose(loop)));
        continue;
      }

      if (l[loop2] == "V")
      {
        l3.append(QString::number(data->getVolume(loop)));
        continue;
      }

      if (l[loop2] == "I")
        l3.append(QString::number(data->getOI(loop)));
    }

    l2.append(l3.join(","));
  }

  QByteArray ba;
  ba.append(l2.join(","));
  proc->write(ba);
}

void ExScript::readFromStdout ()
{
  QByteArray ba = proc->readAllStandardOutput();
  QString s(ba);

  if (s.contains("getBarData("))
  {
    sendBarData(s);
    return;
  }

  if (s.contains("plot("))
  {
    plot(s);
    return;
  }

  parseIndicator(s);
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

  PrefDialog *dialog = new PrefDialog(0);
  dialog->setWindowTitle(QObject::tr("ExScript"));

  QString s;
  parms.getData(spl, s);
  QStringList l;
  l.append(s);
  dialog->addFileItem(spl, l, s);

  int rc = dialog->exec();
  
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  dialog->getFile(spl, l);
  if (l.count())
    parms.setData(spl, l[0]);
  
  delete dialog;
}

 
void ExScript::parseIndicator (QString &s)
{
  if (! s.contains("(") || ! s.contains(")"))
  {
    qDebug() << "ExScript::parseIndicator: error in function request, missing open/close bracket" << s;
    return;
  }

  QStringList parms = s.split("(");
  QString function = parms[0];
  if (function.isEmpty())
  {
    qDebug() << "ExScript::parseIndicator: error in function request, missing function name" << s;
    return;
  }

/*
  parms = parms[1].split(")");
  parms = parms[0].split(",");

  TA_Integer start = 0;
  TA_Integer end = data->count() - 1;
  TA_Integer outstart;
  TA_Integer count;
  // sometimes there is not enough data available to calc anything
  if (end < 0)
  {
    qDebug() << "ExScript::parseIndicator: bars empty";
    return;
  }

  // create and input arrays
  int size = data->count();
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Real oi[size];
  TA_Real in[size];
  TA_Real in2[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
  TA_Integer out4[size];

  // open a TALIB handle
  const TA_FuncHandle *handle;
  TA_RetCode retCode = TA_GetFuncHandle((char *) function.toStdString().c_str(), &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("ExScript::parseIndicator: can't TALIB open handle");
    return;
  }

  // get info on the function
  const TA_FuncInfo *theInfo;
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("ExScript::parseIndicator: can't get function info");
    return;
  }

  // create parm holder
  TA_ParamHolder *parmHolder;
  retCode = TA_ParamHolderAlloc(handle, &parmHolder);
  if (retCode != TA_SUCCESS)
  {
    qDebug("ExScript::parseIndicator: can't create parm holder");
    return;
  }

  // check for any price bar inputs
  const TA_InputParameterInfo *inputParms;
  int loop;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    if (inputParms->type == TA_Input_Price)
    {
      int tloop;
      for (tloop = 0; tloop < size; tloop++)
      {
        open[tloop] = (TA_Real) data->getOpen(tloop);
        high[tloop] = (TA_Real) data->getHigh(tloop);
        low[tloop] = (TA_Real) data->getLow(tloop);
        close[tloop] = (TA_Real) data->getClose(tloop);
        volume[tloop] = (TA_Real) data->getVolume(tloop);
        oi[tloop] = (TA_Real) data->getOI(tloop);
      }

      retCode = TA_SetInputParamPricePtr(parmHolder, 0, &open[0], &high[0], &low[0], &close[0], &volume[0], &oi[0]);
      if (retCode != TA_SUCCESS)
      {
        qDebug() << "ExScript::parseIndicator: cannot set input bars for" << function;
        return;
      }
    }
  }

  // setup the optinput parms
  int tint = 0;
  int parmLoop = 0;
  const TA_OptInputParameterInfo *optInfo;
  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        retCode = TA_SetOptInputParamReal(parmHolder, loop, (TA_Real) parms[parmLoop].toDouble());
        if (retCode != TA_SUCCESS) 
        {
          qDebug() << "ExScript::parseIndicator: cannot set optinfo parm#" << parms[parmLoop];
          return;
        }
        parmLoop++;
        break;
      case TA_OptInput_IntegerRange:
        retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) parms[parmLoop].toInt());
        if (retCode != TA_SUCCESS)
        {
          qDebug() << "ExScript::parseIndicator: cannot set optinfo parm#" << parms[parmLoop];
          return;
        }
        parmLoop++;
        break;
      case TA_OptInput_IntegerList:
        tint = maList.indexOf(parms[parmLoop]);
        retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) tint);
        if (retCode != TA_SUCCESS)
        {
          qDebug() << "ExScript::parseIndicator: cannot set optinfo parm#" << parms[parmLoop];
          return;
        }
        parmLoop++;
        break;
      default:
        break;
    }
  }

  // check for input data and copy data into arrays
  // fill the first input array if requested
  if (theInfo->nbInput > 0)
  {
    QStringList l = inputData1.split(",");

    for (loop = 0; loop < l.count(); loop++)
      in[loop] = (TA_Real) l[loop].toDouble();

    retCode = TA_SetInputParamRealPtr(parmHolder, 0, &in[0]);
    if (retCode != TA_SUCCESS)
    {
      qDebug() << "ExScript::parseIndicator: cannot set input1";
      return;
    }
  }

  // fill the second input array if requested
  if (theInfo->nbInput > 1)
  {
    QStringList l = inputData2.split(",");

    for (loop = 0; loop < l.count(); loop++)
      in2[loop] = (TA_Real) l[loop].toDouble();

    retCode = TA_SetInputParamRealPtr(parmHolder, 0, &in2[0]);
    if (retCode != TA_SUCCESS)
    {
      qDebug() << "ExScript::parseIndicator: cannot set input2";
      return;
    }
  }

  // setup the output arrays
  const TA_OutputParameterInfo *outInfo;
  for (loop = 0; loop < (int) theInfo->nbOutput; loop++)
  {
    retCode = TA_GetOutputParameterInfo(handle, loop, &outInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("ExScript::parseIndicator: cannot get output info");
      return;
    }

    switch (loop)
    {
      case 0:
        if (outInfo->type == TA_Output_Integer)
        {
          retCode = TA_SetOutputParamIntegerPtr(parmHolder, loop, &out4[0]);
          if (retCode != TA_SUCCESS)
          {
            qDebug("ExScript::parseIndicator: cannot set output4");
            return;
          }
        }
        else
        {
          retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out[0]);
          if (retCode != TA_SUCCESS)
          {
            qDebug("ExScript::parseIndicator: cannot set output1");
            return;
          }
        }
        break;      
      case 1:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out2[0]);
        if (retCode != TA_SUCCESS)
        {
          qDebug("ExScript::parseIndicator: cannot set output2");
          return;
        }
        break;      
      case 2:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out3[0]);
        if (retCode != TA_SUCCESS)
        {
          qDebug("ExScript::parseIndicator: cannot set output3");
          return;
        }
        break;
      default:
        break;
    }
  }

  // call the function
  retCode = TA_CallFunc(parmHolder, start, end, &outstart, &count);
  if (retCode != TA_SUCCESS)
  {
    printError(QString("ExScript::parseIndicator: TA_CallFunc"), retCode);
    qDebug() << " start=" << start << " end=" << end;
    return;
  }

  // create the plotlines
  int loop2;
  retCode = TA_GetOutputParameterInfo(handle, 0, &outInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("IndicatorPlugin::calculate: cannot get output info");
    break;
  }
    
    if (outInfo->type == TA_Output_Integer)
    {
      PlotLine *line = new PlotLine;
      for (loop2 = 0; loop2 < count; loop2++)
        line->append((double) out4[loop2]);
      parms.getVariable(ts);
      tlines.insert(ts, line);
      continue;
    }

    s = "Output";
    parms.getData(s, ts);
    PlotLine *line = new PlotLine;
    switch (ts.toInt())
    {
      case 1:
        for (loop2 = 0; loop2 < count; loop2++)
          line->append((double) out2[loop2]);
        break;
      case 2:
        for (loop2 = 0; loop2 < count; loop2++)
          line->append((double) out3[loop2]);
        break;
      default:
        for (loop2 = 0; loop2 < count; loop2++)
          line->append((double) out[loop2]);
        break;
    }

    parms.getVariable(ts);
    tlines.insert(ts, line);

    retCode = TA_ParamHolderFree(parmHolder);
    if (retCode != TA_SUCCESS)
      qDebug("TALIB::calculate:can't delete parm holder");
  }
*/
}

