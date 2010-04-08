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

#include <QtDebug>


MAVP::MAVP ()
{
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
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  settings.getData(Input2, s);
  PlotLine *in2 = data->getInput(data->getInputType(s));
  if (! in2)
  {
    delete in;
    qDebug() << indicator << "::calculate: input 2 not found" << s;
    return 1;
  }

  int min = settings.getInt(Min);
  int max = settings.getInt(Max);

  QStringList maList;
  getMAList(maList);
  
  settings.getData(MAType, s);
  int ma = maList.indexOf(s);

  PlotLine *line = getMAVP(in, in2, min, max, ma);
  if (! line)
  {
    delete in;
    delete in2;
    return 1;
  }

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  PlotLine *bars = getLocalBARS(data, up, down, neutral);
  if (bars)
    ind.addLine(bars);

  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;
  delete in2;

  return 0;
}

int MAVP::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MAVP,<NAME>,<INPUT_1>,<INPUT_2>,<MIN_PERIOD>,<MAX_PERIOD>,<MA_TYPE>

  if (set.count() != 9)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[4];
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
      qDebug() << indicator << "::calculate: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in2);
  }

  bool ok;
  int min = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid min period settings" << set[6];
    return 1;
  }

  int max = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid max period settings" << set[7];
    return 1;
  }

  QStringList maList;
  getMAList(maList);
  int ma = maList.indexOf(set[8]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma settings" << set[8];
    return 1;
  }

  PlotLine *line = getMAVP(in, in2, min, max, ma);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * MAVP::getMAVP (PlotLine *in, PlotLine *in2, int min, int max, int ma)
{
  int size = in->count();
  if (in2->count() < size)
    size = in2->count();

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = in->count() - 1;
  int loop2 = in2->count() - 1;
  int count = size - 1;
  for (; count > -1; loop--, loop2--, count--)
  {
    input[loop] = (TA_Real) in->getData(loop);
    input2[loop2] = (TA_Real) in2->getData(loop2);
  }

  TA_RetCode rc = TA_MAVP(0, size - 1, &input[0], &input2[0], min, max, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

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

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  settings.getData(Input2, d);
  dialog->addComboItem(Input2, page, QObject::tr("Input 2"), inputList, d);

  dialog->addIntItem(Min, page, QObject::tr("Min"), settings.getInt(Min), 2, 100000);

  dialog->addIntItem(Max, page, QObject::tr("Max"), settings.getInt(Max), 2, 100000);

  QStringList maList;
  getMAList(maList);

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


