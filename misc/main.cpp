#include <QString>

#include "CSV.h"


int main (int argc, char **argv)
{
  QString s;
  CSV csv;
  int loop;
  for (loop = 1; loop < argc; loop = loop + 2)
  {
    QByteArray ba = argv[loop];
    if (ba.length() < 2)
    {
      qDebug() << "Invalid switch" << ba;
      return 1;
    }

    switch (ba[1])
    {
      case 'i':
        s = argv[loop + 1];
	csv.setInputFile(s);
        break;
      case 'd':
        s = argv[loop + 1];
	csv.setDateFormat(s);
        break;
      case 't':
        s = argv[loop + 1];
	csv.setTimeFormat(s);
        break;
      case 'f':
        s = argv[loop + 1];
	csv.setFormat(s);
        break;
      case 's':
        s = argv[loop + 1];
	csv.setDelimiter(s);
        break;
      case 'S':
        s = argv[loop + 1];
	csv.setSymbol(s);
        break;
      case 'D':
        s = argv[loop + 1];
	csv.setDate(s);
        break;
      case 'T':
        s = argv[loop + 1];
	csv.setTime(s);
        break;
      case 'O':
        s = argv[loop + 1];
	csv.setOpen(s);
        break;
      case 'H':
        s = argv[loop + 1];
	csv.setHigh(s);
        break;
      case 'L':
        s = argv[loop + 1];
	csv.setLow(s);
        break;
      case 'C':
        s = argv[loop + 1];
	csv.setClose(s);
        break;
      case 'V':
        s = argv[loop + 1];
	csv.setVolume(s);
        break;
      case 'I':
        s = argv[loop + 1];
	csv.setOI(s);
        break;
      default:
        break;
    }
  }

  csv.import();

  return 0;
}


