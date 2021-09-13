#include <cassert>
#include "FollowsTExtractor.h"

/**
 * Extracts FollowsT from each procedure.
 *
 * pseudocode:
 * for each proc in proc list
 *  recurse using the first stmt in stmt list:
 *    if stmt is container
 *      recurse on the first stmt in its stmt list
 *      // this starts a new thread of extracting
 *    // continue
 *    if stmt found in follows hash
 *      recurse(following stmt) and get followsT of following stmt
 *      add the following stmt and followsT to followsT hash of stmt
 *    else base case: no following statement
 *      return empty list
 */
void FollowsTExtractor::Extract(Deliverable* deliverable) {
  this->deliverable_ = deliverable;

  for (Procedure* proc: deliverable_->proc_list_) {
    Statement* first_statement = proc->GetStatementList()->front();
    ExtractFollowsTFromThread(first_statement);
  }
}
/**
 * Extracts Follows* relationship recursively from the thread of Statements that Follow one another transitively.
 * Adds any intermediate Follows* relationship to the deliverable.
 *
 * @param top The Statement at the top of the Follows* thread.
 * @return List of Statements that Follows* the top statement.
 */
std::list<Statement*>* FollowsTExtractor::ExtractFollowsTFromThread(Statement* top) {
  // Check if top statement is a container, i.e. if/while
  if (Container* container = dynamic_cast<Container*>(top)) {
    Statement* first_statement = container->GetStatementList()->front();
    ExtractFollowsTFromThread(first_statement);
    // Statements in the container will not follow the container
    // , so there is no need to collect its result.
  }

  std::unordered_map<Statement*, Statement*> follows_hash = deliverable_->follow_hash_;
  if (follows_hash.find(top) != follows_hash.end()) {
    // there is a statement following top
    Statement* following_statement = follows_hash.find(top)->second;
    std::list<Statement*>* follows_T_list = ExtractFollowsTFromThread(following_statement);
    deliverable_->AddFollowsTransitiveRelationship(top, following_statement);
    deliverable_->AddFollowsTransitiveRelationshipForList(top, follows_T_list);

    std::unordered_map<Statement*, std::list<Statement*>*> ft = deliverable_->follows_T_hash_;
    assert(ft.find(top) != ft.end()); // there must be some FollowsT bcos there is Follows
    // returns all FollowsT of top
    return ft.find(top)->second;
  } else {
    // top is last statement in thread
    return new std::list<Statement*>();
  }
}
