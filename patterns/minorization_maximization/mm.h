// TODO player
// TODO update positive, negatice in gradient descent
// TODO mini batches

#include <vector>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>

#include "patterns/rep.h"
#include "lib/conditional_assert.h"
#include "lib/types.h"
#include "lib/random.h"

//using namespace std;

namespace Mm {

// -----------------------------------------------------------------------------

enum Feature {
  kPatternFeature = 0,
  feature_count = 1
};

const uint level_count [feature_count] = { 2051 };


// -----------------------------------------------------------------------------

struct Gammas {
  Gammas () {
    gammas.resize (feature_count);
    w.resize (feature_count);
    rep (feature, feature_count) {
      gammas [feature].resize (level_count [feature]);
      w [feature].resize (level_count [feature]);
    }
    Reset ();
  }

  void Reset () {
    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        Set (feature, level, 1.0);
        w [feature] [level] = 0.0;
      }
    }
    t = 1.5; // TODO parameter
  }

  double Get (uint feature, uint level) const {
    ASSERT (feature < feature_count);
    ASSERT (level < level_count [feature]);
    return gammas [feature] [level];
  }

  void LambdaUpdate (uint feature, uint level, double update) {
    ASSERT (feature < feature_count);
    ASSERT (level < level_count [feature]);
    gammas [feature] [level] *= exp (update / t);
    t += 0.0000001;
  }

  void Set (uint feature, uint level, double value) {
    ASSERT (feature < feature_count);
    ASSERT (level < level_count [feature]);
    gammas [feature] [level] = value;
  }

  void Normalize () {
    rep (feature, feature_count) {
      double log_sum = 1.0;
      double n = 0.0;
      rep (level, level_count [feature]) {
        log_sum += log (Get (feature, level));
        n += 1.0;
      }

      double mul = exp (log_sum / n);

      rep (level, level_count [feature]) {
        gammas [feature] [level] /= mul;
      }
    }
  }

  double Distance (const Gammas& other) {
    double sum = 0.0;

    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        double d = log (Get (feature, level) / other.Get(feature, level));
        sum += d*d;
      }
    }

    return sqrt(sum);
  }

  std::string ToString () {
    std::ostringstream out;
    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        out << std::setprecision(5) << std::setw (8) << log (Get (feature, level)) << " ";
      }
      out << " | ";
    }
    return out.str ();
  }


  std::vector <std::vector <double> > w;
private:
  std::vector <std::vector <double> > gammas;
  double t;
};

// -----------------------------------------------------------------------------

struct Team {
  Team () {
    rep (feature, feature_count) {
      levels [feature] = 0;
    }
  }

  void SetFeatureLevel (uint feature, uint level) {
    ASSERT (feature < feature_count);
    ASSERT (level < level_count [feature]);
    levels [feature] = level;
  }

  double TeamGamma (const Gammas& gammas) const {
    double mul = 1.0;
    rep (feature, feature_count) {
      mul *= gammas.Get (feature, levels [feature]);
    }
    return mul;
  }

  double IncompleteTeamGamma (const Gammas& gammas, uint skip_feature, double* incomplete) const {
    double mul = 1.0;
    rep (feature, feature_count) {
      if (feature != skip_feature) {
        mul *= gammas.Get (feature, levels [feature]);
      }
    }
    *incomplete = mul;
    return mul * gammas.Get (skip_feature, levels [skip_feature]);;
  }

  double TeamGammaDiff (const Gammas& gammas, uint feature, uint level) const {
    if (levels [feature] != level)
      return 0.0;

    return
      TeamGamma (gammas) / gammas.Get (feature, levels [feature]);
  }

  void GradientUpdate (Gammas& gammas, double update) {
    rep (feature, feature_count) {
      gammas.LambdaUpdate (feature, levels [feature], update);
    }
  }

  std::string ToString () {
    std::ostringstream out;
    rep (ii, feature_count) {
      out << (ii == 0 ? "" : " ") << levels[ii];
    }
    return out.str();
  }

  uint levels [feature_count];
};

// -----------------------------------------------------------------------------

struct Match {

  Team& NewTeam () {
    teams.resize (teams.size() + 1);
    return teams.back();
  }

  void SetWinnerLastTeam () {
    ASSERT (teams.size() > 0);
    this->winner = teams.size()-1;
  }

  void SetRandomWinner (const Gammas& gammas) {
    std::vector <double> team_gammas (teams.size());
    double sum = 0.0;
    rep (ii, teams.size()) {
      team_gammas [ii] = teams [ii].TeamGamma (gammas);
      sum += team_gammas [ii];
    }
    double sample = Rand::next_double (sum);
    double sum2 = 0.0;
    rep (ii, teams.size()) {
      sum2 += team_gammas [ii];
      if (sum2 >= sample) {
        winner = ii;
        break;
      }
    }
    //cerr << "X " << ToString () << " " << sum << " " << sample << endl;
  }

  double TotalGammaDiff (const Gammas& gammas, uint feature, uint level) {
    double sum = 0.0;
    rep (ii, teams.size()) {
      sum += teams[ii].TeamGammaDiff (gammas, feature, level);
    }
    return sum;
  }

  double TotalGamma (const Gammas& gammas) {
    double sum = 0.0;
    rep (ii, teams.size()) {
      sum += teams[ii].TeamGamma (gammas);
    }
    return sum;
  }

  std::string ToString () {
    std::ostringstream out;
    rep (ii, teams.size()) {
      out << "(" << teams[ii].ToString () << ")" << (winner == ii ? "! " : "  ");
    }
    return out.str ();
  }

  void GradientUpdate (Gammas& gammas) {
    // Calculate probabilities
    std::vector<double> p(teams.size());
    double sum = 0.0;
    rep (ii, teams.size()) {
      p [ii] = teams[ii].TeamGamma(gammas);
      sum += p[ii];
    }
    rep (ii, teams.size()) {
      p[ii] /= sum;
    }

    teams[winner].GradientUpdate (gammas, 1.0); // update +alpha
    rep (ii, teams.size()) {
      teams[ii].GradientUpdate (gammas, -p[ii]);
    }
  }

  double LogLikelihood (const Gammas& gammas) {
    return log (teams[winner].TeamGamma (gammas) / TotalGamma (gammas));
  }

  std::vector <Team> teams;
  uint winner;
};

// -----------------------------------------------------------------------------

struct BtModel {
  BtModel () {
    Rand::init(123);
    act_match = 0;
    prior_games = 4.0;
  }

  Match& NewMatch () {
    matches.resize (matches.size() + 1);
    return matches.back();
  }

  void PreprocessData () {
    gammas.Reset ();
    rep (ii, matches.size()) {
      const Match& match = matches [ii];
      const Team& winner = match.teams [match.winner];

      rep (feature , feature_count) {
        uint level = winner.levels [feature];
        gammas.w [feature] [level] += 1.0;
      }
    }
  }

  // Batch MM
  void TrainFeature (uint feature) {
    std::vector <double> c_e (level_count [feature]);
    // prior
    rep (level, level_count [feature]) {
      c_e [level] = prior_games / (gammas.Get (feature, level) + 1.0);
    }

    rep (ii, matches.size ()) {
      const std::vector <Team>& teams = matches [ii].teams;

      // itg = incomplete team gamma (without current feature)
      std::vector<double> itg(teams.size());
      double tg_sum = 0.0; // E in Remi's paper

      rep (jj, teams.size()) {
        tg_sum += teams [jj].IncompleteTeamGamma (gammas, feature, & itg[jj]);
      }

      double tg_sum_inv = 1.0 / tg_sum;

      rep (jj, teams.size()) {
        // level of the updated feature in current team
        uint level = teams [jj].levels [feature];
        c_e [level] += itg [jj] * tg_sum_inv;
      }
    }

    rep (level, level_count [feature]) {
      double new_gamma = (gammas.w [feature] [level] + prior_games / 2) / c_e [level];
      gammas.Set (feature, level, new_gamma);
    }

    gammas.Normalize ();
  }

  void Train (uint epochs) {
    double ll_improve [feature_count];
    rep (f, feature_count)  ll_improve [f] = 1000.0;

    double last_ll = LogLikelihood();
    std::cerr << "Begin LL: " << last_ll << std::endl;

    rep (ii, epochs) {
      uint best_f = 0;
      rep (f, feature_count) {
        if (ll_improve[f] > ll_improve[best_f]) {
          best_f = f;
        }
      }

      std::cerr << "Train feature " << best_f << " ... " << std::flush;
      TrainFeature (best_f);

      double new_ll = LogLikelihood();
      ll_improve [best_f] = new_ll - last_ll;
      last_ll = new_ll;
      std::cerr << "New LL = " << new_ll << "; delta LL = " << ll_improve [best_f] << std::endl;
    }
  }

  // Minorization - Maximization algorithm
  void UpdateGamma (uint feature, uint level) {

    // prior
    double c_e = 2.0 / (gammas.Get (feature, level) + 1.0);

    rep (ii, matches.size()) {
      double tgd = matches [ii].TotalGammaDiff (gammas, feature, level);
      double tg  = matches [ii].TotalGamma (gammas);
      c_e += tgd / tg;
    }
    // + 1.0 is prior
    double w = gammas.w[feature][level] + 1.0;

    gammas.Set (feature, level, w / c_e);
  }

  void DoFullUpdate () {
    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        UpdateGamma (feature, level);
        //cerr << level << " "  << LogLikelihood() << endl;

      }
    }
    gammas.Normalize ();
  }

  void DoGradientUpdate (uint n) {

    rep (ii, n) {
      matches [act_match].GradientUpdate (gammas);
      act_match += 1;
      if (act_match >= matches.size()) act_match = 0;
    }
    gammas.Normalize ();
  }

  double LogLikelihood () {
    double sum = 0.0;
    rep (ii, matches.size ()) {
      sum += matches[ii].LogLikelihood (gammas);
    }
    return sum / matches.size();
  }

  std::vector <Match> matches;
  uint act_match;
  Gammas gammas;
  double prior_games;
};

// -----------------------------------------------------------------------------

void Test () {
  Gammas true_gammas;

  rep (feature, feature_count) {
    rep (level, level_count[feature]) {
      true_gammas.Set (feature, level, exp (Rand::next_double(6)));
    }
  }

  true_gammas.Normalize ();

  BtModel model;
  rep (ii, 200000) {
    Match& match = model.NewMatch ();
    rep (jj, 200) { // TODO randomize team number
      Team& team = match.NewTeam ();
      rep (feature, feature_count) {
        uint level = Rand::next_rand(level_count[feature]);
        team.SetFeatureLevel (feature, level);
      }
    }
    match.SetRandomWinner (true_gammas);
    //cerr << ii << ": " << match.ToString () << endl;
  }

  model.PreprocessData ();
  std::cerr
    << std::endl << "---------------------------------" << std::endl
    << true_gammas.Distance (model.gammas)
    << " / " <<  model.LogLikelihood() << std::endl << std::endl;

  rep (epoch, 15*feature_count) {
    model.TrainFeature (epoch % feature_count);
    std::cerr
      << true_gammas.Distance (model.gammas)
      << " / " <<  model.LogLikelihood() << std::endl;
  }

  // // ----------------------

  // model.PreprocessData ();
  // cerr
  //   << endl << "---------------------------------" << endl
  //   << true_gammas.Distance (model.gammas)
  //   << " / " <<  model.LogLikelihood() << endl;

  // rep (epoch, 20) {
  //   if (epoch % 10 == 0) cerr << endl;
  //   model.DoGradientUpdate (100000);
  //   cerr
  //     << true_gammas.Distance (model.gammas) << " / "
  //     << model.LogLikelihood() << endl;
  // }
  // cerr << endl;

  // ----------------------


  // cerr << endl << "---------------------------------" << endl;

  // model.PreprocessData ();
  // cerr
  //   << true_gammas.Distance (model.gammas)
  //   << " / " <<  model.LogLikelihood() << endl;
  // rep (epoch, 4) {
  //   model.DoFullUpdate ();
  //   cerr
  //     << true_gammas.Distance (model.gammas)
  //     << " / " <<  model.LogLikelihood() << endl;
  // }
}

// -----------------------------------------------------------------------------

} // namespace
