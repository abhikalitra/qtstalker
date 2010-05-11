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

#include "STOCHRSI.h"
#include "MAUtils.h"
#include "RSIUtils.h"
#include "MAX.h"
#include "MIN.h"

#include <QtDebug>


STOCHRSI::STOCHRSI ()
{
  indicator = "STOCHRSI";

  settings.setData(Color, "red");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(Plot, "Line");
  settings.setData(Label, "STOCHRSI");
  settings.setData(Ref1, 0.2);
  settings.setData(Ref2, 0.8);
  settings.setData(Input, "Close");
  settings.setData(Period, 14);
}

int STOCHRSI::getIndicator (Indicator &ind, BarData *data)
{
  PlotLine *ref1 = new PlotLine;
  QString s = "Horizontal";
  ref1->setPlugin(s);
  settings.getData(Ref1Color, s);
  ref1->setColor(s);
  ref1->append(settings.getDouble(Ref1));
  ind.addLine(ref1);

  PlotLine *ref2 = new PlotLine;
  s = "Horizontal";
  ref2->setPlugin(s);
  settings.getData(Ref2Color, s);
  ref2->setColor(s);
  ref2->append(settings.getDouble(Ref2));
  ind.addLine(ref2);

  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);
  PlotLine *line = getSTOCHRSI(in, period);
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

int STOCHRSI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,STOCHRSI,<NAME>,<INPUT>,<PERIOD>
  //    0        1       2       3       4       5

  if (set.count() != 6)
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

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period" << set[5];
    return 1;
  }

  PlotLine *line = getSTOCHRSI(in, period);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * STOCHRSI::getSTOCHRSI (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  RSIUtils rsiu;
  PlotLine *rsi = rsiu.getRSI(in, period);
  if (! rsi)
    return 0;

  MAX max;
  PlotLine *hh = max.getMAX(rsi, period);
  if (! hh)
  {
    delete rsi;
    return 0;
  }

  MIN min;
  PlotLine *ll = min.getMIN(rsi, period);
  if (! ll)
  {
    delete rsi;
    delete hh;
    return 0;
  }

  int loop = rsi->count() - 1;
  int minLoop = ll->count() - 1;
  int maxLoop = hh->count() - 1;
  PlotLine *line = new PlotLine;
  while (loop > -1 && minLoop > -1 && maxLoop > -1)
  {
    double t = rsi->getData(loop) - ll->getData(minLoop);
    double t2 = hh->getData(maxLoop) - ll->getData(minLoop);
    line->prepend(t / t2);

    loop--;
    minLoop--;
    maxLoop--;
  }

  delete rsi;
  delete hh;
  delete ll;

  return line;
}

int STOCHRSI::dialog (int)
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  dialog->addDoubleItem(Ref1, page, QObject::tr("Ref. 1"), settings.getDouble(Ref1), 0.0, 1.0);

  dialog->addDoubleItem(Ref2, page, QObject::tr("Ref. 2"), settings.getDouble(Ref2), 0.0, 1.0);

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
  STOCHRSI *o = new STOCHRSI;
  return ((IndicatorPlugin *) o);
}

