#ifndef WORLD_H
#define WORLD_H

#include <QObject>

class World : public QObject
{
    Q_OBJECT
public:
    explicit World(QObject *parent = 0);
    ~World();

signals:

public slots:
};

#endif // WORLD_H
