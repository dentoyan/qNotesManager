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

#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include <QTextDocument>
#include <QUrl>
#include <QImage>
#include <QVariant>
#include <QMap>
#include <QQueue>
#include <QPixmap>
#include <QTimer>

namespace qNotesManager {
	class ImageLoader;
	class CachedImageFile;
	class IDummyImagesProvider;

	class TextDocument : public QTextDocument {
	Q_OBJECT
	private:
		ImageLoader* loader;


		QList<QUrl> activeDownloads;
		QList<QUrl> errorDownloads;
		void replaceImageUrl(const QUrl& oldName, const QString& newName);
		QSize findImageSize(const QString& resourceID);

		QTimer restartDownloadsTimer;

		QHash<QString, CachedImageFile*> originalImages;

	public:
		explicit TextDocument(QObject *parent = 0);
		~TextDocument();

		QStringList GetImagesList() const;

		CachedImageFile* GetResourceImage(QString name) const;
		void AddResourceImage(CachedImageFile*);
		QStringList GetResourceImagesList() const;

		IDummyImagesProvider* DummyImagesProvider;

	protected:
		//virtual
		QVariant loadResource (int type, const QUrl& name);

	signals:
		void sg_NeedRelayout();

	private slots:
		void sl_Downloader_DownloadFinished (QUrl url, CachedImageFile* image);
		void sl_Downloader_DownloadError (QUrl url, QString errorDescription);
		void sl_Downloader_Progress(QUrl url, int percent);

		void sl_RestartDownloadsTimer_Timeout();

	};
}

#endif // TEXTDOCUMENT_H
