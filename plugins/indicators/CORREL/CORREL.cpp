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

#include "CORREL.h"
#include "DBPlugin.h"
#include "ta_libc.h"

#include <QtDebug>


CORREL::CORREL ()
{
  indicator = "CORREL";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Input, "Close");
  settings.setData(Input2, "SP500");
  settings.setData(Period, 30);
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(Ref3Color, "white");
}

int CORREL::getIndicator (Indicator &ind, BarData *data)
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
  BarData bd;
  bd.setSymbol(s);
  bd.setBarLength(data->getBarLength());
  bd.setBarsRequested(data->getBarsRequested());

  DBPlugin db;
  db.getBars(bd);

  PlotLine *in2 = bd.getInput(BarData::Close);
  if (! in2)
  {
    qDebug() << indicator << "::calculate: input 2 not found";
    delete in;
    return 1;
  }

  int period = settings.getInt(Period);

  PlotLine *line = getCORREL(in, in2, period);
  if (! line)
  {
    delete in;
    delete in2;
    return 1;
  }

  // 1 reference line
  PlotLine *hline = new PlotLine;
  s = "Horizontal";
  hline->setPlugin(s);
  hline->append(1);
  settings.getData(Ref3Color, s);
  hline->setColor(s);
  ind.addLine(hline);

  // 0 reference line
  hline = new PlotLine;
  s = "Horizontal";
  hline->setPlugin(s);
  hline->append(0);
  settings.getData(Ref2Color, s);
  hline->setColor(s);
  ind.addLine(hline);

  // -1 reference line
  hline = new PlotLine;
  s = "Horizontal";
  hline->setPlugin(s);
  hline->append(-1);
  settings.getData(Ref1Color, s);
  hline->setColor(s);
  ind.addLine(hline);

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

int CORREL::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,CORREL,<NAME>,<INPUT_1>,<INPUT_2>,<PERIOD>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[2];
    return 1;
  }

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in);
  }

  PlotLine *in2 = tlines.value(set[4]);
  if (! in2)
  {
    in2 = data->getInput(data->getInputType(set[4]));
    if (! in2)
    {
      qDebug() << indicator << "::calculate: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in2);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[5];
    return 1;
  }

  PlotLine *line = getCORREL(in, in2, period);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * CORREL::getCORREL (PlotLine *in, PlotLine *in2, int period)
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

  TA_RetCode rc = TA_CORREL(0, size - 1, &input[0], &input2[0], period, &outBeg, &outNb, &out[0]);
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

int CORREL::dialog (int)
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
  dialog->addTextItem(Input2, page, QObject::tr("Input 2"), d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 1, 100000);

  page++;
  k = QObject::tr("Ref.");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  settings.getData(Ref3Color, d);
  dialog->addColorItem(Ref3Color, page, QObject::tr("Ref. 3 Color"), d);

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
  CORREL *o = new CORREL;
  return ((IndicatorPlugin *) o);
}


