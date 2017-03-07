#ifndef ITEMMODEL_H
#define ITEMMODEL_H

#include <QStandardItemModel>

class ItemModel : public QStandardItemModel
{
    Q_OBJECT
public:
    ItemModel(int rows, int columns, QObject *parent = 0)
            :
                QStandardItemModel(rows, columns, parent)
        {}

    QVariant data(const QModelIndex &index, int role) const
    {
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignBottom; // <- Make alignment look different, i.e.
                                    // <- text at the bottom.
        } else {
            return QStandardItemModel::data(index, role);
        }
    }
signals:

public slots:

};

#endif // ITEMMODEL_H
