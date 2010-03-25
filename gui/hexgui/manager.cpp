#include <QDebug>
#include <boost/bind.hpp>
#include <iostream>
#include <sstream>
#include "manager.h"
#include "mcts_view.h"
#include "BoardScene.h"







manager::manager(BoardScene *scene, QObject * parent) :
    QObject(parent),
    /*m_index(0),*/
    m_boardScene(scene),
    gameTreeView(NULL),
    auto_reply(false),
    auto_player(Player::None())
{
  m_boardScene->setLeftButtonHandler(boost::bind(&manager::handleLeftClick, this, _1, _2));
  m_boardScene->setRightButtonHandler(boost::bind(&manager::handleRightClick, this, _1, _2));
}

void manager::handleLeftClick(int x, int y) {
/*  switch (m_index) {
    m_boardScene->addBlackStone(x, y);
    m_boardScene->addWhiteStone(x, y);
    m_boardScene->addMark(x, y);
    m_boardScene->addCircle(x, y);
    m_boardScene->addSquare(x, y);
    m_boardScene->addTriangle(x, y);
    m_boardScene->addLabel(x, y, m_label);
    qDebug() << "wrong combobox current index";
  }*/
    std::cout << "LeftClick on (" << x << ", " << y << ")." << std::endl;

    if (game_.nowWinner()==Player::None()) {
        Move m(game_.CurrentPlayer(), Location(x, y));
        bool ok = putStone(m);

        if (ok && auto_reply && auto_player!=Player::None())
            playMove();
    }
    else {
        std::cout << "Game has already finished." << std::endl;
    }
}

void manager::handleRightClick(int x, int y) {
/*  switch (m_index) {
    m_boardScene->removeStone(x, y);
    m_boardScene->removeMark(x, y);
    m_boardScene->removeCircle(x, y);
    m_boardScene->removeSquare(x, y);
    m_boardScene->removeTriangle(x, y);
    m_boardScene->removeLabel(x, y);
    qDebug() << "wrong combobox current index";
  }*/
    std::cout << "RightClick on (" << x << ", " << y << ")." << std::endl;

    Location loc(x, y);
    if (marked.find(loc)==marked.end()) {   //not found
        marked.insert(loc);
        m_boardScene->addTriangle(x, y);
    }
    else {  //found
        marked.erase(loc);
        m_boardScene->removeTriangle(x, y);
    }
}

bool manager::putStone(const Move& m) {


    if (game_.IsValidMove(m)) {
        clearLabels();
        boost::shared_ptr<Move> last_move = tree_.GetLastMove();
        if (last_move.get()) {
            m_boardScene->removeBGMark(
                    last_move->GetLocation().GetX(),
                    last_move->GetLocation().GetY()
                );
        }

        game_.Play(m);
        tree_.AppendMove(m);
//        tree_.Print();

        if (m.GetPlayer()==Player::First())
            m_boardScene->addBlackStone(m.GetLocation().GetX(), m.GetLocation().GetY());
        else
            m_boardScene->addWhiteStone(m.GetLocation().GetX(), m.GetLocation().GetY());

        last_move = tree_.GetLastMove();
        if (last_move.get()) {
            m_boardScene->addBGMark(
                    last_move->GetLocation().GetX(),
                    last_move->GetLocation().GetY(),
                    QColor(0,255,0,80)
                );
        }
        std::cout << "Put stone on: " << m.GetLocation().ToCoords() << std::endl;

        Player winner = game_.nowWinner();
        if (winner!=Player::None()) {
            emit stateChanged(winner);
        }
        emit movePlayed();
        return true;
    }

    std::cout << "Not a valid move." << std::endl;
    return false;
}

void manager::clearLabels() {
    for (uint x=1; x<=Dim::board_size; x++)
        for (uint y=1; y<=Dim::board_size; y++)
            m_boardScene->removeLabel(x, y);
    generated.clear();
}

void manager::showLabels() {
    std::cout << "showLabels()" << std::endl;
    for (std::map<Location, uint>::const_iterator it=generated.begin()
            ; it!=generated.end(); ++it)
    {
        m_boardScene->addLabel(
                it->first.GetX(),
                it->first.GetY(),
                QString::number(it->second)
            );
    }
}

void manager::clearMarkers() {
    for (uint x=1; x<=Dim::board_size; x++)
        for (uint y=1; y<=Dim::board_size; y++)
            m_boardScene->removeTriangle(x, y);
    marked.clear();
}

void manager::newGame() {
  //clear board
  m_boardScene->clearBoard();
  game_.ClearBoard();
  tree_.Reset();
  emit stateChanged(Player::None());
}

void manager::genMove() {
    std::cout << "genMove()" << std::endl;

    if (game_.nowWinner()==Player::None()) {
        clearLabels();

        Player current_ = game_.CurrentPlayer();
        generated[game_.GenMove().GetLocation()] ++;
        showLabels();
    }
    else {
        std::cout << "Game has already finished." << std::endl;
    }
}

void manager::searchMove() {
    std::cout << "searchMove()" << std::endl;

    if (game_.nowWinner()==Player::None()) {
        clearLabels();

        Player current_ = game_.CurrentPlayer();
        for (size_t i=0; i<20; i++) {
            (std::cout << ".").flush();
            generated[game_.GenMove().GetLocation()] ++;
        }
        std::cout << std::endl;
        showLabels();
    }
    else {
        std::cout << "Game has already finished." << std::endl;
    }
}

void manager::playMove() {
    if (game_.nowWinner()==Player::None()) {
        Player current_ = game_.CurrentPlayer();
        Move m = game_.GenMove();
        putStone(m);
        if (auto_reply)
            auto_player = current_;
    }
    else {
        std::cout << "Game has already finished." << std::endl;
    }
}

void manager::autoReply(int state) {
    std::cout << "autoReply()" << std::endl;
    auto_reply = state;
    auto_player = Player::None();
}

void manager::showTreeGame() {
    std::cout << "showTreeGame()" << std::endl;

//    std::string out = tree_.Print();
    if (gameTreeView) {
        gameTreeView->raise();
        gameTreeView->activateWindow();
        return;
    }
    //create new window
//    std::string out = tree_.Print();
    std::cout << "GTREE" << std::endl
              << tree_.Print()
              << std::endl
              << "END" << std::endl;
    gameTreeView = new GAME_View(tree_);

//    connect();
//    connect();
    connect(this, SIGNAL(movePlayed()), gameTreeView, SLOT(updateTree()));
    connect(gameTreeView, SIGNAL(moveChanged()), this, SLOT(changeCurrent()));
    connect(gameTreeView, SIGNAL(finished(int)), this, SLOT(closeTreeGame()));

    gameTreeView->show();
}

void manager::closeTreeGame() {
    std::cout << "closeTreeGame()" << std::endl;
    gameTreeView = NULL;
}

void manager::changeCurrent() {
    std::cout << "changeCurrent()" << std::endl;
    //slot, when current was changed in treeGameView
}

void manager::showTreeMcts() {
    std::cout << "showTreeMcts()" << std::endl;

    std::string out;
    game_.PrintTree(out, 10);

    MCTS_View *view = new MCTS_View(out);
    view->show();
}

void manager::setPlayouts(uint playouts) {
    std::cout << "# set playouts = " << playouts << std::endl;
    game_.GetTimeManager().playouts_per_move = playouts;
}

void manager::setTreeDepth(uint tree_depth) {
    std::cout << "# set tree_depth = " << tree_depth << std::endl;
    game_.SetMaxUTCTreeDepth(tree_depth);
}

