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

#include "ADX.h"

#include <QtDebug>


ADX::ADX ()
{
  indicator = "ADX";

  settings.setData(ADXColor, "blue");
  settings.setData(ADXRColor, "yellow");
  settings.setData(PDIColor, "green");
  settings.setData(MDIColor, "red");
  settings.setData(ADXPlot, "Line");
  settings.setData(ADXRPlot, "Line");
  settings.setData(PDIPlot, "Line");
  settings.setData(MDIPlot, "Line");
  settings.setData(ADXLabel, "ADX");
  settings.setData(ADXRLabel, "ADXR");
  settings.setData(PDILabel, "+DI");
  settings.setData(MDILabel, "-DI");
  settings.setData(ADXCheck, 1);
  settings.setData(ADXRCheck, 1);
  settings.setData(PDICheck, 1);
  settings.setData(MDICheck, 1);
  settings.setData(Period, 14);

  methodList << "ADX";
  methodList << "ADXR";
  methodList << "+DI";
  methodList << "-DI";
  methodList << "DX";
  methodList << "-DM";
  methodList << "+DM";
}

int ADX::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(Period);

  if (settings.getInt(MDICheck))
  {
    PlotLine *line = getLine(data, period, 0);
    if (! line)
      return 1;

    QString s;
    settings.getData(MDIColor, s);
    line->setColor(s);

    settings.getData(MDIPlot, s);
    line->setType(s);

    settings.getData(MDILabel, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  if (settings.getInt(PDICheck))
  {
    PlotLine *line = getLine(data, period, 1);
    if (! line)
      return 1;

    QString s;
    settings.getData(PDIColor, s);
    line->setColor(s);

    settings.getData(PDIPlot, s);
    line->setType(s);

    settings.getData(PDILabel, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  if (settings.getInt(ADXCheck))
  {
    PlotLine *line = getLine(data, period, 2);
    if (! line)
      return 1;

    QString s;
    settings.getData(ADXColor, s);
    line->setColor(s);

    settings.getData(ADXPlot, s);
    line->setType(s);

    settings.getData(ADXLabel, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  if (settings.getInt(ADXRCheck))
  {
    PlotLine *line = getLine(data, period, 3);
    if (! line)
      return 1;

    QString s;
    settings.getData(ADXRColor, s);
    line->setColor(s);

    settings.getData(ADXRPlot, s);
    line->setType(s);

    settings.getData(ADXRLabel, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  return 0;
}

int ADX::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,ADX,<METHOD>,<NAME>,<PERIOD>

  if (set.count() != 5)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
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

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[4];
    return 1;
  }

  PlotLine *line = getLine(data, period, method);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * ADX::getLine (BarData *data, int period, int method)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_MINUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_PLUS_DI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_ADX(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 3:
      rc = TA_ADXR(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 4:
      rc = TA_DX(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 5:
      rc = TA_MINUS_DM(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
      break;
    case 6:
      rc = TA_PLUS_DM(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
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

int ADX::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("General");
  dialog->addPage(page, k);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  page++;
  k = QObject::tr("-DI");
  dialog->addPage(page, k);

  settings.getData(MDIColor, d);
  dialog->addColorItem(MDIColor, page, QObject::tr("Color"), d);

  settings.getData(MDIPlot, d);
  dialog->addComboItem(MDIPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MDILabel, d);
  dialog->addTextItem(MDILabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(MDICheck, page, QObject::tr("Show"), settings.getInt(MDICheck));

  page++;
  k = QObject::tr("+DI");
  dialog->addPage(page, k);

  settings.getData(PDIColor, d);
  dialog->addColorItem(PDIColor, page, QObject::tr("Color"), d);

  settings.getData(PDIPlot, d);
  dialog->addComboItem(PDIPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(PDILabel, d);
  dialog->addTextItem(PDILabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(PDICheck, page, QObject::tr("Show"), settings.getInt(MDICheck));

  page++;
  k = QObject::tr("ADX");
  dialog->addPage(page, k);

  settings.getData(ADXColor, d);
  dialog->addColorItem(ADXColor, page, QObject::tr("Color"), d);

  settings.getData(ADXPlot, d);
  dialog->addComboItem(ADXPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(ADXLabel, d);
  dialog->addTextItem(ADXLabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(ADXCheck, page, QObject::tr("Show"), settings.getInt(MDICheck));

  page++;
  k = QObject::tr("ADXR");
  dialog->addPage(page, k);

  settings.getData(ADXRColor, d);
  dialog->addColorItem(ADXRColor, page, QObject::tr("Color"), d);

  settings.getData(ADXRPlot, d);
  dialog->addComboItem(ADXRPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(ADXRLabel, d);
  dialog->addTextItem(ADXRLabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(ADXRCheck, page, QObject::tr("Show"), settings.getInt(MDICheck));

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


