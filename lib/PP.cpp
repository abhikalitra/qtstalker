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

#include "PP.h"
#include "BARS.h"

#include <QtDebug>


PP::PP ()
{
  indicator = "PP";
  r1cKey = QObject::tr("1R Color");
  r2cKey = QObject::tr("2R Color");
  r3cKey = QObject::tr("3R Color");
  s1cKey = QObject::tr("1S Color");
  s2cKey = QObject::tr("2S Color");
  s3cKey = QObject::tr("3S Color");
  r1lKey = QObject::tr("1R Label");
  r2lKey = QObject::tr("2R Label");
  r3lKey = QObject::tr("3R Label");
  s1lKey = QObject::tr("1S Label");
  s2lKey = QObject::tr("2S Label");
  s3lKey = QObject::tr("3S Label");
  r1ShowKey = QObject::tr("Show R1");
  r2ShowKey = QObject::tr("Show R2");
  r3ShowKey = QObject::tr("Show R3");
  s1ShowKey = QObject::tr("Show S1");
  s2ShowKey = QObject::tr("Show S2");
  s3ShowKey = QObject::tr("Show S3");

  QString d;
  d = "red";
  settings.setData(r1cKey, d);
  settings.setData(r2cKey, d);
  settings.setData(r3cKey, d);
  settings.setData(s1cKey, d);
  settings.setData(s2cKey, d);
  settings.setData(s3cKey, d);

  d = "1R";
  settings.setData(r1lKey, d);

  d = "2R";
  settings.setData(r2lKey, d);

  d = "3R";
  settings.setData(r3lKey, d);

  d = "1S";
  settings.setData(s1lKey, d);

  d = "2S";
  settings.setData(s2lKey, d);

  d = "3S";
  settings.setData(s3lKey, d);

  settings.setData(r1ShowKey, 1);
  settings.setData(r2ShowKey, 1);
  settings.setData(r3ShowKey, 1);
  settings.setData(s1ShowKey, 1);
  settings.setData(s2ShowKey, 1);
  settings.setData(s3ShowKey, 1);
}

int PP::getIndicator (Indicator &ind, BarData *data)
{
  BARS bars;
  int rc = bars.getIndicator(ind, data);
  if (rc)
    return 1;

  QString s;
  // 1R line
  if (settings.getInt(r1ShowKey))
  {
    PlotLine *line = getPP(data, 0);

    settings.getData(r1cKey, s);
    line->setColor(s);

    settings.getData(r1lKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  // 2R line
  if (settings.getInt(r2ShowKey))
  {
    PlotLine *line = getPP(data, 1);

    settings.getData(r2cKey, s);
    line->setColor(s);

    settings.getData(r2lKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  // 3R line
  if (settings.getInt(r3ShowKey))
  {
    PlotLine *line = getPP(data, 2);

    settings.getData(r3cKey, s);
    line->setColor(s);

    settings.getData(r3lKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  // 1S line
  if (settings.getInt(s1ShowKey))
  {
    PlotLine *line = getPP(data, 3);

    settings.getData(s1cKey, s);
    line->setColor(s);

    settings.getData(s1lKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  // 2S line
  if (settings.getInt(s2ShowKey))
  {
    PlotLine *line = getPP(data, 4);

    settings.getData(s2cKey, s);
    line->setColor(s);

    settings.getData(s2lKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  // 3S line
  if (settings.getInt(s3ShowKey))
  {
    PlotLine *line = getPP(data, 5);

    settings.getData(s3cKey, s);
    line->setColor(s);

    settings.getData(s3lKey, s);
    line->setLabel(s);

    ind.addLine(line);
  }

  return 0;
}

int PP::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PP,<NAME>,<POINT>

  if (set.count() != 4)
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
  int point = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid point" << set[3];
    return 1;
  }
  else
  {
    if (point < 0 || point > 5)
    {
      qDebug() << indicator << "::calculate: invalid point" << set[3];
      return 1;
    }
  }

  PlotLine *line = getPP(data, point);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * PP::getPP (BarData *data, int point)
{
  PlotLine *output = new PlotLine;
  output->setType(PlotLine::Horizontal);

  double high = data->getHigh(data->count() - 1);
  double low = data->getLow(data->count() - 1);
  double close = data->getClose(data->count() - 1);
  double pp = 0;
  double t = 0;

  switch (point)
  {
    case 0: // first resistance
      pp = (high + low + close) / 3;
      t = (2 * pp) - low;
      output->append(t);
      break;
    case 1: // second resistance
      pp = (high + low + close) / 3;
      t = pp + (high - low);
      output->append(t);
      break;
    case 2: // third resistance
      pp = (high + low + close) / 3;
      t = (2 * pp) + (high - (2 * low));
      output->append(t);
      break;
    case 3: // first support
      pp = (high + low + close) / 3;
      t = (2 * pp) - high;
      output->append(t);
      break;
    case 4: // second support
      pp = (high + low + close) / 3;
      t = pp - (high - low);
      output->append(t);
      break;
    case 5: // third support
      pp = (high + low + close) / 3;
      t = (2 * pp) - ((2 * high) - low);
      output->append(t);
      break;
    default:
      break;
  }

  return output;
}

int PP::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = "R1";
  dialog->addPage(page, k);

  settings.getData(r1cKey, d);
  dialog->addColorItem(page, r1cKey, d);

  settings.getData(r1lKey, d);
  dialog->addTextItem(page, r1lKey, d);

  dialog->addCheckItem(page, r1ShowKey, settings.getInt(r1ShowKey));

  page++;
  k = "R2";
  dialog->addPage(page, k);

  settings.getData(r2cKey, d);
  dialog->addColorItem(page, r2cKey, d);

  settings.getData(r2lKey, d);
  dialog->addTextItem(page, r2lKey, d);

  dialog->addCheckItem(page, r2ShowKey, settings.getInt(r2ShowKey));

  page++;
  k = "R3";
  dialog->addPage(page, k);

  settings.getData(r3cKey, d);
  dialog->addColorItem(page, r3cKey, d);

  settings.getData(r3lKey, d);
  dialog->addTextItem(page, r3lKey, d);

  dialog->addCheckItem(page, r3ShowKey, settings.getInt(r3ShowKey));

  page++;
  k = "S1";
  dialog->addPage(page, k);

  settings.getData(s1cKey, d);
  dialog->addColorItem(page, s1cKey, d);

  settings.getData(s1lKey, d);
  dialog->addTextItem(page, s1lKey, d);

  dialog->addCheckItem(page, s1ShowKey, settings.getInt(s1ShowKey));

  page++;
  k = "S2";
  dialog->addPage(page, k);

  settings.getData(s2cKey, d);
  dialog->addColorItem(page, s2cKey, d);

  settings.getData(s2lKey, d);
  dialog->addTextItem(page, s2lKey, d);

  dialog->addCheckItem(page, s2ShowKey, settings.getInt(s2ShowKey));

  page++;
  k = "S3";
  dialog->addPage(page, k);

  settings.getData(s3cKey, d);
  dialog->addColorItem(page, s3cKey, d);

  settings.getData(s3lKey, d);
  dialog->addTextItem(page, s3lKey, d);

  dialog->addCheckItem(page, s3ShowKey, settings.getInt(s3ShowKey));

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  QStringList keys;
  settings.getKeyList(keys);
  int loop;
  for (loop = 0; loop < keys.count(); loop++)
  {
    QString d;
    dialog->getItem(keys[loop], d);
    if (! d.isEmpty())
      settings.setData(keys[loop], d);
  }

  delete dialog;
  return rc;
}

