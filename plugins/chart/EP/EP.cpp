/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>

EP::EP ()
{
  pluginName = "EP";
  startX = 2;
  indicatorFlag = FALSE;
  loadSettings();  
}

EP::~EP ()
{
}

void EP::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;
  QColor color = neutralColor;

//  PlotLine *rank = new PlotLine;

/* This is an experiment with concepts of "One- and Two-bar Price Patterns"
*  For some explanation see:
*  "Technical Analysis Explained" by Martin Pring (Fourth Edition)
*  ISBN 0-07-138193-7
*  pp 111-135
*/

  // set first bar as neutral
//  QString color = getData(tr("Neutral"));
//  rank->append(0);
//  paintBars.append(new QColor(color));

  painter.setPen(color);
  double t = data->getOpen(loop);
  int y;
  if (t)
  {
    y = scaler->convertToY(t);
    painter.drawLine (x - 2, y, x, y);
  }
  y = scaler->convertToY(data->getClose(loop));
  painter.drawLine (x + 2, y, x, y);
  int h = scaler->convertToY(data->getHigh(loop));
  int l = scaler->convertToY(data->getLow(loop));
  painter.drawLine (x, h, x, l);
  x = x + pixelspace;
  loop++;

  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
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
    double exhaustionVolume = data->getVolume(loop);
    double exhaustionVolumePr = data->getVolume(loop - 1);
    double exhaustionVolumeDiff = exhaustionVolume / exhaustionVolumePr * 100;
    float exhaustionRange = data->getHigh(loop) - data->getLow(loop);
    float exhaustionRangePr = data->getHigh(loop - 1) - data->getLow(loop - 1);
    float exhaustionRangeDiff = exhaustionRange / exhaustionRangePr * 100;

    // outside bar
    if ((data->getHigh(loop) > data->getHigh(loop - 1)) &&
        (data->getLow(loop) < data->getLow(loop - 1))
       )
    {
      exhaustionOutsideRank = 1;

      // stronger outside bars - rank from 1..5
      // There are many factors which determine significance of an outside bar.
      // Add half a point for each situation.

      // encompass more previous bars
      if ((loop - 2) >= 0)
      {
        if ((data->getHigh(loop) > data->getHigh(loop - 2)) &&
            (data->getLow(loop) < data->getLow(loop - 2))
           )
        {
          exhaustionOutsideRank += 0.5;
          if ((loop - 3) >= 0)
          {
            if ((data->getHigh(loop) > data->getHigh(loop - 3)) &&
                (data->getLow(loop) < data->getLow(loop - 3))
               )
            {
              exhaustionOutsideRank += 0.5;
              if ((loop - 4) >= 0)
              {
                if ((data->getHigh(loop) > data->getHigh(loop - 4)) &&
                    (data->getLow(loop) < data->getLow(loop - 4))
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
//      rank->append((float) exhaustionOutsideRank);

      if (exhaustionOutsideRank > 5)
        exhaustionOutsideRank = 5;
      int exhaustionOutsideRankSwitch = (int) exhaustionOutsideRank;
      switch (exhaustionOutsideRankSwitch)
      {
        case 5:
          color = or5Color;
          break;
        case 4:
          color = or4Color;
          break;
        case 3:
          color = or3Color;
          break;
        case 2:
          color = or2Color;
          break;
        default:
          color = or1Color;
          break;
      }
    }

    // inside bar
    else if ((data->getHigh(loop) < data->getHigh(loop - 1)) &&
             (data->getLow(loop) > data->getLow(loop - 1)) &&
// need better volume test
             (data->getVolume(loop) > 0) && (data->getVolume(loop - 1) > 0)
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
//      rank->append((float) exhaustionInsideRank);

      if (exhaustionInsideRank > 5)
        exhaustionInsideRank = 5;
      int exhaustionInsideRankSwitch = (int) exhaustionInsideRank;
      switch (exhaustionInsideRankSwitch)
      {
        case 5:
          color = ir5Color;
          break;
        case 4:
          color = ir4Color;
          break;
        case 3:
          color = ir3Color;
          break;
        case 2:
          color = ir2Color;
          break;
        default:
          color = ir1Color;
          break;
      }
    }

    // plain bar
    else
    {
      color = neutralColor;
//      rank->append(0);
    }

//    paintBars.append(new QColor(color));

    painter.setPen(color);
    t = data->getOpen(loop);
    if (t)
    {
      y = scaler->convertToY(t);
      painter.drawLine (x - 2, y, x, y);
    }
    y = scaler->convertToY(data->getClose(loop));
    painter.drawLine (x + 2, y, x, y);
    h = scaler->convertToY(data->getHigh(loop));
    l = scaler->convertToY(data->getLow(loop));
    painter.drawLine (x, h, x, l);
    x = x + pixelspace;
    loop++;
  }

//  output.append(rank);
}

void EP::prefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("EP Chart Prefs"));
  dialog->createPage (tr("Parms"));
  dialog->addColorItem(tr("Neutral Color"), tr("Parms"), neutralColor);
  dialog->addIntItem(tr("Min Bar Spacing"), tr("Parms"), minPixelspace, 4, 99);
  
  dialog->createPage (tr("Outside Rank"));
  dialog->addColorItem(tr("Outside Rank 1"), tr("Outside Rank"), or1Color);
  dialog->addColorItem(tr("Outside Rank 2"), tr("Outside Rank"), or2Color);
  dialog->addColorItem(tr("Outside Rank 3"), tr("Outside Rank"), or3Color);
  dialog->addColorItem(tr("Outside Rank 4"), tr("Outside Rank"), or4Color);
  dialog->addColorItem(tr("Outside Rank 5"), tr("Outside Rank"), or5Color);
  
  dialog->createPage (tr("Inside Rank"));
  dialog->addColorItem(tr("Inside Rank 1"), tr("Inside Rank"), ir1Color);
  dialog->addColorItem(tr("Inside Rank 2"), tr("Inside Rank"), ir2Color);
  dialog->addColorItem(tr("Inside Rank 3"), tr("Inside Rank"), ir3Color);
  dialog->addColorItem(tr("Inside Rank 4"), tr("Inside Rank"), ir4Color);
  dialog->addColorItem(tr("Inside Rank 5"), tr("Inside Rank"), ir5Color);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    neutralColor = dialog->getColor(tr("Neutral Color"));
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    or1Color = dialog->getColor(tr("Outside Rank 1"));
    or2Color = dialog->getColor(tr("Outside Rank 2"));
    or3Color = dialog->getColor(tr("Outside Rank 3"));
    or4Color = dialog->getColor(tr("Outside Rank 4"));
    or5Color = dialog->getColor(tr("Outside Rank 5"));
    ir1Color = dialog->getColor(tr("Inside Rank 1"));
    ir2Color = dialog->getColor(tr("Inside Rank 2"));
    ir3Color = dialog->getColor(tr("Inside Rank 3"));
    ir4Color = dialog->getColor(tr("Inside Rank 4"));
    ir5Color = dialog->getColor(tr("Inside Rank 5"));
    
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void EP::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/EP plugin");

  minPixelspace = settings.readNumEntry("/minPixelspace", 4);
  
  neutralColor.setNamedColor(settings.readEntry("/NeutralColor", "dimgray"));
  or1Color.setNamedColor(settings.readEntry("/OutsideRank1", "khaki"));
  or2Color.setNamedColor(settings.readEntry("/OutsideRank2", "goldenrod"));
  or3Color.setNamedColor(settings.readEntry("/OutsideRank3", "gold"));
  or4Color.setNamedColor(settings.readEntry("/OutsideRank4", "darkorange"));
  or5Color.setNamedColor(settings.readEntry("/OutsideRank5", "red"));
  ir1Color.setNamedColor(settings.readEntry("/InsideRank1", "pink"));
  ir2Color.setNamedColor(settings.readEntry("/InsideRank2", "orchid"));
  ir3Color.setNamedColor(settings.readEntry("/InsideRank3", "magenta"));
  ir4Color.setNamedColor(settings.readEntry("/InsideRank4", "royalblue"));
  ir5Color.setNamedColor(settings.readEntry("/InsideRank5", "blue"));
  
  settings.endGroup();
}

void EP::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Bar plugin");
  
  settings.writeEntry("/minPixelspace", minPixelspace);
  settings.writeEntry("/NeutralColor", neutralColor.name());
  settings.writeEntry("/OutsideRank1", or1Color.name());
  settings.writeEntry("/OutsideRank2", or2Color.name());
  settings.writeEntry("/OutsideRank3", or3Color.name());
  settings.writeEntry("/OutsideRank4", or4Color.name());
  settings.writeEntry("/OutsideRank5", or5Color.name());
  settings.writeEntry("/InsideRank1", ir1Color.name());
  settings.writeEntry("/InsideRank2", ir2Color.name());
  settings.writeEntry("/InsideRank3", ir3Color.name());
  settings.writeEntry("/InsideRank4", ir4Color.name());
  settings.writeEntry("/InsideRank5", ir5Color.name());
  
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  EP *o = new EP;
  return ((Plugin *) o);
}



