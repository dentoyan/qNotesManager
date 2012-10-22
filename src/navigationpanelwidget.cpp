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

#include "navigationpanelwidget.h"

#include "foldernavigationwidget.h"
#include "tagsnavigationwidget.h"
#include "datenavigationwidget.h"
#include "document.h"

#include <QVBoxLayout>

using namespace qNotesManager;

NavigationPanelWidget::NavigationPanelWidget(QWidget *parent) : QWidget(parent) {
	tabWidget = new QTabWidget();
	QObject::connect(tabWidget, SIGNAL(currentChanged(int)),
					 this, SLOT(sl_TabWidget_CurrentChanged(int)));

	hierarchyWidget = new FolderNavigationWidget();
	QObject::connect(hierarchyWidget, SIGNAL(sg_NoteClicked(Note*)),
					 this, SIGNAL(sg_NoteClicked(Note*)));
	QObject::connect(hierarchyWidget, SIGNAL(sg_NoteDoubleClicked(Note*)),
					 this, SIGNAL(sg_NoteDoubleClicked(Note*)));
	QObject::connect(hierarchyWidget, SIGNAL(sg_SelectedItemsActionsListChanged()),
					 this, SIGNAL(sg_SelectedItemsActionsListChanged()));
	tabWidget->addTab(hierarchyWidget, QIcon(QPixmap(":/gui/folder-tree")), "Folders");

	tagsWidget = new TagsNavigationWidget();
	QObject::connect(tagsWidget, SIGNAL(sg_NoteClicked(Note*)),
					 this, SIGNAL(sg_NoteClicked(Note*)));
	QObject::connect(tagsWidget, SIGNAL(sg_NoteDoubleClicked(Note*)),
					 this, SIGNAL(sg_NoteDoubleClicked(Note*)));
	tabWidget->addTab(tagsWidget, QIcon(":/gui/tag"), "Tags");

	datesWidget = new DateNavigationWidget();
	QObject::connect(datesWidget, SIGNAL(sg_NoteClicked(Note*)),
					 this, SIGNAL(sg_NoteClicked(Note*)));
	QObject::connect(datesWidget, SIGNAL(sg_NoteDoubleClicked(Note*)),
					 this, SIGNAL(sg_NoteDoubleClicked(Note*)));
	tabWidget->addTab(datesWidget, QIcon(":/gui/date"), "Dates");

	QVBoxLayout* layout = new QVBoxLayout();
#if QT_VERSION < 0x040300
	layout->setMargin(0);
#else
	layout->setContentsMargins(0, 0, 0, 0);
#endif
	layout->addWidget(tabWidget);
	setLayout(layout);
}

int NavigationPanelWidget::CurrentTabIndex() const {
	return tabWidget->currentIndex();
}

void NavigationPanelWidget::SetCurrentTab(int index) {
	if (index < 0 || index > tabWidget->count()) {return;}

	tabWidget->setCurrentIndex(index);
}

void NavigationPanelWidget::sl_NoteActivated(Note* note) {
	if (tabWidget->currentWidget() == hierarchyWidget) {
		hierarchyWidget->SetCurrentItem(note);
	}
}

void NavigationPanelWidget::sl_TabWidget_CurrentChanged(int) {
	emit sg_SelectedItemsActionsListChanged();
}

void NavigationPanelWidget::SetTargetDocument(Document* document) {
	if (document) {
		hierarchyWidget->SetModel(document->GetHierarchyModel());
		datesWidget->SetCreationModel(document->GetCreationDatesModel());
		datesWidget->SetModificationModel(document->GetModificationDatesModel());
		datesWidget->SetTextDateModel(document->GetTextDatesModel());
		tagsWidget->SetModel(document->GetTagsModel());
	} else {
		hierarchyWidget->SetModel(0);
		datesWidget->SetCreationModel(0);
		datesWidget->SetModificationModel(0);
		datesWidget->SetTextDateModel(0);
		tagsWidget->SetModel(0);
	}
}

QList<QAction*> NavigationPanelWidget::GetSelectedItemsActions() const {
	if (tabWidget->currentWidget() == hierarchyWidget) {
		return  hierarchyWidget->GetSelectedItemsActions();
	}

	return QList<QAction*>();
}
