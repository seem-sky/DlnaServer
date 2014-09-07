#include "requestlistmodel.h"

RequestListModel::RequestListModel(QObject *parent) :
    QAbstractTableModel(parent),
    mRecords(),
    mRoles()
{
    mRoles[methodRole] = "method";
    mRoles[argumentRole] = "argument";
    mRoles[hostRole] = "host";
    mRoles[peerAddressRole] = "peerAddress";
    mRoles[statusRole] = "status";
    mRoles[headerRole] = "header";
    mRoles[contentRole] = "content";
    mRoles[durationRole] = "duration";
    mRoles[dateRole] = "date";
    mRoles[answerRole] = "answer";
    mRoles[networkStatusRole] = "network_status";
    mRoles[transcodeLogRole] = "transcode_log";

    connect(this, SIGNAL(newRequest(Request*)), this, SLOT(addRequestInModel(Request*)));
}

RequestListModel::~RequestListModel() {
    // clear the model
    clearAll();
}

void RequestListModel::clearAll() {
    qWarning() << "Remove all requests, size =" << mRecords.size();
    // remove all requests
    qDeleteAll(mRecords);
    mRecords.clear();
}

int RequestListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return mRecords.count();
}

int RequestListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return mRoles.keys().length();
}

QVariant RequestListModel::data(const QModelIndex &index, int role) const
{
    Request *item = 0;

    if ( index.row() >= 0 and index.row() < mRecords.size() )
        item = mRecords.at(index.row());

    if (item != 0) {
        switch (role) {
        case methodRole:
            return item->getMethod();
        case argumentRole:
            return item->getArgument();
        case hostRole:
            return QString("%1 (%2)").arg(item->getHost()).arg(item->socketDescriptor());
        case peerAddressRole:
            return item->getpeerAddress();
        case statusRole:
            return item->getStatus();
        case headerRole:
            return item->getTextHeader();
        case contentRole:
            return item->getTextContent();
        case durationRole:
            return item->getDuration();
        case dateRole:
            return item->getDate();
        case answerRole:
            return item->getTextAnswer();
        case networkStatusRole:
            return item->getNetworkStatus();
        case transcodeLogRole:
            return item->getLog();
        default:
            return QVariant::Invalid;

        }
    }

    return QVariant::Invalid;
}

void RequestListModel::createRequest(Logger *log, QTcpSocket *client, const QString &uuid, const QString &servername, const QString &host, const int &port)
{
    Request* request = 0;

    if (client)
    {
        request = new Request(log,
                              client,
                              uuid, servername,
                              host, port);

        if (request)
            emit newRequest(request);
        else
            log->Error("Unable to create new request");
    }
    else
    {
        log->Error("Unable to create request (client deleted).");
    }
}

void RequestListModel::addRequestInModel(Request *request)
{
    beginInsertRows(QModelIndex(), 0, 0);
    mRecords.prepend(request);
    endInsertRows();

    connect(request, SIGNAL(dataChanged(QString)), this, SLOT(requestChanged(QString)));
}

void RequestListModel::requestChanged(const QString &roleChanged) {
    int roleChangedIndex = mRoles.key(QByteArray(roleChanged.toUtf8().constData()));
    if (mRoles.contains(roleChangedIndex)) {
        int columnIndex = roleChangedIndex-Qt::UserRole-1;
        int requestIndex = mRecords.indexOf(static_cast<Request*>(sender()));
        if (requestIndex!=-1) {
            QVector<int> rolesChanged;
            rolesChanged.append(roleChangedIndex);
            emit dataChanged(index(requestIndex, columnIndex), index(requestIndex, columnIndex), rolesChanged);
        } else {
            qWarning() << "requestChanged ERROR, sender" << sender() << "is unknown";
        }
    } else {
        qWarning() << "requestChanged ERROR, role" << roleChanged << "is unknown";
    }
}

QVariant RequestListModel::get(const int &index, const int &roleIndex) const
{
    return data(this->index(index, 0), Qt::UserRole+1+roleIndex);
}
