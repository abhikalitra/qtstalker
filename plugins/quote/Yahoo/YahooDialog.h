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

#include <qlistbox.h>
#include <qdatetimeedit.h>
#include <qcheckbox.h>
#include <qtabdialog.h>
#include <qdatetime.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include "Toolbar.h"

class YahooDialog : public QTabDialog
{
  Q_OBJECT
  
  public:
    YahooDialog (QWidget *, QString);
    ~YahooDialog ();
    QStringList getList ();
    void updateList ();
    void setAdjustment (bool);
    bool getAdjustment ();
    void setStartDate (QDateTime);
    QDateTime getStartDate ();
    void setEndDate (QDateTime);
    QDateTime getEndDate ();
    void setMethod (QString);
    QString getMethod ();
    void setRetries (int);
    int getRetries ();
    void setTimeout (int);
    int getTimeout ();
    
  public slots:
    void newStock ();
    void selectAll ();
    void unselectAll ();
    void methodChanged (int);
    void help ();
          
  private:
    QListBox *list;
    QCheckBox *adjustment;
    QDateEdit *date;
    QDateEdit *date2;
    QComboBox *method;
    QString dataPath;
    QString helpFile;
    Toolbar *toolbar;
    QSpinBox *retries;
    QSpinBox *timeout;
};

    

