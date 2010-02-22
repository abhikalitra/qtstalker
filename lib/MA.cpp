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

#include "MA.h"
#include "BARS.h"
#include "COMPARE.h"

#include <QtDebug>


MA::MA ()
{
  indicator = "MA";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Input, "Close");
  settings.setData(Period, 14);
  settings.setData(Method, "SMA");
  settings.setData(OptPeriodLow, 10);
  settings.setData(OptPeriodHigh, 20);
  settings.setData(Operator, "<");

  methodList << "EMA";
  methodList << "DEMA";
  methodList << "KAMA";
  methodList << "SMA";
  methodList << "TEMA";
  methodList << "TRIMA";
  methodList << "Wilder";
  methodList << "WMA";
}

int MA::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);

  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  PlotLine *line = getMA(in, period, method);
  if (! line)
  {
    delete in;
    return 1;
  }

  BARS bars;
  bars.turnOffMAs();
  bars.getIndicator(ind, data);

  settings.getData(Color, s);
  line->setColor(s);

  settings.getData(Plot, s);
  line->setType(s);

  settings.getData(Label, s);
  line->setLabel(s);

  ind.addLine(line);

  delete in;

  return 0;
}

int MA::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MA,METHOD,<NAME>,<INPUT>,<PERIOD>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[2]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[2];
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

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[5];
    return 1;
  }

  PlotLine *line = getMA(in, period, method);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * MA::getMA (PlotLine *in, int period, int method)
{
  int size = in->getSize();
  TA_Real input[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_EMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_DEMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_KAMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_SMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 4:
      rc = TA_TEMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 5:
      rc = TA_TRIMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 6: // Wilder
    {
      PlotLine *line = new PlotLine;
      double t = 0;
      int loop;
      for (loop = 0; loop < period; loop++)
        t = t + input[loop];
      double yesterday = t / period;
      line->append(yesterday);
      for (; loop < size; loop++)
      {
        double t  = (yesterday * (period - 1) + input[loop]) / period;
        yesterday = t;
        line->append(t);
      }
      return line;
      break;
    }
    case 7:
      rc = TA_WMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

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

int MA::dialog (int f)
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

  if (f)
    testDialog(dialog, page);

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

// test functions

void MA::testDialog (PrefDialog *dialog, int page)
{
  page++;
  QString d;
  QString k = QObject::tr("Test");
  dialog->addPage(page, k);

  settings.getData(Operator, d);
  dialog->addComboItem(Operator, page, QObject::tr("Input <Operator> MA"), opList, d);

  dialog->addIntItem(OptPeriodLow, page, QObject::tr("Optimazation Period Low"),
		     settings.getInt(OptPeriodLow), -100000, 100000);

  dialog->addIntItem(OptPeriodHigh, page, QObject::tr("Optimazation Period High"),
		     settings.getInt(OptPeriodHigh), -100000, 100000);
}

int MA::test (BarData *data, TestSignal &sigs)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::test: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);

  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  PlotLine *ma = getMA(in, period, method);
  if (! ma)
  {
    qDebug() << indicator << "::test: getMA error" << s;
    delete in;
    return 1;
  }

  COMPARE c;
  settings.getData(Operator, s);
  int op = opList.indexOf(s);
  PlotLine *comp = c.compareAA(in, ma, op);
  if (! comp)
  {
    qDebug() << indicator << "::test: compare error" << s;
    delete in;
    delete ma;
    return 1;
  }

//  qDebug() << in->getSize() << ma->getSize() << comp->getSize();

  delete in;
  delete ma;

  if (sigs.createSignals(data, comp))
  {
    qDebug() << indicator << "::test: createSignals error" << s;
    return 1;
  }

  delete comp;
  return 0;
}

