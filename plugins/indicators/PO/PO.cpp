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

#include "PO.h"
#include "MAFactory.h"
#include "PlotFactory.h"
#include "ta_libc.h"

#include <QtDebug>

PO::PO ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("PO::error on TA_Initialize");

  _indicator = "PO";

  _methodList << "APO" << "PPO";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Histogram Bar");
  _settings.setData(Label, _indicator);
  _settings.setData(Input, "Close");
  _settings.setData(FastPeriod, 12);
  _settings.setData(SlowPeriod, 26);
  _settings.setData(MAType, "SMA");
  _settings.setData(Method, "APO");
}

int PO::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int fast = _settings.getInt(FastPeriod);
  int slow = _settings.getInt(SlowPeriod);

  MAFactory mau;
  _settings.getData(MAType, s);
  int ma = mau.typeFromString(s);

  _settings.getData(Method, s);
  int method = _methodList.indexOf(s);

  _settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  _settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getPO(in, fast, slow, ma, method, lineType, color);
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

int PO::getCUS (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,PO,<METHOD>,<NAME>,<INPUT>,<FAST_PERIOD>,<SLOW_PERIOD>,<MA_TYPE>,<PLOT TYPE>,<COLOR>
  //     0       1     2    3       4      5         6             7           8           9         10

  if (set.count() != 11)
  {
    qDebug() << _indicator << "::getCUS: invalid parm count" << set.count();
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

  PlotLine *in = ind.line(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << _indicator << "::getCUS: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  bool ok;
  int fast = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid fast period" << set[6];
    return 1;
  }

  int slow = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid slow period" << set[7];
    return 1;
  }

  MAFactory mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid ma" << set[8];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[9]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid plot type" << set[9];
    return 1;
  }

  QColor color(set[10]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid color" << set[10];
    return 1;
  }

  PlotLine *line = getPO(in, fast, slow, ma, method, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

PlotLine * PO::getPO (PlotLine *in, int fast, int slow, int ma, int method, int lineType, QColor &color)
{
  if (in->count() < fast || in->count() < slow)
    return 0;

  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_SUCCESS;

  switch ((_Method) method)
  {
    case APO:
      rc = TA_APO(0, size - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
      break;
    case PPO:
      rc = TA_PPO(0, size - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getPO: TA-Lib error" << rc;
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
    line->setData(keys.at(keyLoop), new PlotLineBar(color, out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  return line;
}

int PO::dialog (int)
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

  dialog->addIntItem(FastPeriod, page, QObject::tr("Fast Period"), _settings.getInt(FastPeriod), 2, 100000);

  dialog->addIntItem(SlowPeriod, page, QObject::tr("Slow Period"), _settings.getInt(SlowPeriod), 2, 100000);

  MAFactory mau;
  QStringList maList = mau.list();

  _settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

  _settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), _methodList, d);

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
  PO *o = new PO;
  return ((IndicatorPlugin *) o);
}


