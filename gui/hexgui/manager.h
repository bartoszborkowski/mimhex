#ifndef MANAGER_H_
#define MANAGER_H_

#include <QObject>
#include <QString>
#include <QList>
#include "board_inc.h"
#include "game_view.h"
#include <map>
#include <set>
#include <boost/shared_ptr.hpp>

using namespace Hex;

class BoardScene;





class manager : public QObject
{
  Q_OBJECT
public:
  manager(BoardScene *scene, QObject * parent = 0);

  void handleLeftClick(int x, int y);
  void handleRightClick(int x, int y);

//public slots:
//  void setIndex(int index) { m_index = index; }
//  void setLabel(const QString& label) { m_label = label; }

private:
  bool putStone(const Move&);

public slots:
    void newGame();
    void genMove();
    void searchMove();
    void playMove();
    void autoReply(int);

    void showTreeGame();
    void closeTreeGame();
        void changeCurrent();

    void showTreeMcts();

    void setPlayouts(uint);
    void setTreeDepth(uint);

    void clearLabels();
    void showLabels();
    void clearMarkers();
signals:
    void stateChanged(const Player&);
    void movePlayed();
        //emit when tree changes
private:
//  int m_index;
//  QString m_label;
  BoardScene *m_boardScene;
  GAME_View *gameTreeView;
  Game game_;
  GameTree tree_;

  bool auto_reply;
  Player auto_player;

    struct LocCompare {
        inline bool operator() (const Location& l, const Location& r) const
        {
        return (l.GetX()==r.GetX())
            ? (l.GetY()<r.GetY())
            : (l.GetX()<r.GetX());
        }
    };

  std::map<Location, uint, LocCompare> generated;
  std::set<Location, LocCompare> marked;
};

#endif /* MANAGER_H_ */
