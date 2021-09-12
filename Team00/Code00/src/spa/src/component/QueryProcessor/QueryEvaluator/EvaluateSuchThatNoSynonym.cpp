//
// Created by Max Ng on 12/9/21.
//

#import "EvaluateSuchThatNoSynonym.h"

bool EvaluateNoSynonym(SuchThat st, PKB pkb) {
  std::string firstParam = st.left_hand_side;
  std::string secondParam = st.right_hand_side;
  if (firstParam == "_" && secondParam == "_") {
    // Query the PKB for the existence of this relationship
    // PKB.hasXxxRelRef()
  } else if (firstParam == "_") {
    std::list<std::tuple<DesignEntity, std::string>> result = queryPKBSuchThat(pkb, st.rel_ref, st.right_hand_side, false);
    return result.size() != 0;    // Return true if there is some value being returned.
  } else if (secondParam == "_") {
    std::list<std::tuple<DesignEntity, std::string>> result = queryPKBSuchThat(pkb, st.rel_ref, st.left_hand_side, true);
    return result.size() != 0;    // Return true if there is some value being returned.
  } else {
    std::list<std::tuple<DesignEntity, std::string>> result = queryPKBSuchThat(pkb, st.rel_ref, st.left_hand_side, true);
    for (auto iter = result.begin(); iter != result.end(); iter++) {
      // Check if second synonym in list
      if (std::get<1>(*iter) == st.right_hand_side) {
        return true;
      }
    }
    return false;
  }
}

