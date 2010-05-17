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

#include "MAVP.h"
#include "ta_libc.h"
#include "MAFactory.h"
#include "BARSUtils.h"
#include "PlotFactory.h"

#include <QtDebug>


MAVP::MAVP ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");

  indicator = "MAVP";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Input, "Close");
  settings.setData(Input2, "Close");
  settings.setData(Min, 2);
  settings.setData(Max, 30);
  settings.setData(MAType, "SMA");
}

int MAVP::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  settings.getData(Input2, s);
  PlotLine *in2 = data->getInput(data->getInputType(s));
  if (! in2)
  {
    delete in;
    qDebug() << indicator << "::getIndicator: input 2 not found" << s;
    return 1;
  }

  int min = settings.getInt(Min);
  int max = settings.getInt(Max);

  MAFactory mau;
  settings.getData(MAType, s);
  int ma = mau.typeFromString(s);

  settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getMAVP(in, in2, min, max, ma, lineType, color);
  if (! line)
  {
    delete in;
    delete in2;
    return 1;
  }

  settings.getData(Label, s);
  line->setLabel(s);

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (bars)
    ind.addLine(bars);

  ind.addLine(line);

  delete in;
  delete in2;

  return 0;
}

int MAVP::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MAVP,<NAME>,<INPUT_1>,<INPUT_2>,<MIN_PERIOD>,<MAX_PERIOD>,<MA_TYPE>,<PLOT TYPE>,<COLOR>
  //     0       1     2     3        4        5           6            7          8           9         10

  if (set.count() != 11)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  PlotLine *in2 = tlines.value(set[5]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(set[5]));
    if (! in2)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in2);
  }

  bool ok;
  int min = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid min period settings" << set[6];
    return 1;
  }

  int max = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid max period settings" << set[7];
    return 1;
  }

  MAFactory mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid ma settings" << set[8];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[9]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[9];
    return 1;
  }

  QColor color(set[10]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[10];
    return 1;
  }

  PlotLine *line = getMAVP(in, in2, min, max, ma, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * MAVP::getMAVP (PlotLine *in, PlotLine *in2, int min, int max, int ma, int lineType, QColor &color)
{
  int flag = 0;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    flag = 1;
  }

  QList<int> keys;
  in->keys(keys);
  QList<int> keys2;
  in2->keys(keys2);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = keys.count() - 1;
  int loop2 = keys2.count() - 1;
  while (loop > -1 && loop2 > -1)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    PlotLineBar *bar2 = in2->data(keys2.at(loop2));
    input[loop] = (TA_Real) bar->data();
    input2[loop2] = (TA_Real) bar2->data();

    loop--;
    loop2--;
  }

  TA_RetCode rc = TA_MAVP(0, size - 1, &input[0], &input2[0], min, max, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getMAVP: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  if (! flag)
  {
    int keyLoop = keys.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setData(keys.at(keyLoop), new PlotLineBar(color, out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }
  else
  {
    int keyLoop = keys2.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setData(keys2.at(keyLoop), new PlotLineBar(color, out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }

  return line;
}

int MAVP::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  settings.getData(Input2, d);
  dialog->addComboItem(Input2, page, QObject::tr("Input 2"), inputList, d);

  dialog->addIntItem(Min, page, QObject::tr("Min"), settings.getInt(Min), 2, 100000);

  dialog->addIntItem(Max, page, QObject::tr("Max"), settings.getInt(Max), 2, 100000);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);

  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

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
  MAVP *o = new MAVP;
  return ((IndicatorPlugin *) o);
}


