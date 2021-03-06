#ifndef GAMESCAN_H
#define GAMESCAN_H

#include <set>
#include <map>

#include <QObject> // for moc
#include <QStringList>
#include <QMap>

#include "mthread.h"

class QStringList;

class MythUIProgressDialog;
class GameHandler;
class RomInfo;

struct RomFileInfo
{
    QString system;
    QString gametype;
    QString romfile;
    QString rompath;
    QString romname;
    bool indb;
};

using RomFileInfoList = QList< RomFileInfo >;

class GameScannerThread : public MThread
{
  public:
    explicit GameScannerThread(void);
    ~GameScannerThread() override = default;

    void run(void) override; // MThread

    void SetHandlers(QList<GameHandler*> handlers) { m_handlers = handlers; };
    void SetProgressDialog(MythUIProgressDialog *dialog) { m_dialog = dialog; };

    bool getDataChanged() { return m_dbDataChanged; };

  private:

    static void removeOrphan(const int id);

    void verifyFiles();
    void updateDB();

    bool buildFileList();

    void SendProgressEvent(uint progress, uint total = 0,
                           QString message = QString());

    bool m_hasGUI;

    QList<GameHandler*> m_handlers;

    RomFileInfoList m_files;
    QList<uint> m_remove;
    QList<RomInfo*> m_dbgames;

    MythUIProgressDialog *m_dialog        {nullptr};

    bool                  m_dbDataChanged {false};
};

class GameScanner : public QObject
{
    Q_OBJECT

  public:
    GameScanner();
    ~GameScanner() override;

    void doScan(QList<GameHandler*> handlers);
    void doScanAll(void);

  signals:
    void finished(bool);

  public slots:
    void finishedScan();

  private:
    GameScannerThread  *m_scanThread;
};

#endif
