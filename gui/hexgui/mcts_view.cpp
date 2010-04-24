#include <QtGui>
#include <QPushButton>
#include <QCheckBox>
#include <QIntValidator>

#include "mcts_view.h"


//// TreeItem section
TreeItem::TreeItem(const QList<QVariant>& data, TreeItem *parent)
    : parentItem(parent), itemData(data)
{
}
    
TreeItem::~TreeItem() {
    qDeleteAll(childItems);
}

void TreeItem::appendChild(TreeItem *child) {
    childItems.append(child);
}

TreeItem *TreeItem::child(int row) {
    return childItems.value(row);
}

int TreeItem::childCount() const {
    return childItems.count();
}

int TreeItem::columnCount() const {
    return itemData.count();
}

QVariant TreeItem::data(int column) const {
    return itemData.value(column);
}

int TreeItem::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

TreeItem *TreeItem::parent() {
    return parentItem;
}
//// TreeItem end


//// TreeModel section
TreeModel::TreeModel(TreeItem *root, QObject *parent)
    : QAbstractItemModel(parent), rootItem(root)
{
//    QList<QVariant> rootData;
//    rootData << "Title" << "Summary";
//    rootItem = new TreeItem(rootData);
//    setupModelData(data.split(QString("\n")), rootItem);
}

TreeModel::~TreeModel() {
    delete rootItem;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex& parent) const {
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);
    return QVariant();
}


//treeview
sTreeView::sTreeView(const std::string& ascii_tree, QWidget *parent)
    : QTreeView(parent)
{
    Load(ascii_tree);
}

sTreeView::~sTreeView() {
    delete model;
}

void sTreeView::reloadModel(const std::string& ascii_tree) {
    delete model;
    Load(ascii_tree);
}

void sTreeView::Load(const std::string& ascii_tree) {
    model = new TreeModel(
            prepareData(
                QString(ascii_tree.c_str()).split(QString("\n"))
            )
        );
    setModel(model);
}

       

//window
MCTS_View::MCTS_View(const std::string& ascii_tree, QWidget *parent) :
  QDialog(parent)
{
    /*QTreeView *tree = new QTreeView(this);
    model = new TreeModel(
            prepareData(
                QString(ascii_tree.c_str()).split(QString("\n"))
            )
        );
    tree->setModel(model);*/
    treeView = new sTreeView(ascii_tree, this);

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

//MCTS_View::~MCTS_View() {
//    delete model;
//}
