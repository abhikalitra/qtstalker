/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "EP.h"

EP::EP ()
{
  pluginName = "EP";
  version = 0.2;

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Neutral"), "dimgray", Setting::Color);
  set(tr("Outside Rank 5"), "red", Setting::Color);
  set(tr("Outside Rank 4"), "darkorange", Setting::Color);
  set(tr("Outside Rank 3"), "gold", Setting::Color);
  set(tr("Outside Rank 2"), "goldenrod", Setting::Color);
  set(tr("Outside Rank 1"), "khaki", Setting::Color);
  set(tr("Inside Rank 5"), "blue", Setting::Color);
  set(tr("Inside Rank 4"), "royalblue", Setting::Color);
  set(tr("Inside Rank 3"), "magenta", Setting::Color);
  set(tr("Inside Rank 2"), "orchid", Setting::Color);
  set(tr("Inside Rank 1"), "pink", Setting::Color);
  set(tr("Line Type"), "Bar", Setting::None);
  set(tr("Label"), pluginName, Setting::Text);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  about = "Exhaustion Points\n";
}

EP::~EP ()
{
}

void EP::calculate ()
{
  paintBars.clear();

  PlotLine *rank = new PlotLine;
  rank->setType(tr("Invisible"));
  rank->setLabel(getData(tr("Label")));

/* This is an experiment with concepts of "One- and Two-bar Price Patterns"
*  For some explanation see:
*  "Technical Analysis Explained" by Martin Pring (Fourth Edition)
*  ISBN 0-07-138193-7
*  pp 111-135
*/

  // set first bar as neutral
  QString color = getData(tr("Neutral"));
  rank->append(0);
  paintBars.append(new QColor(color));

  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    Setting *r = data->at(loop);
    Setting *pr = data->at(loop - 1);

/* FIXME
* - Initial implementation only attempts "outside bars" and "inside bars".
* - Not yet doing "exhaustion bars" or "two-bar reversals".
* - Volume must accompany (not yet detected).
* - Strong trend should precede (not yet detected).
* - Detect more significance of bars.
* - Decide what to do when o = h = l = c ... gives indicator which may be false.
*/
    float exhaustionOutsideRank = 0;
    float exhaustionInsideRank = 0;
    double exhaustionVolume = r->getFloat("Volume");
    double exhaustionVolumePr = pr->getFloat("Volume");
    double exhaustionVolumeDiff = exhaustionVolume / exhaustionVolumePr * 100;
    float exhaustionRange = r->getFloat("High") - r->getFloat("Low");
    float exhaustionRangePr = pr->getFloat("High") - pr->getFloat("Low");
    float exhaustionRangeDiff = exhaustionRange / exhaustionRangePr * 100;

    // outside bar
    if ((r->getFloat("High") > pr->getFloat("High")) &&
        (r->getFloat("Low") < pr->getFloat("Low"))
       )
    {
      exhaustionOutsideRank = 1;

      // stronger outside bars - rank from 1..5
      // There are many factors which determine significance of an outside bar.
      // Add half a point for each situation.

      // encompass more previous bars
      if ((loop - 2) >= 0)
      {
        Setting *pr2 = data->at(loop - 2);
        if ((r->getFloat("High") > pr2->getFloat("High")) &&
            (r->getFloat("Low") < pr2->getFloat("Low"))
           )
        {
          exhaustionOutsideRank += 0.5;
          if ((loop - 3) >= 0)
          {
            Setting *pr3 = data->at(loop - 3);
            if ((r->getFloat("High") > pr3->getFloat("High")) &&
                (r->getFloat("Low") < pr3->getFloat("Low"))
               )
            {
              exhaustionOutsideRank += 0.5;
              if ((loop - 4) >= 0)
              {
                Setting *pr4 = data->at(loop - 4);
                if ((r->getFloat("High") > pr4->getFloat("High")) &&
                    (r->getFloat("Low") < pr4->getFloat("Low"))
                   )
                {
                  exhaustionOutsideRank += 0.5;
                }
              }
            }
          }
        }
      }

      // much greater range than the previous bar
      if (exhaustionRangeDiff >= 500)
        exhaustionOutsideRank += 0.5;
      if (exhaustionRangeDiff >= 900)
        exhaustionOutsideRank += 0.5;

      // greater volume than the previous bar
      if (exhaustionVolumeDiff >= 150)
        exhaustionOutsideRank += 0.5;
      if (exhaustionVolumeDiff >= 300)
        exhaustionOutsideRank += 0.5;

      // assign colour for the final ranking
      rank->append((float) exhaustionOutsideRank);
      if (exhaustionOutsideRank > 5)
        exhaustionOutsideRank = 5;
      int exhaustionOutsideRankSwitch = (int) exhaustionOutsideRank;
      switch (exhaustionOutsideRankSwitch)
      {
        case 5:
          color = getData(tr("Outside Rank 5"));
          break;
        case 4:
          color = getData(tr("Outside Rank 4"));
          break;
        case 3:
          color = getData(tr("Outside Rank 3"));
          break;
        case 2:
          color = getData(tr("Outside Rank 2"));
          break;
        default:
          color = getData(tr("Outside Rank 2"));
          break;
      }
    }

    // inside bar
    else if ((r->getFloat("High") < pr->getFloat("High")) &&
             (r->getFloat("Low") > pr->getFloat("Low")) &&
// need better volume test
             (r->getFloat("Volume") > 0) && (pr->getFloat("Volume") > 0)
       )
    {
      exhaustionInsideRank = 1;

      // stronger inside bars - rank from 1..5
      // There are many factors which determine significance of an inside bar.
      // Add half a point for each situation.

      // much smaller range than the previous bar
      if (exhaustionRangeDiff <= 25) exhaustionInsideRank += 0.5;
      if (exhaustionRangeDiff <= 10) exhaustionInsideRank += 0.5;

      // lesser volume than the previous bar
      if (exhaustionVolumeDiff <= 50) exhaustionInsideRank += 0.5;
      if (exhaustionVolumeDiff <= 20) exhaustionInsideRank += 0.5;

      // assign colour for the final ranking
      rank->append((float) exhaustionInsideRank);
      if (exhaustionInsideRank > 5)
        exhaustionInsideRank = 5;
      int exhaustionInsideRankSwitch = (int) exhaustionInsideRank;
      switch (exhaustionInsideRankSwitch)
      {
        case 5:
          color = getData(tr("Inside Rank 5"));
          break;
        case 4:
          color = getData(tr("Inside Rank 4"));
          break;
        case 3:
          color = getData(tr("Inside Rank 3"));
          break;
        case 2:
          color = getData(tr("Inside Rank 2"));
          break;
        default:
          color = getData(tr("Inside Rank 1"));
          break;
      }
    }

    // plain bar
    else
    {
      color = getData(tr("Neutral"));
      rank->append(0);
    }

    paintBars.append(new QColor(color));
  }

  output.append(rank);
}

QList<QColor> EP::getColorBars (QString, QString, QString)
{
  return paintBars;
}

Plugin * create ()
{
  EP *o = new EP;
  return ((Plugin *) o);
}



