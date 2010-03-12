#include <QtGui>
#include <QPushButton>
#include <QCheckBox>
#include <QIntValidator>

#include "hexgui.h"
#include "manager.h"
#include "BoardScene.h"
#include "BoardView.h"
#include "SquareGrid.h"
#include "HexGrid.h"
#include "YGrid.h"
//#include "HoGrid.h"
//#include "HavannahGrid.h"

HexGui::HexGui(QWidget *parent) :
  QDialog(parent) {
  //BoardScene *boardScene = BoardScene::createGoScene(13);
  BoardScene *boardScene = BoardScene::createHexScene(Dim::board_size);
  BoardView *boardView = new BoardView(boardScene, this);
  m = new manager(boardScene, this);

//  labelEdit = new QLineEdit;
//  QLabel *labelLabel = new QLabel("Label");
//  labelLabel->setBuddy(labelEdit);
//  labelEdit->setEnabled(false);

//  QComboBox *typeComboBox = new QComboBox;
//  QLabel *typeLabel = new QLabel("Type");
//  typeLabel->setBuddy(typeComboBox);
//  typeComboBox->addItem("Black Stone");
//  typeComboBox->addItem("White Stone");
//  typeComboBox->addItem("Mark");
//  typeComboBox->addItem("Circle");
//  typeComboBox->addItem("Square");
//  typeComboBox->addItem("Triangle");
//  typeComboBox->addItem("Label");

//  QGridLayout *layout = new QGridLayout;
//  layout->addWidget(labelLabel, 0, 0);
//  layout->addWidget(labelEdit, 0, 1);
//  layout->addWidget(typeLabel, 1, 0);
//  layout->addWidget(typeComboBox, 1, 1);


  //buttons
  QPushButton *new_game = new QPushButton("New game");

  QPushButton *gen_move = new QPushButton("Gen move");
  QPushButton *search_move = new QPushButton("Search move");

  QPushButton *play_move = new QPushButton("Play move");
  QCheckBox *auto_reply = new QCheckBox("Auto reply");

  QPushButton *tree_game = new QPushButton("Game tree");
  QPushButton *tree_mcts = new QPushButton("MCTS tree");

  QPushButton *set_playouts = new QPushButton("Set playouts per move");
  QPushButton *set_tree_depth = new QPushButton("Set max UTC tree depth");

  QPushButton *quit = new QPushButton("Quit");


  QVBoxLayout *controls = new QVBoxLayout;
  controls->addWidget(new QLabel("<b>HexGui</b>"), 0, Qt::AlignHCenter);
  controls->addSpacing(10);
  controls->addWidget(new_game);
  controls->addSpacing(10);
  controls->addWidget(gen_move);
  controls->addWidget(search_move);
  controls->addSpacing(10);
  controls->addWidget(play_move);
  controls->addWidget(auto_reply);
  controls->addSpacing(10);
  controls->addWidget(tree_game);
  controls->addWidget(tree_mcts);
  controls->addSpacing(10);
  controls->addWidget(set_playouts);
  controls->addWidget(set_tree_depth);
  controls->addSpacing(10);
  controls->addWidget(quit);
  controls->addStretch();

  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->addWidget(boardView);
  mainLayout->addLayout(controls);
//  mainLayout->addLayout(layout);

//  connect(typeComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(typeIndexChanged(const QString& )));
//  connect(typeComboBox, SIGNAL(currentIndexChanged(int)), m, SLOT(setIndex(int)));
//  connect(labelEdit, SIGNAL(textChanged(const QString &)), m, SLOT(setLabel(const QString&)));

  connect(new_game, SIGNAL(clicked()), m, SLOT(newGame()));
  connect(gen_move, SIGNAL(clicked()), m, SLOT(genMove()));
  connect(search_move, SIGNAL(clicked()), m, SLOT(searchMove()));
  connect(play_move, SIGNAL(clicked()), m, SLOT(playMove()));
  connect(auto_reply, SIGNAL(stateChanged(int)), m, SLOT(autoReply(int)));
  connect(tree_game, SIGNAL(clicked()), m, SLOT(showTreeGame()));
  connect(tree_mcts, SIGNAL(clicked()), m, SLOT(showTreeMcts()));
  connect(set_playouts, SIGNAL(clicked()), this, SLOT(setPlayouts()));
  connect(set_tree_depth, SIGNAL(clicked()), this, SLOT(setTreeDepth()));
  connect(quit, SIGNAL(clicked()), this, SLOT(close()));

  connect(m, SIGNAL(stateChanged(const Player&)), this, SLOT(setWinner(const Player&)));

  statebar = new QLabel();
  QVBoxLayout *all_ = new QVBoxLayout;
  all_->addLayout(mainLayout);
  all_->addWidget(statebar);

  setLayout(all_);
}

//void HexGui::typeIndexChanged(const QString& text) {
//  labelEdit->setEnabled(text == "Label");
//}

void HexGui::setPlayouts() {
    bool ok;
    uint playouts = QInputDialog::getInt(
            this, "Set playouts per move", "Enter unsigned integer:",
            100000, 0, 2000000000, 1, &ok);
    if (ok)
        m->setPlayouts(playouts);
}
void HexGui::setTreeDepth() {
    bool ok;
    uint tree_depth = QInputDialog::getInt(
            this, "Set max UTC tree depth", "Enter unsigned integer:",
            -1, -1, Dim::field_count, 1, &ok);
    if (ok)
        m->setTreeDepth(tree_depth);
}

void HexGui::setWinner(const Player& player)
{
    if (player==Player::First()) {
        statebar->setText("black wins!");
    } else if (player==Player::Second()) {
        statebar->setText("white wins!");
    } else {
        statebar->setText("");
    }
}

