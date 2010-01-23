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

#include "LMS.h"
#include "ta_libc.h"

#include <QtDebug>
#include <math.h>


LMS::LMS ()
{
  cycleFlag = TRUE;
  fkPeriod = 5;
  skPeriod = 5;
  cmbIndex = 1;
}

int LMS::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,LMS,<SLOWK_NAME>,<2DAY_NAME>,<5DAY_NAME>,<FK_PERIOD>,<SK_PERIOD>,<CMB_INDEX>

  if (set.count() != 8)
  {
    qDebug() << "LMS::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate slowk name" << set[2];
    return 1;
  }
  else
    slowkName = set[2];

  tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate 2day name" << set[3];
    return 1;
  }
  else
    Day2Name = set[3];

  tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate 5day name" << set[4];
    return 1;
  }
  else
    Day5Name = set[4];

  bool ok;
  fkPeriod = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "LMS::calculate: invalid fk period" << set[5];
    return 1;
  }

  skPeriod = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "LMS::calculate: invalid sk period" << set[6];
    return 1;
  }

  cmbIndex = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "LMS::calculate: invalid cmb index" << set[7];
    return 1;
  }
  else
  {
    if (cmbIndex < 0 || cmbIndex > 1)
    {
      qDebug() << "LMS::calculate: invalid cmb index" << set[7];
      return 1;
    }
  }

  getLMS(tlines, data);

  return 0;
}

void LMS::getLMS (QHash<QString, PlotLine *> &tlines, BarData *data)
{
  int i = 0;
  double sigPower = 0;
  double mu = 0;
  double xBar = 0;

  PlotLine *price = new PlotLine;

  //price = (h + l) / 2
  for (i = 0; i < (int) data->count(); i++)
    price->append((data->getHigh(i) + data->getLow(i)) / 2);

  PlotLine *price_offset_4 = new PlotLine;
  //temps
  PlotLine *temp1 = new PlotLine;

  PlotLine *value1 = new PlotLine;
  //dual mode
  PlotLine *value2 = new PlotLine;
  PlotLine *value3 = new PlotLine;

  QVector< double > g(30);
  QVector< double > sigPredict(30);

  g.fill(0.0);
  sigPredict.fill(0.0);

  //Counter trend mode-- -- ------------------------------------------------------
  if (! cycleFlag)
  {
    int offset = 4;

    //price[4]
    for (i = 0; i < price->getSize(); i++)
    {
      if (i > offset - 1)
	price_offset_4->append(price->getData(i - offset));
    }

    //0.5(price - price[4]
    int priceIndex = price->getSize() - 1;
    for (i = price_offset_4->getSize() - 1; i >= 0; i--)
    {
      temp1->prepend(0.5 * (price->getData(priceIndex) - price_offset_4->getData(i)));
      priceIndex--;
    }

    //0.25(price + 0.5(price - price[4])
    priceIndex = price->getSize() - 1;
    for (i = temp1->getSize() - 1; i >= 0; i--)
    //work back for simplicity
    {
      temp1->setData(i, 0.25 * (temp1->getData(i) + price->getData(priceIndex)));
      priceIndex--;
    }

    //Value1 = .25 * (Price + .5 * (Price - Price[4])) + .75 * Value1[1];
    for (i = 0; i < temp1->getSize(); i++)
    {
      if (i == 0)
	value1->append(temp1->getData(i));
      else
	value1->append(temp1->getData(i) + (value1->getData(i - 1) * 0.75));
    }
  } //end counter trend mode

  // Cycle mode-- -- -----------------------------------------------------------------------
  else
  {
    //slowK(price, length, length);
    temp1 = slowK(price, fkPeriod, skPeriod);

    //0.2(2 * (SlowK(price, length, length) / 100 - .5))
    for (i = 0; i < temp1->getSize(); i++)
      temp1->setData(i, 0.2 * (2 * (temp1->getData(i)) / (100 - 0.5)));

    //Value1 = .2 * (2 * (SlowK(Length) / 100 - .5)) + .8 * Value1[1];
    for (i = 0; i < temp1->getSize(); i++)
    {
      if (i == 0)
	value1->append(temp1->getData(i));
      else
	value1->append(temp1->getData(i) + value1->getData(i - 1) * 0.8);
    }
  } //end cycle mode
    // end cycleFlag-- -- --------------------------------------------------------------------

  //if currentBar > length
  int j = 0;
  for (i = skPeriod; i < value1->getSize(); i++)
  {
    //Compute average power for normalization
    sigPower = 0;
    for (j = 0; j < skPeriod; j++)
      sigPower = sigPower + value1->getData(i - j) * value1->getData(i - j);

    sigPower /= skPeriod;

    //Convergence Factor
    if (sigPower > 0)
    mu = 0.25 / (sigPower * skPeriod);

    //Compute signal estimate
    xBar = 0;
    for (j = 1; j <= skPeriod; j++)
      xBar = xBar + value1->getData(i - j) * g[j];

    //Compute gain coefficients
    for (j = 1; j <= skPeriod; j++)
      g[j] = g[j] + (mu * (value1->getData(i) - xBar) * value1->getData(i - j));

    //Compute signal prediction waveform
    for (j = 0; j <= skPeriod; j++)
      sigPredict[j] = value1->getData(i - (skPeriod - j));

    //Extend signal prediction into the future
    int k = 0;
    for (j = skPeriod + 1; j <= skPeriod + 5; j++)
    {
      sigPredict[j] = 0;

      for (k = 1; k <= skPeriod; k++)
	sigPredict[j] = sigPredict[j] + sigPredict[j - k] * g[k];
    }

    value2->append(sigPredict[skPeriod + 2]);
    value3->append(sigPredict[skPeriod + 5]);
  }

  delete price;
  delete price_offset_4;
  delete temp1;

  //remove the first 1.5 * length values-- initial ramp up messes with scaling
  // ***n.b need a delete(index) function for PlotLine ***
  PlotLine *temp1a = new PlotLine;
  PlotLine *temp2 = new PlotLine;
  PlotLine *temp3 = new PlotLine;

  int index1 = value1->getSize() -1;
  int index2 = value2->getSize() - 1;
  int index3 = value3->getSize() - 1;

  for (i = value2->getSize() - 1; i >= (int)skPeriod * 1.5; i--)
  {
    temp1a->prepend(value1->getData(index1));
    temp2->prepend(value2->getData(index2));
    temp3->prepend(value3->getData(index3));

    index1--;
    index2--;
    index3--;
  }

  delete value1;
  delete value2;
  delete value3;

  value1 = temp1a; // slowk
  tlines.insert(slowkName, value1);

  value2 = temp2; // 2day
  tlines.insert(Day2Name, value2);

  value3 = temp3; // 5day
  tlines.insert(Day5Name, value3);
}

PlotLine *LMS::slowK(PlotLine * inLine, int kPeriod, int slowKperiod)
{
  PlotLine *k = new PlotLine();
  int loop;
  for (loop = kPeriod; loop < (int)inLine->getSize(); loop++)
  {
    int loop2;
    double l;
    double h;
    for (loop2 = 0, l = 9999999, h = 0; loop2 < kPeriod; loop2++)
    {
      double t = inLine->getData(loop - loop2);
      if (t > h)
	h = t;
      if (t < l)
	l = t;
    }

    double close = inLine->getData(loop);
    double t = ((close - l) / (h - l)) * 100;
    if (t > 100)
      t = 100;
    if (t < 0)
      t = 0;

    k->append(t);
  }

  // get MA
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[k->getSize()];
  TA_Real out[k->getSize()];
  for (loop = 0; loop < k->getSize(); loop++)
    input[loop] = (TA_Real) k->getData(loop);

  TA_SMA(0, k->getSize() - 1, &input[0], slowKperiod, &outBeg, &outNb, &out[0]);

  PlotLine *k2 = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    k2->append(out[loop]);
  delete k;
  k = k2;
  return k;
}

