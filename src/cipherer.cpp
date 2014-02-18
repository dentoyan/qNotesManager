/*
This file is part of qNotesManager.

qNotesManager is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

qNotesManager is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with qNotesManager. If not, see <http://www.gnu.org/licenses/>.
*/

#include "cipherer.h"

#include <QDebug>

using namespace qNotesManager;

Cipherer::Cipherer() :
		defaultCipherMode(QCA::Cipher::CBC),
		defaultCipherPadding(QCA::Cipher::PKCS7),
		DefaultHashID(0),
		DefaultSecureHashID(0)

{
	avaliableCipherTypes.insert(1, "aes128");
}

QByteArray Cipherer::Encrypt(const QByteArray& data, const QByteArray& keyData, int cipherID) {
	return process(data, keyData, QCA::Encode, cipherID);
}

QByteArray Cipherer::Decrypt(const QByteArray& data, const QByteArray& keyData, int cipherID) {
	return process(data, keyData, QCA::Decode, cipherID);
}

QByteArray Cipherer::process(const QByteArray& data, const QByteArray& keyData,
							QCA::Direction direction, int cipherID) {
	if (data.isEmpty()) {
		return QByteArray();
	}
	if (keyData.isEmpty()) {
		return QByteArray();
	}
	if (!avaliableCipherTypes.contains(cipherID)) {
		return QByteArray();
	}

	QCA::Initializer init;
	QCA::SecureArray arg = data;

	QString cipherType = avaliableCipherTypes.value(cipherID);
	QString fullCipherType = QString(cipherType + "-cbc-pkcs7");

	if (!QCA::isSupported(fullCipherType.toStdString().c_str())) {
		return QByteArray();
	}

	QCA::SymmetricKey key(keyData);

	QString initVectorData = "aes128-cbc-pkcs7-sn93-sh21-jks-12";

	QCA::InitializationVector initVector(initVectorData.toAscii());

	QCA::Cipher cipher(cipherType, defaultCipherMode,
					   defaultCipherPadding,
					   direction,
					   key, initVector);

	QByteArray resultArray;
	if (direction == QCA::Encode) {
		QCA::SecureArray u = cipher.update(arg);
		if (!cipher.ok()) {
			return QByteArray();
		}

		QCA::SecureArray f = cipher.final();
		if (!cipher.ok()) {
			return QByteArray();
		}

		u.append(f);
		resultArray = QByteArray(u.data(), u.size());
	} else {
		QCA::SecureArray f = cipher.process(data);
		if (!cipher.ok()) {
			return QByteArray();
		}
		resultArray = QByteArray(f.data(), f.size());
	}

	return resultArray;
}

QList<int> Cipherer::GetAvaliableCipherIDs() {
	return avaliableCipherTypes.keys();
}

bool Cipherer::IsCipherTypeSupported(QString type) {
	QCA::Initializer init;
	QString str(type + "-cbc-pkcs7");
	return QCA::isSupported(str.toStdString().c_str());
}

bool Cipherer::IsFeatureSupported(const char* feature) {
	QCA::Initializer init;
	return QCA::isSupported(feature);
}

QString Cipherer::GetCipherName(int cipherID) {
	if (!avaliableCipherTypes.contains(cipherID)) {
		return QString();
	}
	return avaliableCipherTypes.value(cipherID);
}

QByteArray Cipherer::GetHash(const QByteArray& str, quint8 hashID) {
	if (!IsHashSupported(hashID)) {
		return QByteArray();
	}

	const int iterations = 1000;
	const QString hashType = "sha256";

	QCA::Initializer init;
	if (!QCA::isSupported(hashType.toStdString().c_str())) {
		return QByteArray();
	}

	QCA::Hash shaHash(hashType);
	for (int i = 0; i < iterations; i++) {
		shaHash.update(str);
	}

	QByteArray finalHash = shaHash.final().toByteArray();

	return finalHash;
}

bool Cipherer::IsHashSupported(quint8 i) {
	return (i == 0);
}

QByteArray Cipherer::GetSecureHash(const QByteArray& data, quint8 hashID) {
	if (!IsSecureHashSupported(hashID)) {
		return QByteArray();
	}

	const int iterations = 1000;
	const QString hashType = "hmac(sha256)";

	QCA::Initializer init;
	if (!QCA::isSupported(hashType.toStdString().c_str())) {
		return QByteArray();
	}

	QCA::SymmetricKey key(data);
	QCA::MessageAuthenticationCode mac(hashType, key);

	for (int i = 0; i < iterations; i++) {
		mac.update(data);
	}

	QByteArray finalHash = mac.final().toByteArray();

	return finalHash;
}

bool Cipherer::IsSecureHashSupported(quint8 i) {
	return (i == 0); // FIXME
}

QString Cipherer::GetMD5Hash(const QByteArray& data) {
	QCA::Initializer init;
	if (!QCA::isSupported("md5")) {
		return QString();
	}

	QCA::Hash hash("md5");
	hash.update(data);
	QString result = QCA::arrayToHex(hash.final().toByteArray());

	return result;
}
