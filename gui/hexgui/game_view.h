#ifndef GUI_GAME_TREE_H
#define GUI_GAME_TREE_H


#include <QtGui>
#include <QDialog>
#include <QTreeView>
#include <QAbstractItemModel>
#include <iostream>
#include "board_inc.h"
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <boost/shared_ptr.hpp>

using namespace Hex;

struct GameNode;
typedef boost::shared_ptr<GameNode> GameNodePtr;


struct GameNode
{
    GameNode(const Move&, GameNode *p = 0 /*parent*/);
    GameNode(); //root

    boost::shared_ptr<Move> GetMove() const;
    GameNodePtr AppendMove(const Move&);

    int ChildrenCount() const;
    GameNode *GetParent() const;
    int GetRow() const;
    int GetMyRow(GameNodePtr) const;
    GameNode *GetChild(int row) const;


    void Print(std::ostream& os, int level) const;

    void Collapse();

    struct NodeCompare
    {
        inline bool operator() (const GameNodePtr& l, const GameNodePtr& r) const
        {
            //no need to check if s_ptr<Move> is valid
            // used in children set, so every child has his parent
            if (l->GetMove()->GetPlayer() == r->GetMove()->GetPlayer())
            {
                uint lx = l->GetMove()->GetLocation().GetX();
                uint ly = l->GetMove()->GetLocation().GetY();
                uint rx = r->GetMove()->GetLocation().GetX();
                uint ry = r->GetMove()->GetLocation().GetY();
                return (lx==ly) ? (rx<ry) : (lx<ly);
            }
            return l->GetMove()->GetPlayer() == Player::First();
                    //then we know right->Player == Second();
        }
    };
    typedef std::set<GameNodePtr, NodeCompare> Children;
private:
    boost::shared_ptr<Move> move;
    Children children;
    GameNode *parent;
};


struct GameTree
{
    GameTree();
    ~GameTree();

    void Reset();
    void AppendMove(const Move&);
    boost::shared_ptr<Move> GetLastMove() const;

    GameNodePtr GetRoot() const;

    std::string Print() const;
private:
    GameNodePtr current;
    GameNodePtr root;
};



class GameTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    GameTreeModel(GameNodePtr root, QObject *parent = 0);
    ~GameTreeModel();

    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
private:
    GameNode *rootItem;
};


class gTreeView : public QTreeView
{
    Q_OBJECT

public:
    gTreeView(QWidget* owner):
        QTreeView(owner) {}

protected slots:
    virtual void selectionChanged(const QModelIndex & index);

};

class GAME_View : public QDialog
{
    Q_OBJECT

public:
    //GAME_View(const std::string& ascii_tree, QWidget *parent = 0);
    GAME_View(GameTree& t, QWidget *parent = 0);
    ~GAME_View();

public slots:
    void updateTree();

signals:
    void moveChanged();

private:
//    sTreeView *treeView;
    QTreeView *treeView;
    GameTree& tree_;
    GameTreeModel *model;
};

#endif /* GUI_GAME_TREE_H */
