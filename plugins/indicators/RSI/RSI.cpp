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

#include "RSI.h"
#include "MAUtils.h"
#include "RSIUtils.h"

#include <QtDebug>


RSI::RSI ()
{
  indicator = "RSI";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Period, 14);
  settings.setData(Smoothing, 1);
  settings.setData(SmoothingType, "SMA");
  settings.setData(Input, "Close");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(Ref1, 30);
  settings.setData(Ref2, 70);
}

int RSI::getIndicator (Indicator &ind, BarData *data)
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
  int smoothing = settings.getInt(Smoothing);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);

  settings.getData(SmoothingType, s);
  int type = maList.indexOf(s);

  PlotLine *line = getRSI(in, period, smoothing, type);
  if (! line)
  {
    delete in;
    return 1;
  }

  PlotLine *ref1 = new PlotLine;
  s = "Horizontal";
  ref1->setPlugin(s);
  ref1->append(settings.getInt(Ref1));
  settings.getData(Ref1Color, s);
  ref1->setColor(s);
  ind.addLine(ref1);

  PlotLine *ref2 = new PlotLine;
  s = "Horizontal";
  ref2->setPlugin(s);
  ref2->append(settings.getInt(Ref2));
  settings.getData(Ref2Color, s);
  ref2->setColor(s);
  ind.addLine(ref2);

  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;

  return 0;
}

int RSI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,RSI,<NAME>,<INPUT>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>

  if (set.count() != 8)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
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
    qDebug() << indicator << "::calculate: invalid period parm" << set[5];
    return 1;
  }

  int smoothing = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid smoothing" << set[6];
    return 1;
  }

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  int type = maList.indexOf(set[7]);
  if (type == -1)
  {
    qDebug() << indicator << "::calculate: invalid smoothing type" << set[7];
    return 1;
  }

  PlotLine *line = getRSI(in, period, smoothing, type);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * RSI::getRSI (PlotLine *in, int period, int smoothing, int type)
{
  if (in->count() < period)
    return 0;

  RSIUtils rsi;
  PlotLine *line = rsi.getRSI(in, period);
  if (! line)
    return 0;

  if (smoothing > 1)
  {
    MAUtils mau;
    PlotLine *ma = mau.getMA(line, smoothing, type);
    delete line;
    line = ma;
  }

  return line;
}

int RSI::dialog (int)
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 1, 100000);

  dialog->addIntItem(Smoothing, page, QObject::tr("Smoothing"), settings.getInt(Smoothing), 1, 100000);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);

  settings.getData(SmoothingType, d);
  dialog->addComboItem(Smoothing, page, QObject::tr("Smoothing Type"), maList, d);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  dialog->addIntItem(Ref1, page, QObject::tr("Ref. 1"), settings.getInt(Ref1), 0, 100);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref. 2"), settings.getInt(Ref2), 0, 100);

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
  RSI *o = new RSI;
  return ((IndicatorPlugin *) o);
}


