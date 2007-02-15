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

#include "ChartToolbar.h"
//#include "Config.h"
#include "RcFile.h"
#include "BarData.h"
#include "PrefDialog.h"
#include <qtooltip.h>
#include <qaccel.h>
#include <qvalidator.h>
#include "../pics/date.xpm"
#include "../pics/next.xpm"

ChartToolbar::ChartToolbar (QMainWindow *mw) : QToolBar (mw, "chartToolbar")
{
  minPixelspace = 2;
  RcFile rcfile;
  QString ts; // temporary string
  int ti;  // temporary integer
  
  ptDate = QDateTime::currentDateTime();
  
  BarData bd(ts);
  compressionCombo = new QComboBox(this);
  compressionCombo->show();
//  compressionCombo->hide();
  bd.getBarLengthList(compressionList);
  compressionCombo->insertStringList(compressionList, -1);
  
  rcfile.loadData(RcFile::BarLength, ti);
  compressionCombo->setCurrentItem((BarData::BarLength) ti);
  QToolTip::add(compressionCombo, tr("Bar Length"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SIGNAL(signalBarLengthChanged(int)));
    
  cmpsBtnW = new QToolButton(this); // compression button weekly
  QToolTip::add(cmpsBtnW, tr("Weekly Compression"));
  cmpsBtnW->setText("W");
  connect(cmpsBtnW, SIGNAL(clicked()), this, SLOT(cmpsBtnWClicked()));
  
  cmpsBtnD = new QToolButton(this); // compression button daily
  QToolTip::add(cmpsBtnW, tr("Daily Compression"));
  cmpsBtnD->setText("D");
  connect(cmpsBtnD, SIGNAL(clicked()), this, SLOT(cmpsBtnDClicked()));
  
  cmpsBtn15 = new QToolButton(this); // compression button 15min
  QToolTip::add(cmpsBtnW, tr("15min Compression"));
  cmpsBtn15->setText("15");
  connect(cmpsBtn15, SIGNAL(clicked()), this, SLOT(cmpsBtn15Clicked()));
  
  addSeparator();

  pixelspace = new QSpinBox(this);
  pixelspace->setRange(minPixelspace, 99);
  rcfile.loadData(RcFile::Pixelspace, ti);
  pixelspace->setValue(ti);
  connect (pixelspace, SIGNAL(valueChanged(int)), this, SIGNAL(signalPixelspaceChanged(int)));
  QToolTip::add(pixelspace, tr("Bar Spacing"));
  pixelspace->setMaximumWidth(40); // FIXME:calc as a function of app font metrics
 
  ps1Button = new QToolButton(this);
  rcfile.loadData(RcFile::PSButton, ts, 1);
  QToolTip::add(ps1Button, tr("Set Bar Spacing to " + ts));
  ps1Button->setText(ts);
  connect(ps1Button, SIGNAL(clicked()), this, SLOT(ps1ButtonClicked()));

  ps2Button = new QToolButton(this);
  rcfile.loadData(RcFile::PSButton, ts, 2);
  QToolTip::add(ps2Button, tr("Set Bar Spacing to " + ts));
  ps2Button->setText(ts);
  connect(ps2Button, SIGNAL(clicked()), this, SLOT(ps2ButtonClicked()));

  ps3Button = new QToolButton(this);
  rcfile.loadData(RcFile::PSButton, ts, 3);
  QToolTip::add(ps3Button, tr("Set Bar Spacing to " + ts));
  ps3Button->setText(ts);
  connect(ps3Button, SIGNAL(clicked()), this, SLOT(ps3ButtonClicked()));

  addSeparator();
  
  QIntValidator *iv = new QIntValidator(1, 99999, this, 0);
  
  barCount = new QLineEdit(this);
  barCount->setValidator(iv);
  rcfile.loadData(RcFile::Bars, ts);
  barCount->setText(ts);
  barCount->setMaximumWidth(40);// FIXME:calc as a function of app font metrics
  QToolTip::add(barCount, tr("Total bars to load"));
  connect(barCount, SIGNAL(returnPressed()), this, SLOT(barsChanged()));

  addSeparator();

  ptdButton = new QToolButton(this);
  QToolTip::add(ptdButton, tr("Paper Trade Date"));
  ptdButton->setPixmap(date);
  connect(ptdButton, SIGNAL(clicked()), this, SLOT(paperTradeDate()));

  ptnButton = new QToolButton(this);
  QToolTip::add(ptnButton, tr("Load Next Bar"));
  ptnButton->setPixmap(next);
  connect(ptnButton, SIGNAL(clicked()), this, SLOT(paperTradeNextBar()));
//  ptnButton->setFocusPolicy(QWidget::StrongFocus);

  slider = new QSlider(this);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalSliderChanged(int)));
  slider->setEnabled(FALSE);
  QToolTip::add(slider, tr("Pan Chart"));

  setStretchableWidget(slider);
  
  QAccel *a = new QAccel(mw);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_End, ChartPannerFocus);
  a->insertItem(CTRL+Key_Plus, BarsLoadedFocus);
  a->insertItem(CTRL+Key_Minus, BarSpacingFocus);
  a->insertItem(CTRL+Key_Prior, BarLengthFocus);
  a->insertItem(CTRL+Key_B, ToolbarFocus);
  
  focusFlag = BarLengthFocus;
  
  connect(this, SIGNAL(orientationChanged( Orientation)), this, SLOT(slotOrientationChanged(Orientation)));
  
  setVerticallyStretchable(TRUE);
}

ChartToolbar::~ChartToolbar ()
{
}

int ChartToolbar::getBars ()
{
  bool ok;
  int t = barCount->text().toInt(&ok);
  if (ok)
    return t;
  else
    return 0;
}

void ChartToolbar::enableSlider (bool d)
{
  slider->setEnabled(d);
}

void ChartToolbar::setPixelspace (int min, int d)
{
  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(min, 99);
  pixelspace->setValue(d);
  pixelspace->blockSignals(FALSE);
}

int ChartToolbar::getPixelspace ()
{
  return pixelspace->value();
}

int ChartToolbar::getBarLengthInt ()
{
  return compressionCombo->currentItem();
}

QString ChartToolbar::getBarLength ()
{
  return compressionCombo->currentText();
}

int ChartToolbar::getSlider ()
{
  return slider->value();
}

int ChartToolbar::setSliderStart (int width, int records)
{
  int page = width / getPixelspace();
  int max = records - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, records - 1);
  int rc = 0;
  
  if ((int) records < page)
    slider->setValue(0);
  else
  {
    slider->setValue(max + 1);
    rc = max + 1;
  }
  
  slider->blockSignals(FALSE);
  
  return rc;
}

void ChartToolbar::saveSettings ()
{
  RcFile rcfile;
  
  rcfile.saveData(RcFile::Bars, getBars());
  rcfile.saveData(RcFile::BarLength, getBarLengthInt());
  rcfile.saveData(RcFile::Pixelspace, getPixelspace());
}

void ChartToolbar::setFocus ()
{
  compressionCombo->setFocus();
  focusFlag = BarLengthFocus;
}

void ChartToolbar::slotAccel (int id)
{
  switch (id)
  {
    case BarLengthFocus:
      compressionCombo->setFocus();
      focusFlag = BarLengthFocus;
      break;  
    case ChartPannerFocus:
      slider->setFocus();
      focusFlag = ChartPannerFocus;
      break;  
    case BarsLoadedFocus:
      barCount->setFocus();
      focusFlag = BarsLoadedFocus;
      break;  
    case BarSpacingFocus:
      pixelspace->setFocus();
      focusFlag = BarSpacingFocus;
      break;  
    case ToolbarFocus:
      setFocus();
      break;
    default:
      break;
  }
}

void ChartToolbar::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (! key->state())
  {
    switch(focusFlag)
    {
      case BarLengthFocus:
//        compressionCombo->doKeyPress(key);
	break;
      case BarSpacingFocus:
//        pixelspace->doKeyPress(key);
	break;
      case BarsLoadedFocus:
//        barCount->doKeyPress(key);
	break;
      case ChartPannerFocus:
//        slider->doKeyPress(key);
	break;
      default:
        break;
    }
  }
  else
  {
    if (key->state() == Qt::ControlButton)
    {
      switch (key->key())
      {
        case Qt::Key_Prior:
	  slotAccel(BarLengthFocus);
          break;
        case Qt::Key_Plus:
	  slotAccel(BarsLoadedFocus);
          break;
        case Qt::Key_End:
	  slotAccel(ChartPannerFocus);
          break;
        case Qt::Key_Minus:
	  slotAccel(BarSpacingFocus);
          break;
        case Qt::Key_B:
	  slotAccel(ToolbarFocus);
          break;
        default:
          break;
      }
    }
  }
}

void ChartToolbar::barsChanged ()
{
  bool ok;
  int t = barCount->text().toInt(&ok);
  if (ok)
    emit signalBarsChanged(t);
}

void ChartToolbar::ps1ButtonClicked ()
{
  RcFile rcfile;
  int ti;
  rcfile.loadData(RcFile::PSButton, ti, 1);
//  setPixelspace(minPixelspace, s.toInt());
//  emit signalBarLengthChanged(compressionCombo->currentItem());
  pixelspace->setValue(ti);
  //emit signalBarsChanged(0);
//  emit signalPaperTradeNextBar();
}

void ChartToolbar::ps2ButtonClicked ()
{
  RcFile rcfile;
  int ti;
  rcfile.loadData(RcFile::PSButton, ti, 2);
//  setPixelspace(minPixelspace, s.toInt());
//  emit signalBarLengthChanged(compressionCombo->currentItem());
  pixelspace->setValue(ti);
  //emit signalBarsChanged(0);
//  emit signalPaperTradeNextBar();
}

void ChartToolbar::ps3ButtonClicked ()
{
  RcFile rcfile;
  int ti;
  rcfile.loadData(RcFile::PSButton, ti, 3);
//  setPixelspace(minPixelspace, s.toInt());
//  emit signalBarLengthChanged(compressionCombo->currentItem());
  pixelspace->setValue(ti);
  //emit signalBarsChanged(0);
//  emit signalPaperTradeNextBar();
}

void ChartToolbar::cmpsBtnWClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)7);
  emit signalBarLengthChanged((int)7);
}

void ChartToolbar::cmpsBtnDClicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)6);
  emit signalBarLengthChanged((int)6);
}

void ChartToolbar::cmpsBtn15Clicked()
{
  compressionCombo->setCurrentItem((BarData::BarLength)3);
  emit signalBarLengthChanged((int)3);
}

void ChartToolbar::slotOrientationChanged(Orientation o)
{
    slider->setOrientation(o);
   // updateGeometry();

 // if(o)
 // {
    
 // }
}

//*********************************************************************
//******************** paper trade stuff ******************************
//*********************************************************************

void ChartToolbar::getPaperTradeDate (QDateTime &d)
{
  d = ptDate;
}

void ChartToolbar::paperTradeDate ()
{
  QString dl = tr("Last Date");
  QString tl = tr("Last Time");
  QString pl = "Date";

  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("Paper trade date select."));
  dialog->createPage (pl);
  dialog->addDateItem(dl, pl, ptDate);
  dialog->addTimeItem(tl, pl, ptDate);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QDateTime dt;
    dialog->getDate(dl, dt);
    ptDate.setDate(dt.date());

    dialog->getDate(tl, dt);
    ptDate.setTime(dt.time());

    emit signalPaperTradeNextBar();
  }

  delete dialog;
}

void ChartToolbar::paperTradeClicked (bool d)
{
  if (d)
  {
    ptdButton->show();
    ptnButton->show();
    barCount->setEnabled(FALSE);
  }
  else
  {
    ptdButton->hide();
    ptnButton->hide();
    barCount->setEnabled(TRUE);
  }
}

void ChartToolbar::paperTradeNextBar ()
{
  if (ptdButton->isHidden())
    return;

  switch (compressionCombo->currentItem())
  {
    case 0:
      ptDate = ptDate.addSecs(60);
      break;
    case 1:
      ptDate = ptDate.addSecs(300);
      break;
    case 2:
      ptDate = ptDate.addSecs(600);
      break;
    case 3:
      ptDate = ptDate.addSecs(900);
      break;
    case 4:
      ptDate = ptDate.addSecs(1800);
      break;
    case 5:
      ptDate = ptDate.addSecs(3600);
      break;
    case 6: // daily
      ptDate = ptDate.addDays(1);
      if (ptDate.date().dayOfWeek() == 6)
        ptDate = ptDate.addDays(2);
      else
      {
        if (ptDate.date().dayOfWeek() == 7)
          ptDate = ptDate.addDays(1);
      }
      break;
    case 7: // weekly
      ptDate = ptDate.addDays(7);
      break;
    case 8: // monthly
      ptDate = ptDate.addMonths(1);
      break;
    default:
      break;
  }
  
  emit signalPaperTradeNextBar();
}
