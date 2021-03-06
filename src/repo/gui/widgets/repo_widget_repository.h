/**
 *  Copyright (C) 2014 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

//------------------------------------------------------------------------------
// Qt
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QCompleter>
#include <QSettings>

//------------------------------------------------------------------------------
// Repo Core
#include <repo/repo_controller.h>

//------------------------------------------------------------------------------
// Repo GUI
#include "ui_repo_widget_repository.h"
#include "../primitives/repo_fontawesome.h"
#include "../primitives/repo_idbcache.h"
#include "../primitives/repo_sort_filter_proxy_model.h"
#include "repo_line_edit.h"

namespace Ui {
    class RepositoryWidget;
}

namespace repo {
namespace gui {
namespace widget{

	class RepositoryWidget : public QWidget, public repo::gui::primitive::RepoIDBCache
	{
		Q_OBJECT

			//! Databases header positions
		enum RepoDatabasesColumns { NAME = 0, COUNT = 1, SIZE = 2, ALLOCATED = 3 };

		//! Collection header positions
		enum RepoCollectionColumns { DOCUMENT = 0, VALUE = 1, TYPE = 2 };

		static const QString DATABASES_COLUMNS_SETTINGS;

	public:

		//! Constructor
		explicit RepositoryWidget(QWidget* parent = 0);

		//! Desctructor, removes the custom title bar widget.
		~RepositoryWidget();

	signals:

		/*! Signal emitted to cancel all running threads.
			*	Use waitForDone() to make sure all have finished.
			*/
		void cancel();




		//--------------------------------------------------------------------------
		//
		// RepoIDBCache
		//
		//--------------------------------------------------------------------------

		public slots :

		virtual QList<QString> getCollections(const QString &host, const QString &database) const
		{
			return QList<QString>();
		}

		////! Returns connection corresponding to given host.
		virtual repo::RepoController::RepoToken* getConnection(const QString &host) const;

		//! Returns a list of available hosts.
		virtual QList<QString> getHosts() const;

		//! Returns a list of available projects in a given database (a subset of all collections).
		virtual QList<QString> getProjects(const QString &host, const QString &database) const;

		/*!
			* Returns a copy of a selected connection. It is necessary to reconnect
			* and reauthenticate.
			*/
		repo::RepoController::RepoToken* getSelectedConnection() const { return token; }

		//! Returns a list of available databases.
		QList<QString> getDatabases(const QString& host) const;

		//! Returns selected host, empty string if none selected.
		QString getSelectedHost() const;

		//! Returns selected database, empty string if none selected.
		QString getSelectedDatabase() const;

		//--------------------------------------------------------------------------

		public slots :

		//! Disconnects database connection, if any.
		bool disconnectDB();

		//! Refreshes all connected databases. Only one at the moment.
		void refresh();

		//! Removes all threads from the thread pool and returns true if successful.
		bool cancelAllThreads();

		//! Fetches databases from the server.
		void fetchDatabases(repo::RepoController *controller, repo::RepoController::RepoToken * token);

		//! Fetches currently selected collection (if any) from the server.
		void fetchCollection();

		////! Fetches collection.
		void fetchCollection(
			const QString& /* database */,
			const QString& /* collection */);

		void addHost(QString name);

		void addDatabase(QString name);

		void addCollection(const repo::core::model::CollectionStats &stats);

		void addKeyValuePair(
			QVariant /* key */,
			QVariant /* value */,
			QVariant /* type */,
			unsigned int depth = 0);

		//--------------------------------------------------------------------------
		//
		// Data management
		//
		//--------------------------------------------------------------------------

		//! Removes all items from the databases model.
		void clearDatabaseModel();

		//! Removes all items from the collection model.
		void clearCollectionModel();

		//! Changes the tab based on an index value.
		void changeTab(int index);

		//! Copies selected collection cell to clipboard.
		void copySelectedCollectionCellToClipboard();

		//! Expands all collection records.
		inline void expandAllCollectionRecords() { ui->collectionTreeView->expandAll(); }

		//! Increments the current database row.
		inline void incrementDatabaseRow() { databaseRowCounter++; }

	public:

		//--------------------------------------------------------------------------
		//
		// Getters
		//
		//--------------------------------------------------------------------------

		//! Returns selected collection, empty string if none selected.
		QString getSelectedCollection() const;

		//! Returns selected project, empty string if none selected.
		QString getSelectedProject() const;

		//! Returns the databases tree view.
		QWidget *getDatabasesTreeView() const { return ui->databasesTreeView; }

		//! Returns the collection tree view.
		QWidget *getCollectionTreeView() const { return ui->collectionTreeView; }

		const QPoint &mapToGlobalDatabasesTreeView(const QPoint &pos)
		{
			return ui->databasesTreeView->viewport()->mapToGlobal(pos);
		}

		const QPoint &mapToGlobalCollectionTreeView(const QPoint &pos)
		{
			return ui->collectionTreeView->viewport()->mapToGlobal(pos);
		}

	private:

		//! Returns a selected databases model corresponding to the NAME column.
		QModelIndex getSelectedDatabasesTreeViewIndex() const;

		QModelIndex getHostModelIndex(const QString& host) const;

		//--------------------------------------------------------------------------
		//
		// Static helpers
		//
		//--------------------------------------------------------------------------

		/*! Returns a hierarchy depth for given model index.
			Root is depth 0, top level items are 1, their children are 2, etc.
			*/
		static unsigned int getHierarchyDepth(const QModelIndex&);

		static QStandardItem* getHierarchyDepth(
			const QStandardItemModel* /* model */,
			unsigned int /* depth */);

		/*! Sets the appropriate flags for case insensitive filtering for all
			* columns of a model.
			*/
		static void enableFiltering(
			QAbstractItemView*,
			QStandardItemModel*,
			QSortFilterProxyModel*,
			QLineEdit*);

		//--------------------------------------------------------------------------
		//! Returns a non-editable item with set properties as given.
		static QStandardItem* createItem(const QString&, const QVariant&, Qt::Alignment = Qt::AlignLeft, bool enabled = true);

		static QStandardItem* createItem(const QVariant&, Qt::Alignment = Qt::AlignLeft);

		static void setItem(QStandardItem*, const QString&, const QVariant&);

		static void setItemSize(QStandardItem*, uint64_t);

		static void setItemCount(QStandardItem*, uint64_t);

		/*! Returns icon if collection contains recognized string such as "scene" or "history",
			*	empty icon otherwise.
			*/
		QIcon getIcon(const QString& collection) const;

		//! Returns a human readable string of kilobytes, megabytes etc.
		static QString toFileSize(uint64_t bytes);

		//! Returns the current locale string representation.
		template <class T>
		static QString toLocaleString(const T & value)
		{
			QLocale locale;
			return locale.toString(value);
		}

		//--------------------------------------------------------------------------
		//
		// Private variables
		//
		//--------------------------------------------------------------------------

	private:

		//! Access to ui elements
		Ui::RepositoryWidget *ui;

		//! Default model for the databases.
		QStandardItemModel *databasesModel;

		//! Sorting model proxy for the databases.
		QSortFilterProxyModel *databasesProxyModel;

		//! Default model for the collection.
		QStandardItemModel *collectionModel;

		//! Sorting model proxy for the collection.
		QSortFilterProxyModel *collectionProxyModel;

		//! Private thread pool local to this object only.
		QThreadPool threadPool;

		//! Database controller
		repo::RepoController *controller;

		//! Connection token
		repo::RepoController::RepoToken *token;

		//! Counter of database rows
		int databaseRowCounter;
	};
}
} // end namespace gui
} // end namespace repo
