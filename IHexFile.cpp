#include "IHexFile.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

IHexFile::IHexFile(QObject *parent) :
	QObject(parent),
	segmentAddrRecord(false),
	linearAddrRecord(false),
	blocks()
{

}

int IHexFile::saveFileFromByteArray(QByteArray &data, QString &fileName,
	quint32 baseAddr, int type)
{
	QByteArray ba;
	QMap<quint32, QByteArray>::iterator it;

    QFile *outFile = new QFile(fileName);

    if (!outFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        delete outFile;
        return -1;
    }

    QTextStream out(outFile);
    out.setPadChar('0');
    out.setFieldWidth(2);
    out.setNumberFlags(QTextStream::UppercaseDigits);

/*
    if (data.size() > 0xFFFF) {
    	out << INH32M_HEADER;
    	out.setFieldWidth(0);
    	out << endl;
    	out.setFieldWidth(2);
    }
*/

	int len = data.size();
	quint32 address = baseAddr;
	quint32 linearAddress = 0;
	quint8 blockSize = 32;
	quint8 checksum = 0;
	int offset = 0;
	int blockLen = 0;

	while (len > 0) {
		if (address > 0xFFFF) {
			if (type == IHEXFILE_HEX386) {
				quint16 addr = (address >> 16);
				if (addr != linearAddress) {
					out << ":02000004";
					out.setFieldWidth(4);
					out << hex << addr;
					out.setFieldWidth(2);
					checksum = 0x06 + addr + (addr >> 8);
					checksum = 0x01 + ~checksum;
					out << hex << static_cast<quint16>(checksum);
					out.setFieldWidth(0);
					out << endl;
					out.setFieldWidth(2);
					linearAddress = addr;
				}
			} else { // HEX86
				quint16 addr = ((address >> 4) & 0xF000);
				if (addr != linearAddress) {
					out << ":02000002";
					out.setFieldWidth(4);
					out << hex << addr;
					out.setFieldWidth(2);
					checksum = 0x04 + addr + (addr >> 8);
					checksum = 0x01 + ~checksum;
					out << hex << static_cast<quint16>(checksum);
					out.setFieldWidth(0);
					out << endl;
					out.setFieldWidth(2);
					linearAddress = addr;
				}
			}
		}

		checksum = 0;
		blockLen = (len > blockSize)?blockSize:len;
		out.setFieldWidth(0);
		out << ":";
		out.setFieldWidth(2);
		out << hex << blockLen;	// length
		checksum += blockLen;
		out.setFieldWidth(4);
		out << hex << static_cast<quint16>(address); // address
		out.setFieldWidth(2);
		checksum += static_cast<quint8>(address);
		checksum += static_cast<quint8>(address >> 8);

		out << "00"; // record type

		for (int i = 0; i < blockLen; i++) {

			//out << uppercasedigits << hex << static_cast<quint8>(data.at(address+i));
			out << hex << static_cast<quint8>(data.at(offset+i));
			checksum += static_cast<quint8>(data.at(offset+i));

		}

		checksum = 0x01 + ~checksum;
		out << hex << static_cast<quint16>(checksum);
		out.setFieldWidth(0);
		out << endl;
		out.setFieldWidth(2);

		address += blockLen;
		offset += blockLen;
		len -= blockLen;
	}

	out << ":00000001FF\n";

	outFile->close();
	delete outFile;

	return 0;
}

// reference to https://github.com/codinghead/Intel-HEX-Class/blob/master/intelhex_class/intelhexclass.cpp
QByteArray IHexFile::decode(QString &hexFileName)
{
    QFile *inFile = new QFile(hexFileName);
    QString line;
	QByteArray binArray = QByteArray();
	quint32 linearAddress;
	quint32 loadOffset = 0;
	quint8 blockSize;
	quint8 recordType;
	quint8 checksum = 0;
	int lineCounter = 0;
	int blockLen = 0;
	
    if (!inFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        delete inFile;
        return binArray;
    }

	while (!inFile->atEnd()) {
		line = QString(inFile->readLine().simplified());
		checksum = 0;
		lineCounter++;
		
		if (!line.isEmpty()) {
			if (!line.startsWith(':')) {
				if (lineCounter == 1) {
					// it's not a intel hex file
					break;
				} else {
					// just dump warning and continue
					continue;
				}
			}

			line.remove(0, 1);
			QByteArray ba = QByteArray::fromHex(line.toLatin1());
			for (int i = 0; i < ba.count(); i++) {
				checksum += static_cast<quint8>(ba.at(i));
			}

			if (checksum != 0) {
				qWarning() << QObject::tr("checksum is error at line %1").arg(lineCounter);
				continue;
			}

			blockSize = ba.at(0);
			loadOffset = (static_cast<quint8>(ba.at(1)) << 8) + static_cast<quint8>(ba.at(2));
			recordType = ba.at(3);

			switch (recordType) {
				case 0: // data record
				{
					int len = ba.count() - 4 - 1;
					QByteArray data = ba.mid(4, len);
					binArray.append(data);

					break;
				}
				default:
					break;
			}
		}
	}

	return binArray;
}
