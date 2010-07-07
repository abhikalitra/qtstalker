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

#include "CMO.h"
#include "PlotFactory.h"
#include "ta_libc.h"

#include <QtDebug>

CMO::CMO ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CMO::error on TA_Initialize");

  _indicator = "CMO";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Histogram Bar");
  _settings.setData(Label, _indicator);
  _settings.setData(Input, "Close");
  _settings.setData(Period, 14);
}

int CMO::getIndicator (Indicator &ind, BarData &data)
{
  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = _settings.getInt(Period);

  _settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  _settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getCMO(in, period, lineType, color);
  if (! line)
  {
    delete in;
    return 1;
  }

  _settings.getData(Label, s);
  line->setLabel(s);
  
  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  delete in;

  return 0;
}

int CMO::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CMO,<NAME>,<INPUT>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1     2    3       4       5         6          7

  if (set.count() != 8)
  {
    qDebug() << _indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << _indicator << "::getCUS: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid period parm" << set[5];
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

  PlotLine *line = getCMO(in, period, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * CMO::getCMO (PlotLine *in, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  TA_Real input[in->count()];
  TA_Real out[in->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar bar;
    in->data(keys.at(loop), bar);
    input[loop] = (TA_Real) bar.data();
  }

  TA_RetCode rc = TA_CMO(0,
                         keys.count() - 1,
                         &input[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getCMO: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    PlotLineBar bar(color, out[outLoop]);
    line->setData(keys.at(keyLoop), bar);
    keyLoop--;
    outLoop--;
  }

  return line;
}

int CMO::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  _settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 2, 100000);

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
  CMO *o = new CMO;
  return ((IndicatorPlugin *) o);
}


