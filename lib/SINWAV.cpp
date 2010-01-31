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

#include "SINWAV.h"

#include <QtDebug>
#include <math.h>
#include <QVector>

#define PI 3.14159265

SINWAV::SINWAV ()
{
  indicator = "SINWAV";
  scKey = QObject::tr("Sine Color");
  lcKey = QObject::tr("Lead Color");
  spKey = QObject::tr("Sine Plot");
  lpKey = QObject::tr("Lead Plot");
  slKey = QObject::tr("Sine Label");
  llKey = QObject::tr("Lead Label");

  QString d;
  d = "red";
  settings.setData(scKey, d);

  d = "yellow";
  settings.setData(lcKey, d);

  d = "Line";
  settings.setData(spKey, d);
  settings.setData(lpKey, d);

  d = "SINE";
  settings.setData(slKey, d);

  d = "LEAD";
  settings.setData(llKey, d);
}

int SINWAV::getIndicator (Indicator &ind, BarData *data)
{
  QList<PlotLine *> l;
  getSINWAV(data, l);
  if (l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  // sine line
  QString s;
  PlotLine *line = l.at(0);
  settings.getData(scKey, s);
  line->setColor(s);

  settings.getData(spKey, s);
  line->setType(s);

  settings.getData(slKey, s);
  line->setLabel(s);

  ind.addLine(line);

  // lead line
  line = l.at(1);
  settings.getData(lcKey, s);
  line->setColor(s);

  settings.getData(lpKey, s);
  line->setType(s);

  settings.getData(llKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int SINWAV::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // Ehler's sine wave
  // INDICATOR,SINWAV,<SINE_NAME>,<LEAD_NAME>

  if (set.count() != 4)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: invalid sine name" << set[2];
    return 1;
  }

  tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: invalid lead name" << set[3];
    return 1;
  }

  QList<PlotLine *> l;
  getSINWAV(data, l);
  if (l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[2], l.at(0)); // sine
  tlines.insert(set[3], l.at(1)); // lead

  return 0;
}

void SINWAV::getSINWAV (BarData *data, QList<PlotLine *> &l)
{
  PlotLine *Price = new PlotLine;

  int i = 0;

  // price = (h + l) / 2
  for (i = 0; i < (int) data->count(); i++)
    Price->append((data->getHigh(i) + data->getLow(i)) / 2);

  //! VERSION 2

  QVector<double> *smooth = new QVector<double>(Price->getSize());
  smooth->fill(0.0);
  QVector<double> *detrender = new QVector<double>(Price->getSize());
  detrender->fill(0.0);
  QVector<double> *period = new QVector<double>(Price->getSize());
  period->fill(0.0);
  QVector<double> *Q1 = new QVector<double>(Price->getSize());
  Q1->fill(0.0);
  QVector<double> *I1 = new QVector<double>(Price->getSize());
  I1->fill(0.0);
  QVector<double> *jI = new QVector<double>(Price->getSize());
  jI->fill(0.0);
  QVector<double> *jQ = new QVector<double>(Price->getSize());
  jQ->fill(0.0);
  QVector<double> *I2 = new QVector<double>(Price->getSize());
  I2->fill(0.0);
  QVector<double> *Q2 = new QVector<double>(Price->getSize());
  Q2->fill(0.0);
  QVector<double> *Re = new QVector<double>(Price->getSize());
  Re->fill(0.0);
  QVector<double> *Im = new QVector<double>(Price->getSize());
  Im->fill(0.0);
  QVector<double> *SmoothPrice = new QVector<double>(Price->getSize());
  SmoothPrice->fill(0.0);
  QVector<double> *DCPhase = new QVector<double>(Price->getSize());
  DCPhase->fill(0.0);

  PlotLine *out1 = new PlotLine;
  PlotLine *out2 = new PlotLine;

  double SmoothPeriod = 0;
  double DCPeriod = 0;

  for (i = 6; i< Price->getSize(); i++)
  {
    // Weighted price
    (*smooth)[i] = ( 4 * Price->getData(i) + 3 * Price->getData(i-1) +
		      2 * Price->getData(i-2) +  Price->getData(i-3)) /10.0;

    (*detrender)[i] = (0.0962 * smooth->at(i) + 0.5769 * smooth->at(i-2) -
			0.5769 * smooth->at(i-4) - 0.0962 * smooth->at(i-6)) *
		        (0.075 * period->at(i-1) + 0.54);

    // Compute InPhase and Quadrature components
    (*Q1)[i] = (0.0962 * detrender->at(i) + 0.5769 * detrender->at(i-2) -
		 0.5769 * detrender->at(i-4) - 0.0962 * detrender->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);
    (*I1)[i] = detrender->at(i-3);

    //Advance the phase of I1 and Q1 by 90 degrees
    (*jI)[i] = (0.0962 * I1->at(i) + 0.5769 * I1->at(i-2) -
		 0.5769 * I1->at(i-4) - 0.0962 * I1->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);

    (*Q1)[i] = (0.0962 * Q1->at(i) + 0.5769 * Q1->at(i-2) -
		 0.5769 * Q1->at(i-4) - 0.0962 * Q1->at(i-6)) *
		 (0.075 * period->at(i-1) + 0.54);

    // Phasor addition for 3-bar averaging
    (*I2)[i] = I1->at(i) - jQ->at(i);
    (*Q2)[i] = Q1->at(i) + jI->at(i);

    // Smooth the I and Q components before applying the discriminator
    (*I2)[i] = 0.2 * I2->at(i) + 0.8 * I2->at(i-1);
    (*Q2)[i] = 0.2 * Q2->at(i) + 0.8 * Q2->at(i-1);

    // Homodyne Discriminator
    (*Re)[i] = I2->at(i) * I2->at(i-1) + Q2->at(i) * Q2->at(i-1);
    (*Im)[i] = I2->at(i) * Q2->at(i-1) - Q2->at(i) * I2->at(i-1);
    (*Re)[i] = 0.2 * Re->at(i) + 0.8 * Re->at(i-1);
    (*Im)[i] = 0.2 * Im->at(i) + 0.8 * Im->at(i-1);
    if (Im->at(i) != 0 && Re->at(i) != 0 )
      (*period)[i] = 360/(atan(Im->at(i) / Re->at(i)) * (180/PI));
    if (period->at(i) > 1.5 * period->at(i-1))
      (*period)[i] = 1.5 * period->at(i-1);
    if (period->at(i) < 0.67 * period->at(i-1))
      (*period)[i] = 0.67 * period->at(i-1);
    if (period->at(i) < 6)
      (*period)[i] = 6;
    if (period->at(i) > 50)
      (*period)[i] = 50;

    (*period)[i] = 0.2 * period->at(i) + 0.8 * period->at(i-1);

    SmoothPeriod = 0.33 * period->at(i) + 0.67 * SmoothPeriod;

    // Compute Dominant CyclePhase
    (*SmoothPrice)[i] = (4 * Price->getData(i) + 3 * Price->getData(i-1) +
			  2 * Price->getData(i-2) +  Price->getData(i-3)) /10.0;

    DCPeriod = (int)(SmoothPeriod + 0.5);

    double RealPart = 0;
    double ImagPart = 0;
    int j = 0;

    if (i >= (int)DCPeriod)
    {
      for (j = 0; j < (int)DCPeriod; j++)
      {
        RealPart = RealPart + sin((360 * (double)j / DCPeriod) * (PI/180)) * (SmoothPrice->at(i - j));
	ImagPart = ImagPart + cos((360 * (double)j / DCPeriod) * (PI/180)) * (SmoothPrice->at(i - j));
      }
    }

    if (fabs(ImagPart) > 0.001)
      (*DCPhase)[i] = atan(RealPart / ImagPart) * (180/PI);

    if( fabs(ImagPart) <= 0.001 )
      (*DCPhase)[i] = 90.0 * sin(RealPart * (PI/180));

    (*DCPhase)[i] = DCPhase->at(i) + 90.0;

    // Compensate for one bar lag of the Weighted Moving Average
    (*DCPhase)[i] = DCPhase->at(i) + 360.0 / SmoothPeriod;

    if (ImagPart < 0)
      (*DCPhase)[i] = DCPhase->at(i) + 180.0;

    if (DCPhase->at(i) > 315.0)
      (*DCPhase)[i] = DCPhase->at(i) - 360.0;

    // we're done....
    out1->append(sin(DCPhase->at(i) * (PI/180)));
    out2->append(sin((DCPhase->at(i) + 45.0) * (PI/180)));
  }

  delete smooth;
  delete detrender;
  delete period;
  delete Q1;
  delete I1;
  delete jI;
  delete jQ;
  delete I2;
  delete Q2;
  delete Re;
  delete Im;
  delete SmoothPrice;
  delete DCPhase;
  delete Price;

  l.append(out1); // sine
  l.append(out2); // lead
}

int SINWAV::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Sine");
  dialog->addPage(page, k);

  settings.getData(scKey, d);
  dialog->addColorItem(page, scKey, d);

  settings.getData(spKey, d);
  dialog->addComboItem(page, spKey, plotList, d);

  settings.getData(slKey, d);
  dialog->addTextItem(page, slKey, d);

  page++;
  k = QObject::tr("Lead");
  dialog->addPage(page, k);

  settings.getData(lcKey, d);
  dialog->addColorItem(page, lcKey, d);

  settings.getData(lpKey, d);
  dialog->addComboItem(page, lpKey, plotList, d);

  settings.getData(llKey, d);
  dialog->addTextItem(page, llKey, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(scKey, d);
  settings.setData(scKey, d);

  dialog->getItem(spKey, d);
  settings.setData(spKey, d);

  dialog->getItem(slKey, d);
  settings.setData(slKey, d);

  dialog->getItem(lcKey, d);
  settings.setData(lcKey, d);

  dialog->getItem(lpKey, d);
  settings.setData(lpKey, d);

  dialog->getItem(llKey, d);
  settings.setData(llKey, d);

  delete dialog;
  return rc;
}

