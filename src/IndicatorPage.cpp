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

#include "IndicatorPage.h"
#include "ok.xpm"
#include "disable.xpm"
#include <qtooltip.h>
#include <qlayout.h>

IndicatorPage::IndicatorPage (QWidget *w, Plot *pl) : QWidget (w)
{
  plot = pl;

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  list = new QListBox(this);
  connect(list, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  vbox->addWidget(list);

  enableMain = new QCheckBox(tr("Disable All"), this);
  QToolTip::add(enableMain, tr("Disable All Main Chart Indicators"));
  connect(enableMain, SIGNAL(clicked()), this, SLOT(mainToggled()));
  vbox->addWidget(enableMain);

  refreshList();
}

IndicatorPage::~IndicatorPage ()
{
}

void IndicatorPage::refreshList ()
{
  list->clear();

  QStringList l = plot->getIndicators();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Indicator *i = plot->getIndicator(l[loop]);

    if (i->getEnable())
      list->insertItem(ok, l[loop], -1);
    else
      list->insertItem(disable, l[loop], -1);
  }
}

void IndicatorPage::doubleClick (QListBoxItem *item)
{
  int index = list->index(item);

  Indicator *i = plot->getIndicator(item->text());
  if (i->getEnable())
  {
    list->changeItem(disable, item->text(), index);
    i->setEnable(FALSE);
  }
  else
  {
    i->setEnable(TRUE);
    list->changeItem(ok, item->text(), index);
  }

  plot->draw();
}

void IndicatorPage::mainToggled ()
{
  QStringList l = plot->getIndicators();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Indicator *i = plot->getIndicator(l[loop]);

    if (enableMain->isChecked() == TRUE)
    {
      list->changeItem(disable, list->text(loop), loop);
      i->setEnable(FALSE);
    }
    else
    {
      list->changeItem(ok, list->text(loop), loop);
      i->setEnable(TRUE);
    }
  }

  if (enableMain->isChecked() == TRUE)
    list->setEnabled(FALSE);
  else
    list->setEnabled(TRUE);

  plot->draw();
}




