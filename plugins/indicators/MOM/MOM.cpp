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

#include "MOM.h"
#include "MAUtils.h"

#include <QtDebug>


MOM::MOM ()
{
  indicator = "MOM";

  settings.setData(Color, "red");
  settings.setData(Plot, "Histogram Bar");
  settings.setData(Label, indicator);
  settings.setData(Period, 10);
  settings.setData(Smoothing, 1);
  settings.setData(SmoothingType, "SMA");
  settings.setData(Input, "Close");
}

int MOM::getIndicator (Indicator &ind, BarData *data)
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

  PlotLine *line = getMOM(in, period, smoothing, type);
  if (! line)
  {
    delete in;
    return 1;
  }

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

int MOM::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MOM,<NAME>,<INPUT>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>

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

  PlotLine *line = getMOM(in, period, smoothing, type);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * MOM::getMOM (PlotLine *in, int period, int smoothing, int type)
{
  if (in->count() < period)
    return 0;
  
  PlotLine *line = new PlotLine;
  int size = in->count();
  int loop = period;
  for (; loop < size; loop++)
    line->append(in->getData(loop) - in->getData(loop - period));

  if (smoothing > 1)
  {
    MAUtils mau;
    PlotLine *ma = mau.getMA(line, smoothing, type);
    delete line;
    line = ma;
  }

  return line;
}

int MOM::dialog (int)
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
  MOM *o = new MOM;
  return ((IndicatorPlugin *) o);
}


