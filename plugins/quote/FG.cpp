#include "FG.h"
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qtimer.h>

FG::FG ()
{
  pluginName = "FG";
  version = 0.2;

  set(tr("Input"), "", Setting::FileList);

  about = "Imports ascii files that were downloaded\n";
  about.append("from www.futuresguide.com");
}

FG::~FG ()
{
}

void FG::download ()
{
  QTimer::singleShot(1000, this, SLOT(parse()));
}

void FG::parse ()
{
  QStringList list = getList(tr("Input"));

  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    QFile f(list[loop]);
    if (! f.open(IO_ReadOnly))
      continue;
    QTextStream stream(&f);

    while(stream.atEnd() == 0)
    {
      QString s = stream.readLine();
      s = stripJunk(s);

      QStringList l = QStringList::split(" ", s, FALSE);
      if (l.count() != 9)
        continue;

      if (l[7].toInt() == 0)
        continue;

      // symbol
      QString symbol = l[0];
      QString month;
      if (symbol.length() == 6)
      {
        if (symbol[0] == 'R')
          symbol.remove(0, 1);
        else
          symbol.remove(2, 1);
      }

      if (symbol.length() == 5)
      {
        s = symbol.left(2);
        month = symbol.mid(2, 1);
        symbol.remove(2, 1);
        symbol.insert(2, "20");
      }
      else
      {
        s = symbol.left(1);
        month = symbol.mid(1, 1);
        symbol.remove(1, 1);
        symbol.insert(1, "20");
      }

      if (setFutures(s))
        continue;

      if (! s.compare("CO"))
      {
        symbol.remove(1, 1);
        symbol.insert(1, "C");
      }

      if (! s.compare("OJ"))
      {
        symbol.remove(0, 1);
        symbol.insert(1, "O");
      }

      symbol.append(month);

      if (! s.compare("TY"))
      {
        l[2] = translateTY(l[2]);
        l[3] = translateTY(l[3]);
        l[4] = translateTY(l[4]);
        l[5] = translateTY(l[5]);
      }

      if (! s.compare("US"))
      {
        l[2] = translateUS(l[2]);
        l[3] = translateUS(l[3]);
        l[4] = translateUS(l[4]);
        l[5] = translateUS(l[5]);
      }

      // date
      QStringList l2 = QStringList::split("/", l[1], FALSE);
      if (l2.count() != 3)
        continue;
      QString date = l2[2];
      date.append(l2[0]);
      date.append(l2[1]);
      date.append("000000");

      // open
      QString open;
      if (setTFloat(l[2]))
        continue;
      else
        open = QString::number(tfloat);

      // high
      QString high;
      if (setTFloat(l[3]))
        continue;
      else
        high = QString::number(tfloat);

      // low
      QString low;
      if (setTFloat(l[4]))
        continue;
      else
        low = QString::number(tfloat);

      // close
      QString close;
      if (setTFloat(l[5]))
        continue;
      else
        close = QString::number(tfloat);

      // volume
      QString volume;
      if (setTFloat(l[7]))
        continue;
      else
        volume = QString::number(tfloat);

      // oi
      QString oi;
      if (setTFloat(l[8]))
        continue;
      else
        oi = QString::number(tfloat);

      // check for bad values
      if (close.toFloat() == 0)
        return;
      if (open.toFloat() == 0)
        open = close;
      if (high.toFloat() == 0)
        high = close;
      if (low.toFloat() == 0)
        low = close;

      Setting *r = new Setting;
      r->set("Date", date, Setting::Date);
      r->set("Open", open, Setting::Float);
      r->set("High", high, Setting::Float);
      r->set("Low", low, Setting::Float);
      r->set("Close", close, Setting::Float);
      r->set("Volume", volume, Setting::Float);
      r->set("Open Interest", oi, Setting::Float);

      s = dataPath;
      s.append("/");
      s.append(symbol);
      ChartDb *db = new ChartDb();
      db->setPath(s);
      db->openChart();

      s = db->getChartType();
      if (! s.length())
      {
        db->setChartType(tr("Futures"));
        db->setSymbol(symbol);
        db->setSource(pluginName);

        Setting *set = new Setting;
        set->set("Futures Month", month, Setting::None);
        set->set("Futures Type", futureSymbol, Setting::None);
        set->set("Title", futureName, Setting::Text);
        db->setDetails(set);
        delete set;
      }

      db->setRecord(r);
      delete db;

      if (! symbol.compare(cc))
      {
        symbol = "CC";
        symbol.append(futureSymbol);

        s = dataPath;
        s.append("/");
        s.append(symbol);
        ChartDb *db = new ChartDb();
        db->setPath(s);
        db->openChart();

        s = db->getChartType();
        if (! s.length())
        {
          db->setChartType(tr("CC Futures"));
          db->setSymbol(symbol);
          db->setSource(pluginName);

          Setting *set = new Setting;
          set->set("Futures Type", futureSymbol, Setting::None);
          s = QObject::tr("Continuous ");
          s.append(futureSymbol);
          set->set("Title", s, Setting::Text);
          db->setDetails(set);
          delete set;
        }

        db->setRecord(r);
        delete db;
      }

      delete r;
    }

    f.close();
  }

  emit done();
}

QString FG::translateUS (QString d)
{
  QString f = d.right(2);
  QString s = d;
  s.truncate(s.length() - 2);
  QString s2 = QString::number(f.toFloat() / 32);
  s2.remove(0, 1);
  s.append(s2);
  return s;
}

QString FG::translateTY (QString d)
{
  QString f = d.right(3);
  QString s = d;
  s.truncate(s.length() - 3);
  QString s2 = QString::number((f.toFloat() / 32) / 10);
  s2.remove(0, 1);
  s.append(s2);
  return s;
}

Plugin * create ()
{
  FG *o = new FG;
  return ((Plugin *) o);
}

