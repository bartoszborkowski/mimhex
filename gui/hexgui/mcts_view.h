#ifndef GUI_MCTS_TREE_H
#define GUI_MCTS_TREE_H

#include <QDialog>
#include <QList>
#include <QVariant>
#include <QTreeView>
#include <QAbstractItemModel>
//#include <QLabel>
//#include "manager.h"


class TreeItem
{
public:
    TreeItem(const QList<QVariant>& data, TreeItem *parent=0);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parent();

private:
    TreeItem *parentItem;
    QList<TreeItem*> childItems;
    QList<QVariant> itemData;
};

inline TreeItem *prepareData(const QStringList& lines)
{
    if (lines.empty()) {
        return new TreeItem(QList<QVariant>() << "n/a");
    }

    TreeItem *root = new TreeItem(QList<QVariant>() << "MCTS Tree");

    QList<TreeItem*> parents;
    QList<int> indent;

    indent << -1;
    parents << root;

    int num = 0;

    while (num < lines.count()) {
        int pos = 0;
        while (pos<lines[num].length()) {
            if (lines[num].mid(pos, 1) != " ")
                break;
            pos ++;
        }
        QString data = lines[num].mid(pos).trimmed();

        num++;
        if (data.isEmpty())
            continue;

        if (pos>indent.last()) {
            if (parents.last()->childCount() > 0) {
                parents << parents.last()->child(parents.last()->childCount()-1);
                indent << pos;
            }
        } else {
            while (pos<indent.last() && parents.count()>0) {
                parents.pop_back();
                indent.pop_back();
            }
        }
        parents.last()->appendChild(new TreeItem(QList<QVariant>() << data, parents.last()));
 
    }

    return root;
}

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(TreeItem *root, QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
private:
    TreeItem *rootItem;
};


class sTreeView : public QTreeView
{
public:
    sTreeView(const std::string& ascii_tree, QWidget *parent);
    ~sTreeView();

    void reloadModel(const std::string& ascii_tree);
private:
    void Load(const std::string& ascii_tree);
    TreeModel *model;
};

class MCTS_View: public QDialog
{
Q_OBJECT

public:
    MCTS_View(const std::string& ascii_tree, QWidget *parent = 0);
//    ~MCTS_View();
private:
    //TreeModel *model;
    sTreeView *treeView;
};

#endif // GUI_MCTS_TREE_H
