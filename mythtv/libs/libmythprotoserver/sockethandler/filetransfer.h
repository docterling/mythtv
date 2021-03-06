#ifndef FILETRANSFER_H_
#define FILETRANSFER_H_

// C++ headers
#include <cstdint>

using namespace std;

#include <QMutex>
#include <QString>
#include <QWaitCondition>

#include <vector>

#include "mythsocket.h"
#include "sockethandler.h"

class ProgramInfo;
class RingBuffer;

class FileTransfer : public SocketHandler
{
  public:
    FileTransfer(QString &filename, MythSocket *remote,
                 MythSocketManager *parent,
                 bool usereadahead, int timeout_ms);
    FileTransfer(QString &filename, MythSocket *remote,
                 MythSocketManager *parent, bool write);

    bool isOpen(void);
    bool ReOpen(QString newFilename = "");

    void Stop(void);

    void Pause(void);
    void Unpause(void);
    int RequestBlock(int size);
    int WriteBlock(int size);

    long long Seek(long long curpos, long long pos, int whence);

    uint64_t GetFileSize(void);
    QString GetFileName(void);

    void SetTimeout(bool fast);

  private:
   ~FileTransfer() override;

    volatile bool  m_readthreadlive {true};
    bool           m_readsLocked {false};
    QWaitCondition m_readsUnlockedCond;

    ProgramInfo *m_pginfo {nullptr};
    RingBuffer  *m_rbuffer {nullptr};
    bool m_ateof {false};

    vector<char> m_requestBuffer;

    QMutex m_lock {QMutex::NonRecursive};

    bool m_writemode {false};
};

#endif
