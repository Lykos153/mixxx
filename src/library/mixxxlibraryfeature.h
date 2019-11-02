// mixxxlibraryfeature.h
// Created 8/23/2009 by RJ Ryan (rryan@mit.edu)

#ifndef MIXXXLIBRARYFEATURE_H
#define MIXXXLIBRARYFEATURE_H

#include <QStringListModel>
#include <QUrl>
#include <QVariant>
#include <QIcon>
#include <QModelIndex>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QObject>

#include "library/libraryfeature.h"
#include "library/dao/trackdao.h"
#include "library/treeitemmodel.h"
#include "preferences/usersettings.h"

class DlgHidden;
class DlgMissing;
class Library;
class BaseTrackCache;
class LibraryTableModel;
class TrackCollection;

class MixxxLibraryFeature : public LibraryFeature {
    Q_OBJECT
    public:
    MixxxLibraryFeature(Library* pLibrary,
                        TrackCollection* pTrackCollection,
                        UserSettingsPointer pConfig);
    virtual ~MixxxLibraryFeature();

    QVariant title() override;
    QIcon getIcon() override;
    bool dropAccept(QList<QUrl> urls, QObject* pSource) override;
    bool dragMoveAccept(QUrl url) override;
    TreeItemModel* getChildModel() override;
    void bindLibraryWidget(WLibrary* pLibrary,
                    KeyboardEventFilter* pKeyboard) override;

    bool hasTrackTable() override {
        return true;
    }

  public slots:
    void activate() override;
    void activateChild(const QModelIndex& index) override;
    void refreshLibraryModels();

  private:
    const QString kMissingTitle;
    const QString kHiddenTitle;
    Library* m_pLibrary;
    QSharedPointer<BaseTrackCache> m_pBaseTrackCache;
    LibraryTableModel* m_pLibraryTableModel;
    DlgMissing* m_pMissingView;
    DlgHidden* m_pHiddenView;
    TreeItemModel m_childModel;
    UserSettingsPointer m_pConfig;
    TrackCollection* m_pTrackCollection;
    QIcon m_icon;
};

#endif /* MIXXXLIBRARYFEATURE_H */
