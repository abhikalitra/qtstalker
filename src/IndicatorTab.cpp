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

#include "IndicatorTab.h"
#include <qstringlist.h>


IndicatorTab::IndicatorTab (QWidget *w) : QWidget (w)
{
  vbox = new QVBoxLayout(this);
  vbox->setMargin(0);
  vbox->setSpacing(0);
  
  stack = new QWidgetStack(this);
  vbox->addWidget(stack);

  tab = new QTabBar(this);
  connect(tab, SIGNAL(selected(int)), this, SLOT(slotTabChanged(int)));
  tab->setShape(QTabBar::RoundedBelow);
  vbox->addWidget(tab);
}

IndicatorTab::~IndicatorTab ()
{
}

void IndicatorTab::slotTabChanged (int id) 
{
  stack->raiseWidget(id);
  Plot *p = (Plot *) stack->visibleWidget();
  if (! p)
    return;

  p->draw();
}

void IndicatorTab::drawCurrent () 
{
  Plot *p = (Plot *) stack->visibleWidget();
  if (! p)
    return;

  p->draw();
}

void IndicatorTab::getTabList (QStringList &l) 
{
  l.clear();
  
  int loop;
  for (loop = 0; loop < (int) tab->count(); loop++)
  {
    QTab *t = tab->tabAt(loop);
    l.append(t->text());
  }
}

int IndicatorTab::getInsertIndex (QString &d) 
{
  QStringList l;
  getTabList(l);
  l.append(d);
  l.sort();
  return l.findIndex(d);
}

bool IndicatorTab::deleteTab (QString &d) 
{
  QString s = idList.getData(d);
  if (! s.length())
    return FALSE;
    
  QTab *t = tab->tab(s.toInt());
  tab->removeTab(t);
  
  stack->removeWidget(stack->widget(s.toInt()));
  
  idList.remove(d);
  
  slotTabChanged(tab->currentTab());
        
  return TRUE;
}

void IndicatorTab::insertTab (QWidget *w, QString n, int p)
{
  QTab *t = new QTab(n);
  int id = tab->insertTab(t, p);
  
  idList.setData(n, QString::number(id));
  
  stack->addWidget(w, id);

  tab->update();
  
  tab->setCurrentTab(id);
}



