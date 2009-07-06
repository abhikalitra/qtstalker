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

#include "IndicatorPlugin.h"
#include "DataBase.h"
#include "BARS.h"
#include "UTIL.h"
#include <QtDebug>


IndicatorPlugin::IndicatorPlugin(QString &n, BarData *d)
{
  name = n;
  data = d;

  setDefaults();
}

IndicatorPlugin::IndicatorPlugin()
{
  setDefaults();
}

IndicatorPlugin::~IndicatorPlugin()
{
  TA_Shutdown();  
}

void IndicatorPlugin::setDefaults ()
{
  PlotLine pl;
  pl.getLineTypes(lineTypes);

  maList.append("SMA"); //    TA_MAType_SMA       =0,
  maList.append("EMA"); //    TA_MAType_EMA       =1,
  maList.append("WMA"); //    TA_MAType_WMA       =2,
  maList.append("DEMA"); //    TA_MAType_DEMA      =3,
  maList.append("TEMA"); //    TA_MAType_TEMA      =4,
  maList.append("TRIMA"); //    TA_MAType_TRIMA     =5,
  maList.append("KAMA"); //    TA_MAType_KAMA      =6,
  maList.append("MAMA"); //    TA_MAType_MAMA      =7,
  maList.append("T3"); //    TA_MAType_T3        =8
//  maList.append("Wilder");

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");
}

void IndicatorPlugin::setIndicatorInput (BarData *d)
{
  data = d;
}

void IndicatorPlugin::getMATypes (QStringList &l)
{
  l = maList;
}

void IndicatorPlugin::wakeup ()
{
  emit signalWakeup();
}

void IndicatorPlugin::calculate (QList<PlotLine *> &lines)
{
  qDeleteAll(lines);
  lines.clear();

  TA_Integer start = 0;
  TA_Integer end = data->count() - 1;
  TA_Integer outstart;
  TA_Integer count;
  // sometimes are not enough data available
  // to calc anything
  if (end < 0)
    return;

  QStringList varList;
  QHash<QString, PlotLine *> tlines;

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
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    open[loop] = (TA_Real) data->getOpen(loop);
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Real) data->getVolume(loop);
    oi[loop] = (TA_Real) data->getOI(loop);
  }

  Indicator i;
  i.setName(name);
  DataBase db;
  db.getIndicator(i);

  int mainLoop;
  for (mainLoop = 0; mainLoop < (int) i.count(); mainLoop++)
  {
    IndicatorParms parms;
    i.getParm(mainLoop, parms);

    // open a TALIB handle
    QString s;
    parms.getIndicator(s);
    if (s.isEmpty())
    {
      qDebug() << "IndicatorPlugin::calculate: no indicator";
      continue;
    }

    // intercept any local functions not found in TALIB here
    if (s == "BARS")
    {
      getBARS(parms, tlines);
      continue;
    }

    if (s == "UTIL")
    {
      getUTIL(parms, tlines);
      continue;
    }

    const TA_FuncHandle *handle;
    TA_RetCode retCode = TA_GetFuncHandle((char *) s.toStdString().c_str(), &handle);
    if (retCode != TA_SUCCESS)
    {
      qDebug("IndicatorPlugin::calculateCustom:can't open handle");
      continue;
    }

    // get info on the function
    const TA_FuncInfo *theInfo;
    retCode = TA_GetFuncInfo(handle, &theInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("IndicatorPlugin::calculateCustom:can't get function info");
      continue;
    }

    // create parm holder
    TA_ParamHolder *parmHolder;
    retCode = TA_ParamHolderAlloc(handle, &parmHolder);
    if (retCode != TA_SUCCESS)
    {
      qDebug("IndicatorPlugin::calculateCustom:can't create parm holder");
      continue;
    }

    // setup the input arrays
    s = "Input1";
    QString ts;
    parms.getData(s, ts);
    if (! ts.length())
    {
      retCode = TA_SetInputParamPricePtr(parmHolder, 0, &open[0], &high[0], &low[0], &close[0], &volume[0], &oi[0]);
      if (retCode != TA_SUCCESS)
      {
        qDebug("IndicatorPlugin::calculateCustom:cannot set input prices");
        continue;
      }
    }
    else
    {
      addInputLine(ts, tlines);
      PlotLine *line = tlines.value(ts);
      if (! line)
      {
        qDebug() << "IndicatorPlugin::calculateCustom: " << s << " input1 not found";
        continue;
      }
      int loop2;
      for (loop2 = 0; loop2 < line->getSize(); loop2++)
        in[loop2] = (TA_Real) line->getData(loop2);
      retCode = TA_SetInputParamRealPtr(parmHolder, 0, &in[0]);
      if (retCode != TA_SUCCESS)
      {
        qDebug() << "IndicatorPlugin::calculateCustom: cannot set input1";
        continue;
      }

      s = "Input2";
      parms.getData(s, ts);
      if (ts.length())
      {
        addInputLine(ts, tlines);
        PlotLine *line = tlines.value(ts);
        if (! line)
        {
          qDebug() << "IndicatorPlugin::calculateCustom: " << s << " input 2 not found";
          continue;
        }
        for (loop2 = 0; loop2 < line->getSize(); loop2++)
          in2[loop2] = (TA_Real) line->getData(loop2);
        retCode = TA_SetInputParamRealPtr(parmHolder, 1, &in2[0]);
        if (retCode != TA_SUCCESS)
        {
          qDebug("IndicatorPlugin::calculateCustom: cannot set in2 price");
          continue;
        }
      }
    }

    // setup the optinput parms
    int tint = 0;
    const TA_OptInputParameterInfo *optInfo;
    for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
    {
      TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
      s = optInfo->displayName;
      switch (optInfo->type)
      {
        case TA_OptInput_RealRange:
          parms.getData(s, ts);
          retCode = TA_SetOptInputParamReal(parmHolder, loop, (TA_Real) ts.toDouble());
          if (retCode != TA_SUCCESS) 
            qDebug() << "IndicatorPlugin::calculateCustom: cannot set " << s;
          break;
        case TA_OptInput_IntegerRange:
          parms.getData(s, ts);
          retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) ts.toInt());
          if (retCode != TA_SUCCESS)
            qDebug() << "IndicatorPlugin::calculateCustom: cannot set " << s;
          break;
        case TA_OptInput_IntegerList:
          parms.getData(s, ts);
          tint = maList.indexOf(ts);
          retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) tint);
          if (retCode != TA_SUCCESS)
            qDebug() << "IndicatorPlugin::calculateCustom: cannot set " << s;
          break;
        default:
          break;
      }
    }

    // setup the output arrays
    const TA_OutputParameterInfo *outInfo;
    for (loop = 0; loop < (int) theInfo->nbOutput; loop++)
    {
      retCode = TA_GetOutputParameterInfo(handle, loop, &outInfo);
      if (retCode != TA_SUCCESS)
      {
        qDebug("IndicatorPlugin::calculateCustom: cannot get output info");
        break;
      }

      switch (loop)
      {
        case 0:
          if (outInfo->type == TA_Output_Integer)
          {
            retCode = TA_SetOutputParamIntegerPtr(parmHolder, loop, &out4[0]);
            if (retCode != TA_SUCCESS)
            {
              qDebug("IndicatorPlugin::calculateCustom: cannot set output4");
              break;;
            }
          }
          else
          {
            retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out[0]);
            if (retCode != TA_SUCCESS)
            {
              qDebug("IndicatorPlugin::calculateCustom: cannot set output1");
              break;
            }
          }
          break;      
        case 1:
          retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out2[0]);
          if (retCode != TA_SUCCESS)
          {
            qDebug("IndicatorPlugin::calculateCustom: cannot set output2");
            break;
          }
          break;      
        case 2:
          retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out3[0]);
          if (retCode != TA_SUCCESS)
          {
            qDebug("IndicatorPlugin::calculateCustom: cannot set output3");
            break;
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
      printError(QString("IndicatorPlugin::calculateCustom: TA_CallFunc"), retCode);
      qDebug() << " start=" << start << " end=" << end;
      break;
    }

    // create the plotlines
    int loop2;
    retCode = TA_GetOutputParameterInfo(handle, 0, &outInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("IndicatorPlugin::calculateCustom: cannot get output info");
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
      qDebug("TALIB::calculateCustom:can't delete parm holder");
  }

  createPlot(i, tlines, lines);

  qDeleteAll(tlines);
}

void IndicatorPlugin::createPlot (Indicator &i, QHash<QString, PlotLine *> &tlines, QList<PlotLine *> &lines)
{
  int loop;
  for (loop = 0; loop < i.count(); loop++)
  {
    IndicatorParms parms;
    i.getParm(loop, parms);
    if (! parms.getPlot())
      continue;

    //var name
    QString s;
    parms.getVariable(s);
    PlotLine *pl = tlines.value(s);
    if (! pl)
    {
      qDebug() << "IndicatorPlugin::createPlot: variable missing";
      continue;
    }

    //color
    QColor col;
    parms.getColor(col);
    pl->setColor(col);

    //label
    parms.getLabel(s);
    pl->setLabel(s);

    //linetype
    parms.getLineType(s);
    pl->setType(s);

    PlotLine *tline = new PlotLine;
    tline->copy(pl);
    lines.append(tline);
  }
}

void IndicatorPlugin::setName (QString &d)
{
  name = d;
}

void IndicatorPlugin::getName (QString &d)
{
  d = name;
}

void IndicatorPlugin::getIndicatorList (QStringList &l)
{
  l.clear();

  TA_StringTable *table;
  int loop;
  QStringList cl;

  TA_RetCode retCode = TA_GroupTableAlloc(&table);
  if (retCode == TA_SUCCESS)
  {
    for (loop = 0; loop < (int) table->size; loop++)
      cl.append(table->string[loop]);

    TA_GroupTableFree(table);
  }
  else
    printError(QString("IndicatorPlugin::getIndicatorList: "), retCode);

  for (loop = 0; loop < (int) cl.count(); loop++)
  {
    retCode = TA_FuncTableAlloc((char *) cl[loop].toStdString().c_str(), &table);
    if (retCode == TA_SUCCESS)
    {
      int loop2;
      for (loop2 = 0; loop2 < (int) table->size; loop2++ )
        l.append(table->string[loop2]);

      TA_FuncTableFree(table);
    }
    else
      printError(QString("IndicatorPlugin::getIndicatorList: "), retCode);
  }

  l.append("BARS");
  l.append("UTIL");

  l.sort();
}

/*
PlotLine * IndicatorPlugin::getMA (PlotLine *in, int type, int period)
{
  Wilder's MA:

  if (period >= (int) in->getSize())
    return ma;

  if (period < 1)
    return ma;

  double t = 0;
  int loop;
  for (loop = 0; loop < period; loop++)
    t = t + in->getData(loop);

  double yesterday = t / period;

  ma->append(yesterday);

  for (; loop < (int) in->getSize(); loop++)
  {
    double t  = (yesterday * (period - 1) + in->getData(loop))/period;
    yesterday = t;
    ma->append(t);
  }
}
*/

void IndicatorPlugin::printError (QString es, TA_RetCode rc)
{
  TA_RetCodeInfo info;
  TA_SetRetCodeInfo(rc, &info);
  qDebug() << es << ":" << rc << "(" << info.enumStr << "): " << info.infoStr;
}

void IndicatorPlugin::addInputLine (QString &key, QHash<QString, PlotLine *> &tlines)
{
  PlotLine *tline = tlines.value(key);
  if (tline)
    return;

  tline = data->getInput(data->getInputType(key));
  if (tline)
    tlines.insert(key, tline);
}

void IndicatorPlugin::getBARS (IndicatorParms &p, QHash<QString, PlotLine *> &tlines)
{
  BARS i;
  i.calculate(data, p, tlines);
}

void IndicatorPlugin::getUTIL (IndicatorParms &p, QHash<QString, PlotLine *> &tlines)
{
  UTIL i;
  i.calculate(data, p, tlines);
}


