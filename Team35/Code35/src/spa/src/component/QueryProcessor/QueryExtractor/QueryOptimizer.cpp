#include "QueryOptimizer.h"
#include <component/QueryProcessor/types/Exceptions.h>

/**
 * Groups queries that should be evaluated together, based on existence of common synonyms.
 */
void QueryOptimizer::GroupClauses(std::vector<Clause*>* clauses, std::list<Group*>* groups, Synonym* target) {
  // TODO: implement grouping algorithm.
  // current implementation assumes only 1 such that and 1 pattern clause; manually grouping by common synonym.
  if (clauses->size() == 0) {
    return;
  }

  if (clauses->size() == 1) {
    // expecting 1 such that or 1 pattern clause
    bool has_target_syn = false;
    // such that clause
    if (typeid(*(*clauses)[0]) == typeid(SuchThat)) {
      SuchThat* st = dynamic_cast<SuchThat*>((*clauses)[0]);
      if (st->left_is_synonym && st->left_hand_side.compare(target->GetName()) == 0) {
        has_target_syn = true;
      } else if (st->right_is_synonym && st->right_hand_side.compare(target->GetName()) == 0) {
        has_target_syn = true;
      }
    } else {
      // pattern clause
      Pattern* pt = dynamic_cast<Pattern*>((*clauses)[0]);
      // has_target_syn if lhs is a target synonym, or if  syn-assn is a target synonym
      if (pt->assign_synonym.compare(target->GetName()) == 0 ||
      pt->left_is_synonym && pt->left_hand_side.compare(target->GetName()) == 0) {
        has_target_syn = true;
      }
    }
    // add clause into its own group
    std::vector<Clause*> cl1;
    cl1.push_back((*clauses)[0]);
    Group* g1 = new Group(cl1, has_target_syn);
    groups->push_back(g1);

  } else if (clauses->size() == 2) {
    // expecting 1 such that followed by 1 pattern clause
    bool has_target_syn = false;
    if (typeid(*(*clauses)[0]) == typeid(SuchThat)) {
      SuchThat* st = dynamic_cast<SuchThat*>((*clauses)[0]);
      if (st->left_is_synonym && st->left_hand_side.compare(target->GetName()) == 0) {
        has_target_syn = true;
      } else if (st->right_is_synonym && st->right_hand_side.compare(target->GetName()) == 0) {
        has_target_syn = true;
      }
    }
    std::vector<Clause*> cl1;
    cl1.push_back((*clauses)[0]);
    Group* g1 = new Group(cl1, has_target_syn);
    groups->push_back(g1);

    // if 1x pattern cl has common synonym, it goes in the same group.
    Pattern* pt = dynamic_cast<Pattern*>((*clauses)[1]);
    bool has_common_assign_syn = pt->assign_synonym.compare(cl1[0]->left_hand_side) == 0;
    bool has_common_args = pt->left_is_synonym && (
            pt->left_hand_side.compare(cl1[0]->left_hand_side) == 0 ||
            pt->left_hand_side.compare(cl1[0]->right_hand_side) == 0);
    if (has_common_assign_syn || has_common_args) {
      g1->AddClauseToVector(pt);
    } else {
      std::vector<Clause*> cl2;
      cl2.push_back(pt);
      bool pattern_cl_has_tgt_syn = false;
      if (pt->assign_synonym.compare(target->GetName()) == 0 ||
      pt->left_is_synonym && pt->left_hand_side.compare(target->GetName()) == 0) {
        pattern_cl_has_tgt_syn = true;
      }
      Group* g2 = new Group(cl2, pattern_cl_has_tgt_syn);
      groups->push_back(g2);
    }
  } else {
    throw PQLValidationException("Got more than 2 clauses.");
  }
}