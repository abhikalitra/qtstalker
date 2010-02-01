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
  methodKey = QObject::tr("Method");
  adxColorKey = QObject::tr("ADX Color");
  adxrColorKey = QObject::tr("ADXR Color");
  pdiColorKey = QObject::tr("+DI Color");
  mdiColorKey = QObject::tr("-DI Color");
  adxPlotKey = QObject::tr("ADX Plot");
  adxrPlotKey = QObject::tr("ADXR Plot");
  pdiPlotKey = QObject::tr("+DI Plot");
  mdiPlotKey = QObject::tr("-DI Plot");
  adxLabelKey = QObject::tr("ADX Label");
  adxrLabelKey = QObject::tr("ADXR Label");
  pdiLabelKey = QObject::tr("+DI Label");
  mdiLabelKey = QObject::tr("-DI Label");
  adxCheckKey = QObject::tr("ADX Display");
  adxrCheckKey = QObject::tr("ADXR Display");
  pdiCheckKey = QObject::tr("+DI Display");
  mdiCheckKey = QObject::tr("-DI Display");

  methodList << "ADX";
  methodList << "ADXR";
  methodList << "+DI";
  methodList << "-DI";
  methodList << "DX";
  methodList << "-DM";
  methodList << "+DM";

  QString d;
  d = "red";
  settings.setData(mdiColorKey, d);

  d = "green";
  settings.setData(pdiColorKey, d);

  d = "blue";
  settings.setData(adxColorKey, d);

  d = "yellow";
  settings.setData(adxrColorKey, d);

  d = "Line";
  settings.setData(mdiPlotKey, d);
  settings.setData(pdiPlotKey, d);
  settings.setData(adxPlotKey, d);
  settings.setData(adxrPlotKey, d);

  d = "-DI";
  settings.setData(mdiLabelKey, d);

  d = "+DI";
  settings.setData(pdiLabelKey, d);

  d = "ADX";
  settings.setData(adxLabelKey, d);

  d = "ADXR";
  settings.setData(adxrLabelKey, d);

  settings.setData(periodKey, 14);

  d = "ADX";
  settings.setData(methodKey, d);

  settings.setData(mdiCheckKey, 1);
  settings.setData(pdiCheckKey, 1);
  settings.setData(adxCheckKey, 1);
  settings.setData(adxrCheckKey, 1);
}

int ADX::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(periodKey);

  if (settings.getInt(mdiCheckKey))
  {
    PlotLine *line = getLine(data, period, 0);
    if (! line)
      return 1;

    QString s;
    settings.getData(mdiColorKey, s);
    line->setColor(s);

    settings.getData(mdiPlotKey, s);
    line->setType(s);

    settings.getData(mdiLabelKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  if (settings.getInt(pdiCheckKey))
  {
    PlotLine *line = getLine(data, period, 1);
    if (! line)
      return 1;

    QString s;
    settings.getData(pdiColorKey, s);
    line->setColor(s);

    settings.getData(pdiPlotKey, s);
    line->setType(s);

    settings.getData(pdiLabelKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  if (settings.getInt(adxCheckKey))
  {
    PlotLine *line = getLine(data, period, 2);
    if (! line)
      return 1;

    QString s;
    settings.getData(adxColorKey, s);
    line->setColor(s);

    settings.getData(adxPlotKey, s);
    line->setType(s);

    settings.getData(adxLabelKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  if (settings.getInt(adxrCheckKey))
  {
    PlotLine *line = getLine(data, period, 3);
    if (! line)
      return 1;

    QString s;
    settings.getData(adxrColorKey, s);
    line->setColor(s);

    settings.getData(adxrPlotKey, s);
    line->setType(s);

    settings.getData(adxrLabelKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  return 0;
}

int ADX::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,ADX,<NAME>,<PERIOD>,<METHOD>

  if (set.count() != 5)
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

  bool ok;
  int period = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[3];
    return 1;
  }

  int method = methodList.indexOf(set[4]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[4];
    return 1;
  }

  PlotLine *line = getLine(data, period, method);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

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

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 2, 100000);

  page++;
  k = QObject::tr("-DI");
  dialog->addPage(page, k);

  settings.getData(mdiColorKey, d);
  dialog->addColorItem(page, mdiColorKey, d);

  settings.getData(mdiPlotKey, d);
  dialog->addComboItem(page, mdiPlotKey, plotList, d);

  settings.getData(mdiLabelKey, d);
  dialog->addTextItem(page, mdiLabelKey, d);

  dialog->addCheckItem(page, mdiCheckKey, settings.getInt(mdiCheckKey));

  page++;
  k = QObject::tr("+DI");
  dialog->addPage(page, k);

  settings.getData(pdiColorKey, d);
  dialog->addColorItem(page, pdiColorKey, d);

  settings.getData(pdiPlotKey, d);
  dialog->addComboItem(page, pdiPlotKey, plotList, d);

  settings.getData(pdiLabelKey, d);
  dialog->addTextItem(page, pdiLabelKey, d);

  dialog->addCheckItem(page, pdiCheckKey, settings.getInt(pdiCheckKey));

  page++;
  k = QObject::tr("ADX");
  dialog->addPage(page, k);

  settings.getData(adxColorKey, d);
  dialog->addColorItem(page, adxColorKey, d);

  settings.getData(adxPlotKey, d);
  dialog->addComboItem(page, adxPlotKey, plotList, d);

  settings.getData(adxLabelKey, d);
  dialog->addTextItem(page, adxLabelKey, d);

  dialog->addCheckItem(page, adxCheckKey, settings.getInt(adxCheckKey));

  page++;
  k = QObject::tr("ADXR");
  dialog->addPage(page, k);

  settings.getData(adxrColorKey, d);
  dialog->addColorItem(page, adxrColorKey, d);

  settings.getData(adxrPlotKey, d);
  dialog->addComboItem(page, adxrPlotKey, plotList, d);

  settings.getData(adxrLabelKey, d);
  dialog->addTextItem(page, adxrLabelKey, d);

  dialog->addCheckItem(page, adxrCheckKey, settings.getInt(adxrCheckKey));

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(periodKey, d);
  settings.setData(periodKey, d);

  dialog->getItem(mdiColorKey, d);
  settings.setData(mdiColorKey, d);

  dialog->getItem(mdiPlotKey, d);
  settings.setData(mdiPlotKey, d);

  dialog->getItem(mdiLabelKey, d);
  settings.setData(mdiLabelKey, d);

  settings.setData(mdiCheckKey, dialog->getCheck(mdiCheckKey));

  dialog->getItem(pdiColorKey, d);
  settings.setData(pdiColorKey, d);

  dialog->getItem(pdiPlotKey, d);
  settings.setData(pdiPlotKey, d);

  dialog->getItem(pdiLabelKey, d);
  settings.setData(pdiLabelKey, d);

  settings.setData(pdiCheckKey, dialog->getCheck(pdiCheckKey));

  dialog->getItem(adxColorKey, d);
  settings.setData(adxColorKey, d);

  dialog->getItem(adxPlotKey, d);
  settings.setData(adxPlotKey, d);

  dialog->getItem(adxLabelKey, d);
  settings.setData(adxLabelKey, d);

  settings.setData(adxCheckKey, dialog->getCheck(adxCheckKey));

  dialog->getItem(adxrColorKey, d);
  settings.setData(adxrColorKey, d);

  dialog->getItem(adxrPlotKey, d);
  settings.setData(adxrPlotKey, d);

  dialog->getItem(adxrLabelKey, d);
  settings.setData(adxrLabelKey, d);

  settings.setData(adxrCheckKey, dialog->getCheck(adxrCheckKey));

  delete dialog;
  return rc;
}


