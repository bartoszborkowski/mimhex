#include <QtGui>
#include <QPushButton>
#include "game_view.h"


//section: GameTree

GameNode::GameNode(const Move& m, GameNode *p)
    : move(boost::shared_ptr<Move>(new Move(m))),
      parent(p)
{
}
GameNode::GameNode()
{
}

void GameNode::Collapse() {
    children.clear();
}

GameNodePtr GameNode::AppendMove(const Move& m) {
    GameNodePtr node(new GameNode(m, this));

    GameNode::Children::iterator it = children.find(node);
    if (it!=children.end())
        return *it;

    children.insert(node);
    return node;
}

int GameNode::ChildrenCount() const {
    return children.size();
}

GameNode *GameNode::GetParent() const {
    return parent;
}

int GameNode::GetRow() const {
    if (!parent)
        return 0;
    GameNodePtr node(new GameNode(*move));
    return parent->GetMyRow(node);
}

int GameNode::GetMyRow(GameNodePtr node) const {
    int num = 0;
    for (Children::const_iterator it=children.begin()
            ; it!=children.end()
            ; ++it) {
        if ( !GameNode::NodeCompare()(*it,node)
          && !GameNode::NodeCompare()(node,*it))
            return num;
        num ++;
    }
    return 0;
}

GameNode *GameNode::GetChild(int row) const {
    int num = 0;
    for (Children::const_iterator it=children.begin()
            ; it!=children.end()
            ; ++it) {
        if (num == row)
            return (*it).get();
        num++;
    }
    return NULL;
}

boost::shared_ptr<Move> GameNode::GetMove() const {
    return move;
}

void GameNode::Print(std::ostream& os, int level) const {
    if (!level) {
        os << "Empty" << std::endl;
    }
    else {
        for (int i=0; i<level; i++)
            os << "  ";
        os
            << ((move->GetPlayer()==Player::First()) ? "O" : "#")
            << " "
            << move->GetLocation().ToCoords()
            << std::endl;
    }
    level++;
    //for (int i=0; i<children.size(); i++) {
    //    children[i]->Print(level);
    //}
    for (GameNode::Children::iterator it=children.begin()
            ; it!=children.end()
            ; ++it) {
        (*it)->Print(os, level);
    }
}
////

GameTree::GameTree()
{
    Reset();
}

GameTree::~GameTree()
{
    Reset();
}

void GameTree::Reset() {
    if (root.get())
        root->Collapse();
    root.reset(new GameNode());
    current = root;
}

void GameTree::AppendMove(const Move& m) {
    current = current->AppendMove(m);
}

boost::shared_ptr<Move> GameTree::GetLastMove() const {
    return current->GetMove();
}

GameNodePtr GameTree::GetRoot() const {
    return root;
}

std::string GameTree::Print() const {
    std::stringstream os;
    root->Print(os, 0);
    return os.str();
}

//end: GameTree



//section: GameTreeModel

GameTreeModel::GameTreeModel(GameNodePtr root, QObject *parent) :
    QAbstractItemModel(parent)
    ,rootItem(root.get())
{
}

GameTreeModel::~GameTreeModel()
{
    delete rootItem;
}

QModelIndex GameTreeModel::index(int row, int column, const QModelIndex& parent) const
{
//    std::cout << "DBG: index(" << row << ", " << column << ")" << std::endl;
    if (!hasIndex(row, column, parent)) {
//        std::cout << "DBG:   no index" << std::endl;
        return QModelIndex();
    }
    GameNode *parentItem;
    parentItem = parent.isValid()
        ? static_cast<GameNode*>(parent.internalPointer())
        : rootItem;

    GameNode *child = parentItem->GetChild(row);
    if (child) {
//        std::cout << "DBG:   child " << child->GetMove()->GetLocation().ToCoords() << std::endl;
        return createIndex(row, column, child);
    }
    return QModelIndex();
}

QModelIndex GameTreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    GameNode *child = static_cast<GameNode*>(index.internalPointer());
    GameNode *parent = child->GetParent();

    if (parent == rootItem)
        return QModelIndex();
    return createIndex(parent->GetRow(), 0, parent);
}

int GameTreeModel::rowCount(const QModelIndex& parent) const {
    GameNode *parentItem;
    parentItem = parent.isValid()
        ? static_cast<GameNode*>(parent.internalPointer())
        : rootItem;

    return parentItem->ChildrenCount();
}

int GameTreeModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant GameTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    //construct
    GameNode *node = static_cast<GameNode*>(index.internalPointer());

    boost::shared_ptr<Move> m = node->GetMove();
    return m.get()
        ? QString(m->GetPlayer().ToString().c_str())
          + QString(" ")
          + QString(m->GetLocation().ToCoords().c_str())
        : QString("Empty");
}

Qt::ItemFlags GameTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant GameTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QString("Game Tree");
    return QVariant();
}

//end: GameTreeModel


/*GAME_View::GAME_View(const std::string& ascii_tree, QWidget *parent) :
    QDialog(parent)
{
    treeView = new sTreeView(ascii_tree, this);*/
GAME_View::GAME_View(GameTree& t, QWidget *parent) :
        QDialog(parent),
        tree_(t)
{

    treeView = new gTreeView(this);
    model = new GameTreeModel(tree_.GetRoot());
    treeView->setModel(model);
    treeView->setHeaderHidden(true);

    connect(treeView, SIGNAL(activated(const QModelIndex&)), treeView, SLOT(selectionChanged(const QModelIndex&)));

    QPushButton *expButton = new QPushButton("Expand all");
    QPushButton *collButton = new QPushButton("Collapse all");
    QPushButton *closeButton = new QPushButton("Close");

    connect(expButton, SIGNAL(clicked()), treeView, SLOT(expandAll()));
    connect(collButton, SIGNAL(clicked()), treeView, SLOT(collapseAll()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *controls = new QHBoxLayout();
    controls->addWidget(expButton);
    controls->addWidget(collButton);
    controls->addWidget(closeButton);

    QVBoxLayout *all_ = new QVBoxLayout();
    all_->addLayout(controls);
    all_->addWidget(treeView);

    setLayout(all_);

}


GAME_View::~GAME_View()
{
    delete model;
}

void GAME_View::updateTree()
{
    std::cout << "updateTree()" << std::endl;
}

void gTreeView::selectionChanged(const QModelIndex & index)
{
    std::cout << "selectionChanged()" << std::endl;
}

