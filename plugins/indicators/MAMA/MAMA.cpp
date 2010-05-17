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

#include "MAMA.h"
#include "ta_libc.h"
#include "BARSUtils.h"
#include "PlotFactory.h"

#include <QtDebug>

MAMA::MAMA ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");

  indicator = "MAMA";

  settings.setData(OSC, 1);
  settings.setData(OSCColor, "red");
  settings.setData(MAMAColor, "red");
  settings.setData(FAMAColor, "yellow");
  settings.setData(MAMAPlot, "Line");
  settings.setData(FAMAPlot, "Line");
  settings.setData(MAMALabel, "MAMA");
  settings.setData(FAMALabel, "FAMA");
  settings.setData(OSCLabel, "MAMAOSC");
  settings.setData(FastLimit, 0.5);
  settings.setData(SlowLimit, 0.05);
  settings.setData(Input, "Close");
}

int MAMA::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  double fast = settings.getDouble(FastLimit);
  double slow = settings.getDouble(SlowLimit);

  settings.getData(MAMAColor, s);
  QColor mcolor(s);

  settings.getData(FAMAColor, s);
  QColor fcolor(s);

  PlotFactory fac;
  settings.getData(MAMAPlot, s);
  int mlineType = fac.typeFromString(s);

  settings.getData(FAMAPlot, s);
  int flineType = fac.typeFromString(s);

  QList<PlotLine *> l;
  if (getMAMA(in, fast, slow, l, mlineType, flineType, mcolor, fcolor))
  {
    delete in;
    return 1;
  }

  PlotLine *mama = l.at(0);
  PlotLine *fama = l.at(1);

  int osc = settings.getInt(OSC);
  if (osc)
  {
    s = "Histogram Bar";
    PlotLine *line = fac.plot(s);
    if (! line)
    {
      delete in;
      delete mama;
      delete fama;
      return 1;
    }

    settings.getData(OSCColor, s);
    QColor color(s);

    settings.getData(OSCLabel, s);
    line->setLabel(s);

    QList<int> keys;
    mama->keys(keys);

    int loop = 0;
    for (; loop < keys.count(); loop++)
    {
      PlotLineBar *mbar = mama->data(keys.at(loop));
      if (! mbar)
        continue;

      PlotLineBar *fbar = fama->data(keys.at(loop));
      if (! fbar)
        continue;

      line->setData(keys.at(loop), new PlotLineBar(color, mbar->data() - fbar->data()));
    }

    ind.addLine(line);
  }
  else
  {
    QColor up("green");
    QColor down("red");
    QColor neutral("blue");
    BARSUtils b;
    PlotLine *bars = b.getBARS(data, up, down, neutral);
    if (bars)
      ind.addLine(bars);

    settings.getData(MAMALabel, s);
    mama->setLabel(s);
    ind.addLine(mama);

    // fama line
    settings.getData(FAMALabel, s);
    fama->setLabel(s);
    ind.addLine(fama);
  }

  delete in;

  return 0;
}

int MAMA::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MAMA,<INPUT>,<NAME_MAMA>,<NAME_FAMA>,<FAST_LIMIT>,<SLOW_LIMIT>,<MAMA PLOT TYPE>,<FAMA PLOT TYPE>,<MAMA COLOR>,<FAMA COLOR>
  //      0       1     2     3         4           5           6            7              8                9              10          11

  if (set.count() != 12)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in);
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: mama duplicate name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: fama duplicate name" << set[5];
    return 1;
  }

  bool ok;
  double fast = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid fast limit" << set[6];
    return 1;
  }

  double slow = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid slow limit" << set[7];
    return 1;
  }

  PlotFactory fac;
  int mlineType = fac.typeFromString(set[8]);
  if (mlineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid mama plot type" << set[8];
    return 1;
  }

  int flineType = fac.typeFromString(set[9]);
  if (flineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid fama plot type" << set[9];
    return 1;
  }

  QColor mcolor(set[10]);
  if (! mcolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid mama color" << set[10];
    return 1;
  }

  QColor fcolor(set[11]);
  if (! fcolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid fama color" << set[11];
    return 1;
  }

  QList<PlotLine *> l;
  if (getMAMA(in, fast, slow, l, mlineType, flineType, mcolor, fcolor))
    return 1;

  l.at(0)->setLabel(set[4]);
  l.at(1)->setLabel(set[5]);

  tlines.insert(set[4], l.at(0));
  tlines.insert(set[5], l.at(1));

  return 0;
}

int MAMA::getMAMA (PlotLine *in, double fast, double slow, QList<PlotLine *> &l, int mlineType, int flineType,
                   QColor &mcolor, QColor &fcolor)
{
  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MAMA(0, size - 1, &input[0], fast, slow, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getMAMA: TA-Lib error" << rc;
    return 1;
  }

  PlotFactory fac;
  PlotLine *mama = fac.plot(mlineType);
  if (! mama)
    return 1;

  PlotLine *fama = fac.plot(flineType);
  if (! fama)
  {
    delete mama;
    return 1;
  }

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    mama->setData(keys.at(keyLoop), new PlotLineBar(mcolor, out[outLoop]));
    fama->setData(keys.at(keyLoop), new PlotLineBar(fcolor, out2[outLoop]));
    keyLoop--;
    outLoop--;
  }

  l.append(mama);
  l.append(fama);

  return 0;
}

int MAMA::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addDoubleItem(FastLimit, page, QObject::tr("Fast Limit"), settings.getDouble(FastLimit), 0.01, 0.99);

  dialog->addDoubleItem(SlowLimit, page, QObject::tr("Slow Limit"), settings.getDouble(SlowLimit), 0.01, 0.99);

  dialog->addCheckItem(OSC, page, QObject::tr("Oscillator"), settings.getInt(OSC));

  page++;
  k = QObject::tr("MAMA");
  dialog->addPage(page, k);

  settings.getData(MAMAColor, d);
  dialog->addColorItem(MAMAColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(MAMAPlot, d);
  dialog->addComboItem(MAMAPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MAMALabel, d);
  dialog->addTextItem(MAMALabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("FAMA");
  dialog->addPage(page, k);

  settings.getData(FAMAColor, d);
  dialog->addColorItem(FAMAColor, page, QObject::tr("Color"), d);

  settings.getData(FAMAPlot, d);
  dialog->addComboItem(FAMAPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(FAMALabel, d);
  dialog->addTextItem(FAMALabel, page, QObject::tr("Label"), d);

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
  MAMA *o = new MAMA;
  return ((IndicatorPlugin *) o);
}


