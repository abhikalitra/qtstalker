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

#include "TALIB.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <qinputdialog.h>
#include "ta_libc.h"

TALIB::TALIB ()
{
  pluginName = "TALIB";
  helpFile = "talib.html";
  setDefaults();
}

TALIB::~TALIB ()
{
}

void TALIB::setDefaults ()
{
  getIndicatorList(methodList);
}

void TALIB::calculate ()
{
  // open a TALIB handle
  const TA_FuncHandle *handle;
  TA_RetCode retCode = TA_GetFuncHandle(parms.getData("method"), &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculate:can't open handle");
    return;
  }

  // get info on the function
  const TA_FuncInfo *theInfo;
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculate:can't get function info");
    return;
  }

  // create parm holder
  TA_ParamHolder *parmHolder;
  retCode = TA_ParamHolderAlloc(handle, &parmHolder);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculate:can't create parm holder");
    return;
  }

  // create and populate the input arrays
  int loop = data->count();
  TA_Real open[loop];
  TA_Real high[loop];
  TA_Real low[loop];
  TA_Real close[loop];
  TA_Integer volume[loop];
  TA_Integer oi[loop];
  for (loop = 0; loop < data->count(); loop++)
  {
    open[loop] = (TA_Real) data->getOpen(loop);
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Integer) data->getVolume(loop);
    oi[loop] = (TA_Integer) data->getOI(loop);
  }

  // setup the input arrays
  const TA_InputParameterInfo *inputParms;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    QString s;
    int tint = 3;
    switch (inputParms->type)
    {
      case TA_Input_Price:
        retCode = TA_SetInputParamPricePtr(parmHolder, loop, 0, &open[0], &high[0], &low[0], &close[0], &volume[0], &oi[0]);
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set input prices");
        break;
      case TA_Input_Real:
        s = QObject::tr("input") + QString::number(loop + 1);
        tint = parms.getInt(s);
        switch (tint)
        {
          case 0:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &open[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set real price");
            break;
          case 1:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &high[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set real price");
            break;
          case 2:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &low[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set real price");
            break;
          case 3:
            retCode = TA_SetInputParamRealPtr(parmHolder, loop, &close[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set real price");
            break;
          case 4:
            retCode = TA_SetInputParamIntegerPtr(parmHolder, loop, &volume[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set integer price");
            break;
          case 5:
            retCode = TA_SetInputParamIntegerPtr(parmHolder, loop, &oi[0]);
            if (retCode != TA_SUCCESS)
              qDebug("TALIB::calculate:cannot set integer price");
            break;
          default:
            break;
        }
        break;
      case TA_Input_Integer:
        break;
      default:
        break;
    }
  }

  // setup the optinput parms
  const TA_OptInputParameterInfo *optInfo;
  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    QString s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        retCode = TA_SetOptInputParamReal(parmHolder, loop, (TA_Real) parms.getDouble(s));
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set inputopt real");
        break;
      case TA_OptInput_IntegerRange:
        retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) parms.getInt(s));
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set inputopt integer");
        break;
      default:
        break;
    }
  }

  // setup the output arrays
  TA_Real out1[data->count()];
  TA_Real out2[data->count()];
  TA_Real out3[data->count()];
  TA_Integer out4[data->count()];
  const TA_OutputParameterInfo *outInfo;
  for (loop = 0; loop < (int) theInfo->nbOutput; loop++)
  {
    retCode = TA_GetOutputParameterInfo(handle, loop, &outInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("TALIB::calculate:cannot get output info");
      continue;
    }

    switch (loop)
    {
      case 0:
        if (outInfo->type == TA_Output_Integer)
        {
          retCode = TA_SetOutputParamIntegerPtr(parmHolder, loop, &out4[0]);
          if (retCode != TA_SUCCESS)
            qDebug("TALIB::calculate:cannot set output4");
        }
        else
        {
          retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out1[0]);
          if (retCode != TA_SUCCESS)
            qDebug("TALIB::calculate:cannot set output1");
        }
        break;      
      case 1:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out2[0]);
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set output2");
        break;      
      case 2:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out3[0]);
        if (retCode != TA_SUCCESS)
          qDebug("TALIB::calculate:cannot set output3");
        break;
      default:
        break;
    }
  }

  // call the function
  TA_Integer start = 0;
  TA_Integer end = data->count() - 1;
  TA_Integer outstart;
  TA_Integer count;
  retCode = TA_CallFunc(parmHolder, start, end, &outstart, &count);
  if (retCode != TA_SUCCESS)
    qDebug("TALIB::calculate:call function failed");
  else
  {
    // create the plotlines
    clearOutput();

    for (loop = 0; loop < (int) theInfo->nbOutput; loop++ )
    {
      PlotLine *line = new PlotLine;
      output->addLine(line);

      QString s = QObject::tr("Color") + QString::number(loop + 1);
      QColor color(parms.getData(s));
      line->setColor(color);

      s = QObject::tr("Label") + QString::number(loop + 1);
      s = parms.getData(s);
      line->setLabel(s);

      s = QObject::tr("Line Type") + QString::number(loop + 1);
      line->setType((PlotLine::LineType)parms.getInt(s));

      retCode = TA_GetOutputParameterInfo(handle, loop, &outInfo);
      if (retCode != TA_SUCCESS)
      {
        qDebug("TALIB::calculate:cannot get output info");
        continue;
      }

      int loop2;
      switch (loop)
      {
        case 0:
          if (outInfo->type == TA_Output_Integer)
          {
            for (loop2 = 0; loop2 < count; loop2++)
              line->append((double) out4[loop2]);
          }
          else
          {
            for (loop2 = 0; loop2 < count; loop2++)
              line->append((double) out1[loop2]);
          }
          break;      
        case 1:
          for (loop2 = 0; loop2 < count; loop2++)
            line->append((double) out2[loop2]);
          break;      
        case 2:
          for (loop2 = 0; loop2 < count; loop2++)
            line->append((double) out3[loop2]);
          break;
        default:
          break;
      }
    }
  }

  retCode = TA_ParamHolderFree(parmHolder);
  if (retCode != TA_SUCCESS)
    qDebug("TALIB::calculate:can't delete parm holder");
}

int TALIB::indicatorPrefDialog (QWidget *w)
{
  bool newFlag = FALSE;
  if (! parms.count())
    newFlag = TRUE;

  if (newFlag)
  {
    bool ok;
    QString s = QInputDialog::getItem(QObject::tr("TALIB Indicator Selection"),
                                      QObject::tr("Select an indicator:"),
                                      methodList,
                                      0,
                                      TRUE,
                                      &ok,
                                      w);
    if (ok)
    {
      QString s2 = "method";
      parms.setData(s2, s);
    }
    else
      return FALSE;
  }

  const TA_FuncHandle *handle;
  const TA_FuncInfo *theInfo;
 
  // open a TALIB handle
  TA_RetCode retCode = TA_GetFuncHandle(parms.getData("method"), &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::indicatorPrefDialog:can't open handle");
    return FALSE;
  }

  // get info on the function
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::indicatorPrefDialog:can't get function info");
    return FALSE;
  }

  QString pl = QObject::tr("Parms");
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("TALIB Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  // get the input parms
  const TA_OptInputParameterInfo *optInfo;
  int loop;
  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    QString s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        if (! parms.getData(s).length())
        {
          dialog->addDoubleItem(s, pl, (double) optInfo->defaultValue, 0, 99999999);
          parms.setData(s, QString::number((double) optInfo->defaultValue));
        }
        else
          dialog->addDoubleItem(s, pl, parms.getDouble(s), 0, 99999999);
        break;
      case TA_OptInput_RealList:
        break;
      case TA_OptInput_IntegerRange:
        if (! parms.getData(s).length())
        {
          dialog->addIntItem(s, pl, (int) optInfo->defaultValue, 1, 999999);
          parms.setData(s, QString::number((int) optInfo->defaultValue));
        }
        else
          dialog->addIntItem(s, pl, parms.getInt(s), 1, 999999);
        break;
      case TA_OptInput_IntegerList:
        break;
      default:
        break;
    }
  }

  // check for any array inputs
  const TA_InputParameterInfo *inputParms;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    QString s = QObject::tr("input") + QString::number(loop + 1);
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    switch (inputParms->type)
    {
      case TA_Input_Real:
        if (! parms.getData(s).length())
        {
          dialog->addComboItem(s, pl, inputTypeList, 3);
          parms.setData(s, QString::number(3));
        }
        else
          dialog->addComboItem(s, pl, inputTypeList, parms.getInt(s));
        break;
      default:
        break;
    }
  }

  // get the output parms
  for (loop = 0; loop < (int) theInfo->nbOutput; loop++ )
  {
    pl = QObject::tr("Plot") + QString::number(loop + 1);
    dialog->createPage (pl);

    QString s = QObject::tr("Color") + QString::number(loop + 1);
    QColor color("red");
    if (! parms.getData(s).length())
    {
      dialog->addColorItem(s, pl, color);
      parms.setData(s, color.name());
    }
    else
    {
      color.setNamedColor(parms.getData(s));
      dialog->addColorItem(s, pl, color);
    }

    s = QObject::tr("Label") + QString::number(loop + 1);
    if (! parms.getData(s).length())
    {
      dialog->addTextItem(s, pl, s);
      parms.setData(s, s);
    }
    else
    {
      QString s2 = parms.getData(s);
      dialog->addTextItem(s, pl, s2);
    }

    s = QObject::tr("Line Type") + QString::number(loop + 1);
    if (! parms.getData(s).length())
    {
      dialog->addComboItem(s, pl, lineTypes, 4);
      parms.setData(s, QString::number(4));
    }
    else
      dialog->addComboItem(s, pl, lineTypes, parms.getInt(s));
  }

  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QStringList l;
    parms.getKeyList(l);
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      if (l[loop].compare("method"))
        parms.setData(l[loop], dialog->getItem(l[loop]));
    }

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void TALIB::getIndicatorSettings (Setting &dict)
{
  QString s;
  parms.getString(s);
  dict.parse(s);
  dict.setData("plugin", pluginName);
}

void TALIB::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;

  QString s;
  dict.getString(s);
  parms.parse(s);
}

void TALIB::getIndicatorList (QStringList &l)
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

  for (loop = 0; loop < (int) cl.count(); loop++)
  {
    retCode = TA_FuncTableAlloc(cl[loop], &table);
    if (retCode == TA_SUCCESS)
    {
      int loop2;
      for (loop2 = 0; loop2 < (int) table->size; loop2++ )
        l.append(table->string[loop2]);

      TA_FuncTableFree(table);
    }
  }

  l.sort();
}

PlotLine * TALIB::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format: METHOD, ..., ...., ..... etc (first parm must be the method)

  QStringList l = QStringList::split(",", p, FALSE);

  if (! l.count())
  {
    qDebug("TALIB::calculateCustom: no method parm");
    return 0;
  }

  // open a TALIB handle
  const TA_FuncHandle *handle;
  TA_RetCode retCode = TA_GetFuncHandle(l[0], &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculateCustom:can't open handle");
    return 0;
  }

  // get info on the function
  const TA_FuncInfo *theInfo;
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculateCustom:can't get function info");
    return 0;
  }

  // create parm holder
  TA_ParamHolder *parmHolder;
  retCode = TA_ParamHolderAlloc(handle, &parmHolder);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::calculateCustom:can't create parm holder");
    return 0;
  }

  // create and input arrays
  int loop = data->count();
  TA_Real open[loop];
  TA_Real high[loop];
  TA_Real low[loop];
  TA_Real close[loop];
  TA_Integer volume[loop];
  TA_Integer oi[loop];
  TA_Real treal[loop];

  int sparmIndex = 1;

  // setup the input arrays
  const TA_InputParameterInfo *inputParms;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);

    if (inputParms->type == TA_Input_Price)
    {
      // populate the input arrays
      int loop2;
      for (loop2 = 0; loop2 < data->count(); loop2++)
      {
        open[loop2] = (TA_Real) data->getOpen(loop2);
        high[loop2] = (TA_Real) data->getHigh(loop2);
        low[loop2] = (TA_Real) data->getLow(loop2);
        close[loop2] = (TA_Real) data->getClose(loop2);
        volume[loop2] = (TA_Integer) data->getVolume(loop2);
        oi[loop2] = (TA_Integer) data->getOI(loop2);
      }

      retCode = TA_SetInputParamPricePtr(parmHolder, loop, 0, &open[0], &high[0], &low[0], &close[0], &volume[0], &oi[0]);
      if (retCode != TA_SUCCESS)
      {
        qDebug("TALIB::calculateCustom:cannot set input prices");
        return 0;
      }
    }

    if (inputParms->type == TA_Input_Real)
    {
      if (! d.count())
      {
        qDebug("TALIB::calculateCustom: no input");
        return 0;
      }

      if (sparmIndex >= (int) l.count())
      {
        qDebug("TALIB::calculateCustom: input invalid number of parms");
        return 0;
      }

      PlotLine *line =  d.at(0);
      int loop2;
      for (loop2 = 0; loop2 < line->getSize(); loop2++)
        treal[loop2] = (TA_Real) line->getData(loop2);

      retCode = TA_SetInputParamRealPtr(parmHolder, loop, &treal[0]);
      if (retCode != TA_SUCCESS)
      {
        qDebug("TALIB::calculateCustom:cannot set real price");
        return 0;
      }

      sparmIndex++;
    }
  }

  if (sparmIndex < (int) l.count())
  {
    // setup the optinput parms
    const TA_OptInputParameterInfo *optInfo;
    for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
    {
      TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
      switch (optInfo->type)
      {
        case TA_OptInput_RealRange:
          if (sparmIndex >= (int) l.count())
          {
            qDebug("TALIB::calculateCustom: optinput real invalid number of parms");
            return 0;
          }

          retCode = TA_SetOptInputParamReal(parmHolder, loop, (TA_Real) l[sparmIndex].toDouble());
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set inputopt real");
            return 0;
          }
          sparmIndex++;
          break;
        case TA_OptInput_IntegerRange:
          if (sparmIndex >= (int) l.count())
          {
            qDebug("TALIB::calculateCustom: optinput integer invalid number of parms");
            return 0;
          }

          retCode = TA_SetOptInputParamInteger(parmHolder, loop, (TA_Integer) l[sparmIndex].toInt());
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set inputopt integer");
            return 0;
          }
          sparmIndex++;
          break;
        default:
          break;
      }
    }
  }

  // setup the output arrays
  TA_Real out1[data->count()];
  TA_Real out2[data->count()];
  TA_Real out3[data->count()];
  TA_Integer out4[data->count()];
  const TA_OutputParameterInfo *outInfo;
  for (loop = 0; loop < (int) theInfo->nbOutput; loop++)
  {
    retCode = TA_GetOutputParameterInfo(handle, loop, &outInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("TALIB::calculateCustom:cannot get output info");
      return 0;
    }

    switch (loop)
    {
      case 0:
        if (outInfo->type == TA_Output_Integer)
        {
          retCode = TA_SetOutputParamIntegerPtr(parmHolder, loop, &out4[0]);
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set output4");
            return 0;
          }
        }
        else
        {
          retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out1[0]);
          if (retCode != TA_SUCCESS)
          {
            qDebug("TALIB::calculateCustom:cannot set output1");
            return 0;
          }
        }
        break;      
      case 1:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out2[0]);
        if (retCode != TA_SUCCESS)
        {
          qDebug("TALIB::calculateCustom:cannot set output2");
          return 0;
        }
        break;      
      case 2:
        retCode = TA_SetOutputParamRealPtr(parmHolder, loop, &out3[0]);
        if (retCode != TA_SUCCESS)
        {
          qDebug("TALIB::calculateCustom:cannot set output3");
          return 0;
        }
        break;
      default:
        break;
    }
  }

  // call the function
  TA_Integer start = 0;
  TA_Integer end = data->count() - 1;
  if (d.count())
    end = d.at(0)->getSize() - 1;
  TA_Integer outstart;
  TA_Integer count;
  retCode = TA_CallFunc(parmHolder, start, end, &outstart, &count);
  if (retCode != TA_SUCCESS)
    qDebug("TALIB::calculateCustom:call function failed");
  else
  {
    // create the plotlines
    clearOutput();

    int loop2;
    PlotLine *line = new PlotLine;
    output->addLine(line);

    retCode = TA_GetOutputParameterInfo(handle, 0, &outInfo);
    if (retCode != TA_SUCCESS)
    {
      qDebug("TALIB::calculateCustom:cannot get output info");
      return 0;
    }
    
    if (outInfo->type == TA_Output_Integer)
    {
      for (loop2 = 0; loop2 < count; loop2++)
        line->append((double) out4[loop2]);
    }
    else
    {
      for (loop2 = 0; loop2 < count; loop2++)
        line->append((double) out1[loop2]);
    }
  }

  retCode = TA_ParamHolderFree(parmHolder);
  if (retCode != TA_SUCCESS)
    qDebug("TALIB::calculateCustom:can't delete parm holder");

  if (output->getLines())
    return output->getLine(0);
  else
    return 0;
}

PlotLine * TALIB::getMA (PlotLine *in, int type, int period)
{
  PlotLine *ma = new PlotLine;

  TA_RetCode rc = TA_Initialize(NULL);
  if (rc != TA_SUCCESS)
  {
    qDebug("TALIB::getMA:error on TA_Initialize");
    return ma;
  }

  TA_Real input[in->getSize()];
  TA_Real out[in->getSize()];
  TA_Integer outBeg;
  TA_Integer count;

  int loop;
  for (loop = 0; loop < in->getSize(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  switch (type)
  {
    case 0:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_SMA, &outBeg, &count, &out[0]);
      break;
    case 1:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_EMA, &outBeg, &count, &out[0]);
      break;
    case 2:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_WMA, &outBeg, &count, &out[0]);
      break;
    case 4:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_DEMA, &outBeg, &count, &out[0]);
      break;
    case 5:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_KAMA, &outBeg, &count, &out[0]);
      break;
    case 6:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_TEMA, &outBeg, &count, &out[0]);
      break;
    case 7:
      rc = TA_MA(0, in->getSize()- 1, &input[0], period, TA_MAType_TRIMA, &outBeg, &count, &out[0]);
      break;
    default:
      break;    
  }

  TA_Shutdown();  

  if (rc != TA_SUCCESS)
  {
    qDebug("TALIB::getMA:error on TALIB function call");
    return ma;
  }

  for (loop = 0; loop < count; loop++)
    ma->append((double) out[loop]);

  return ma;  
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  TALIB *o = new TALIB;
  return ((IndicatorPlugin *) o);
}



