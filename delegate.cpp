#include "delegate.h"
#include <QStandardItem>
#include <QStyledItemDelegate>
Delegate::Delegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QSize Delegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

}
