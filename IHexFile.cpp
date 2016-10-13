#include "IHexFile.h"

#include <QFile>
#include <QTextStream>

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