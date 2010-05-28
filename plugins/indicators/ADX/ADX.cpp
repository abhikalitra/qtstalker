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

#include "ADX.h"
#include "PlotFactory.h"
#include "ta_libc.h"

#include <QtDebug>

ADX::ADX ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("ADX::error on TA_Initialize");

  _indicator = "ADX";

  _settings.setData(ADXColor, "blue");
  _settings.setData(ADXRColor, "yellow");
  _settings.setData(PDIColor, "green");
  _settings.setData(MDIColor, "red");
  _settings.setData(ADXPlot, "Line");
  _settings.setData(ADXRPlot, "Line");
  _settings.setData(PDIPlot, "Line");
  _settings.setData(MDIPlot, "Line");
  _settings.setData(ADXLabel, "ADX");
  _settings.setData(ADXRLabel, "ADXR");
  _settings.setData(PDILabel, "+DI");
  _settings.setData(MDILabel, "-DI");
  _settings.setData(ADXCheck, 1);
  _settings.setData(ADXRCheck, 1);
  _settings.setData(PDICheck, 1);
  _settings.setData(MDICheck, 1);
  _settings.setData(Period, 14);

  _methodList << "ADX" << "ADXR" << "+DI" << "-DI" << "DX";
}

int ADX::getIndicator (Indicator &ind, BarData *data)
{
  int period = _settings.getInt(Period);

  if (_settings.getInt(MDICheck))
  {
    QString s;
    _settings.getData(MDIColor, s);
    QColor color(s);

    PlotFactory fac;
    _settings.getData(MDIPlot, s);
    int lineType = fac.typeFromString(s);
    
    PlotLine *line = getLine(data, period, MDI, lineType, color);
    if (! line)
      return 1;

    _settings.getData(MDILabel, s);
    line->setLabel(s);

    s = "0";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }

  if (_settings.getInt(PDICheck))
  {
    QString s;
    _settings.getData(PDIColor, s);
    QColor color(s);

    PlotFactory fac;
    _settings.getData(PDIPlot, s);
    int lineType = fac.typeFromString(s);

    PlotLine *line = getLine(data, period, PDI, lineType, color);
    if (! line)
      return 1;

    _settings.getData(PDILabel, s);
    line->setLabel(s);
    
    s = "1";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }

  if (_settings.getInt(ADXCheck))
  {
    QString s;
    _settings.getData(ADXColor, s);
    QColor color(s);

    PlotFactory fac;
    _settings.getData(ADXPlot, s);
    int lineType = fac.typeFromString(s);

    PlotLine *line = getLine(data, period, _ADX, lineType, color);
    if (! line)
      return 1;

    _settings.getData(ADXLabel, s);
    line->setLabel(s);
    
    s = "2";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }

  if (_settings.getInt(ADXRCheck))
  {
    QString s;
    _settings.getData(ADXRColor, s);
    QColor color(s);

    PlotFactory fac;
    _settings.getData(ADXRPlot, s);
    int lineType = fac.typeFromString(s);

    PlotLine *line = getLine(data, period, ADXR, lineType, color);
    if (! line)
      return 1;

    _settings.getData(ADXRLabel, s);
    line->setLabel(s);
    
    s = "3";
    ind.setLine(s, line);
    ind.addPlotOrder(s);
  }

  return 0;
}

int ADX::getCUS (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,ADX,<METHOD>,<NAME>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1     2     3        4      5          6         7

  if (set.count() != 8)
  {
    qDebug() << _indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid period" << set[5];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getLine(data, period, method, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

PlotLine * ADX::getLine (BarData *data, int period, int method, int lineType, QColor &color)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
    close[loop] = (TA_Real) bar->getClose();
  }

  TA_RetCode rc = TA_SUCCESS;
  
  switch ((Method) method)
  {
    case MDI:
      rc = TA_MINUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case PDI:
      rc = TA_PLUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _ADX:
      rc = TA_ADX(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ADXR:
      rc = TA_ADXR(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case DX:
      rc = TA_DX(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getLine: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setData(dataLoop, new PlotLineBar(color, out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

int ADX::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("General");
  dialog->addPage(page, k);

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 2, 100000);

  page++;
  k = QObject::tr("-DI");
  dialog->addPage(page, k);

  _settings.getData(MDIColor, d);
  dialog->addColorItem(MDIColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(MDIPlot, d);
  dialog->addComboItem(MDIPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MDILabel, d);
  dialog->addTextItem(MDILabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(MDICheck, page, QObject::tr("Show"), _settings.getInt(MDICheck));

  page++;
  k = QObject::tr("+DI");
  dialog->addPage(page, k);

  _settings.getData(PDIColor, d);
  dialog->addColorItem(PDIColor, page, QObject::tr("Color"), d);

  _settings.getData(PDIPlot, d);
  dialog->addComboItem(PDIPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(PDILabel, d);
  dialog->addTextItem(PDILabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(PDICheck, page, QObject::tr("Show"), _settings.getInt(MDICheck));

  page++;
  k = QObject::tr("ADX");
  dialog->addPage(page, k);

  _settings.getData(ADXColor, d);
  dialog->addColorItem(ADXColor, page, QObject::tr("Color"), d);

  _settings.getData(ADXPlot, d);
  dialog->addComboItem(ADXPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(ADXLabel, d);
  dialog->addTextItem(ADXLabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(ADXCheck, page, QObject::tr("Show"), _settings.getInt(MDICheck));

  page++;
  k = QObject::tr("ADXR");
  dialog->addPage(page, k);

  _settings.getData(ADXRColor, d);
  dialog->addColorItem(ADXRColor, page, QObject::tr("Color"), d);

  _settings.getData(ADXRPlot, d);
  dialog->addComboItem(ADXRPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(ADXRLabel, d);
  dialog->addTextItem(ADXRLabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(ADXRCheck, page, QObject::tr("Show"), _settings.getInt(MDICheck));

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  getDialogSettings(dialog);

  delete dialog;
  return rc;
}


//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ADX *o = new ADX;
  return ((IndicatorPlugin *) o);
}


