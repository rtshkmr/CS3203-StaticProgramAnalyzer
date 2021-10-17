#include <exception/SyntaxException.h>
#include "CallsTExtractor.h"

/**
 * Extracts transitive Calls* relationship from Calls relationship in the deliverable.
 *
 * @param deliverable Deliverable populated by PSubsystem.
 * @throws SyntaxException when there is cyclic call.
 *
 * pseudocode - dfs with Procedure as vertices and Calls as edge
 * for all Calls in Calls map
 *  recurse with Calling if not in Call* map and not visited
 *  for all Called
 *  if no Called
 *      + visited
 *      return list with this procedure
 *  else if visited get Call*
 *  else recurse on every Called not in Call* map
 *      add Call* to this procedure with returned list
 *      if this procedure is found in returned list, throw SyntaxException
 *  + visited
 *  return this Call*
 */
void CallsTExtractor::Extract(Deliverable* deliverable) {
  this->deliverable_ = deliverable;

  std::vector<Procedure*> visited_procedures; // visited list
  for (auto[key, value]: deliverable_->calls_hash_) {
    bool in_calls_t_map = deliverable_->calls_T_hash_.count(key) != 0;
    bool was_visited = std::find(visited_procedures.begin(), visited_procedures.end(), key)
        != visited_procedures.end();  // procedure found in visited_procedures
    if (in_calls_t_map || was_visited) {
      continue;
    }

    ExtractCallsTFromProc(key, &visited_procedures);
  }
}

/**
 * Extracts Calls* relationship from a Procedure, and marking this Procedure as visited.
 *
 * @param calling_proc The input Procedure that may call other Procedures.
 * @param visited_procedures A vector of Procedures that have already been visited.
 * @return List of Procedures that is Called* by calling_proc.
 * @throws SyntaxException when there is cyclic call.
 */
std::list<Procedure*>* CallsTExtractor::ExtractCallsTFromProc(Procedure* calling_proc,
                                                              std::vector<Procedure*>* visited_procedures) {
  // procedure has Calls*, implies that it was visited.
  if (deliverable_->calls_T_hash_.count(calling_proc) != 0) {
    return deliverable_->calls_T_hash_.find(calling_proc)->second;
  }

  bool was_visited = std::find(visited_procedures->begin(), visited_procedures->end(), calling_proc)
      != visited_procedures->end();
  // procedure does not have Calls, found in visited_procedures, does not have Calls*
  if (deliverable_->calls_hash_.count(calling_proc) == 0 || was_visited) {
    visited_procedures->push_back(calling_proc);
    return new std::list<Procedure*>{calling_proc};
  }

  // must record visited before recurring to avoid cycle
  visited_procedures->push_back(calling_proc);

  std::list<Procedure*>* called_proc_list = deliverable_->calls_hash_.find(calling_proc)->second;
  for (Procedure* called_proc: *called_proc_list) {
    std::list<Procedure*>* extracted_procedures = ExtractCallsTFromProc(called_proc, visited_procedures);
    for (Procedure* extracted_proc: *extracted_procedures) {
      if (extracted_proc->GetName() == calling_proc->GetName()) { // uses overloaded ==
        throw SyntaxException("Cyclic call detected.");
      }
      deliverable_->AddCallsTransitiveRelationship(calling_proc, extracted_proc);
    }
    deliverable_->AddCallsTransitiveRelationship(calling_proc, called_proc);
  }

  return deliverable_->calls_T_hash_.find(calling_proc)->second;
}
