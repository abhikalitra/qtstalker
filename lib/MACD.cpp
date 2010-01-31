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

#include "MACD.h"

#include <QtDebug>


MACD::MACD ()
{
  indicator = "MACD";
  macdcKey = QObject::tr("MACD Color");
  sigcKey = QObject::tr("Signal Color");
  histcKey = QObject::tr("Histogram Color");
  macdpKey = QObject::tr("MACD Plot");
  sigpKey = QObject::tr("Signal Plot");
  histpKey = QObject::tr("Histogram Plot");
  macdlKey = QObject::tr("MACD Label");
  siglKey = QObject::tr("Signal Label");
  histlKey = QObject::tr("Histogram Label");
  fpKey = QObject::tr("Fast Period");
  spKey = QObject::tr("Slow Period");
  sigpdKey = QObject::tr("Signal Period");

  QString d;
  d = "red";
  settings.setData(macdcKey, d);

  d = "yellow";
  settings.setData(sigcKey, d);

  d = "blue";
  settings.setData(histcKey, d);

  d = "Line";
  settings.setData(macdpKey, d);

  d = "Dash";
  settings.setData(sigpKey, d);

  d = "Histogram";
  settings.setData(histpKey, d);

  d = "MACD";
  settings.setData(macdlKey, d);

  d = "SIG";
  settings.setData(siglKey, d);

  d = "HIST";
  settings.setData(histlKey, d);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(fpKey, 12);
  settings.setData(spKey, 26);
  settings.setData(sigpdKey, 9);
}

int MACD::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int fast = settings.getInt(fpKey);
  int slow = settings.getInt(spKey);
  int signal = settings.getInt(sigpdKey);

  QList<PlotLine *> l;
  int rc = getMACD(in, fast, slow, signal, l);
  if (rc)
  {
    qDeleteAll(l);
    delete in;
    return 1;
  }

  // macd line
  PlotLine *line = l.at(0);
  settings.getData(macdcKey, s);
  line->setColor(s);

  settings.getData(macdpKey, s);
  line->setType(s);

  settings.getData(macdlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // signal line
  line = l.at(1);
  settings.getData(sigcKey, s);
  line->setColor(s);

  settings.getData(sigpKey, s);
  line->setType(s);

  settings.getData(siglKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // hist line
  line = l.at(2);
  settings.getData(histcKey, s);
  line->setColor(s);

  settings.getData(histpKey, s);
  line->setType(s);

  settings.getData(histlKey, s);
  line->setLabel(s);

  ind.addLine(line);

  delete in;

  return 0;
}

int MACD::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MACD,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<FAST_PERIOD>,
  // <SLOW_PERIOD>,<SIGNAL_PERIOD>

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

  tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[5];
    return 1;
  }

  PlotLine *in = tlines.value(set[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[2]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[2];
      return 1;
    }

    tlines.insert(set[2], in);
  }

  bool ok;
  int fast = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fast period" << set[6];
    return 1;
  }

  int slow = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid slow period" << set[7];
    return 1;
  }

  int signal = set[8].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid signal period" << set[8];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getMACD(in, fast, slow, signal, l);
  if (rc)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[3], l.at(0));
  tlines.insert(set[4], l.at(1));
  tlines.insert(set[5], l.at(2));

  return 0;
}

int MACD::getMACD (PlotLine *in, int fast, int slow, int signal, QList<PlotLine *> &l)
{
  int size = in->getSize();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_MACD(0, size - 1, &input[0], fast, slow, signal, &outBeg, &outNb, &out[0], &out2[0], &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *sig = new PlotLine;
  PlotLine *hist = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    sig->append(out2[loop]);
    hist->append(out3[loop]);
  }

  l.append(line);
  l.append(sig);
  l.append(hist);

  return 0;
}

int MACD::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(inputKey, d);
  dialog->addComboItem(page, inputKey, inputList, d);

  dialog->addIntItem(page, fpKey, settings.getInt(fpKey), 2, 100000);

  dialog->addIntItem(page, spKey, settings.getInt(spKey), 2, 100000);

  dialog->addIntItem(page, sigpdKey, settings.getInt(sigpdKey), 1, 100000);

  page++;
  k = QObject::tr("MACD");
  dialog->addPage(page, k);

  settings.getData(macdcKey, d);
  dialog->addColorItem(page, macdcKey, d);

  settings.getData(macdpKey, d);
  dialog->addComboItem(page, macdpKey, plotList, d);

  settings.getData(macdlKey, d);
  dialog->addTextItem(page, macdlKey, d);

  page++;
  k = QObject::tr("Signal");
  dialog->addPage(page, k);

  settings.getData(sigcKey, d);
  dialog->addColorItem(page, sigcKey, d);

  settings.getData(sigpKey, d);
  dialog->addComboItem(page, sigpKey, plotList, d);

  settings.getData(siglKey, d);
  dialog->addTextItem(page, siglKey, d);

  page++;
  k = QObject::tr("Hist");
  dialog->addPage(page, k);

  settings.getData(histcKey, d);
  dialog->addColorItem(page, histcKey, d);

  settings.getData(histpKey, d);
  dialog->addComboItem(page, histpKey, plotList, d);

  settings.getData(histlKey, d);
  dialog->addTextItem(page, histlKey, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(macdcKey, d);
  settings.setData(macdcKey, d);

  dialog->getItem(macdpKey, d);
  settings.setData(macdpKey, d);

  dialog->getItem(macdlKey, d);
  settings.setData(macdlKey, d);

  dialog->getItem(sigcKey, d);
  settings.setData(sigcKey, d);

  dialog->getItem(sigpKey, d);
  settings.setData(sigpKey, d);

  dialog->getItem(siglKey, d);
  settings.setData(siglKey, d);

  dialog->getItem(histcKey, d);
  settings.setData(histcKey, d);

  dialog->getItem(histpKey, d);
  settings.setData(histpKey, d);

  dialog->getItem(histlKey, d);
  settings.setData(histlKey, d);

  dialog->getItem(inputKey, d);
  settings.setData(inputKey, d);

  dialog->getItem(fpKey, d);
  settings.setData(fpKey, d);

  dialog->getItem(spKey, d);
  settings.setData(spKey, d);

  dialog->getItem(sigpdKey, d);
  settings.setData(sigpdKey, d);

  delete dialog;
  return rc;
}

