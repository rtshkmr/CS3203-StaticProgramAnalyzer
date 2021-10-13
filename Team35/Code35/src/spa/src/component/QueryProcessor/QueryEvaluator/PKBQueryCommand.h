#ifndef AUTOTESTER_PKBQUERYCOMMAND_H
#define AUTOTESTER_PKBQUERYCOMMAND_H

#include <component/QueryProcessor/types/IntermediateTable.h>
#include "component/PKB/PKB.h"
#include "model/Entity.h"
#include "component/QueryProcessor/types/Types.h"

class PKBQueryReceiver {
  private:
    PKB *pkb;
  public:
    PKBQueryReceiver(PKB *pkb);
    //    std::vector<Entity *> QueryPKBSuchThat(RelRef r, const std::string& stmt, bool is_first_param);
    //    bool QueryPkbForRelationshipExistence(SuchThat *such_that_clause);
    //    std::vector<Entity *> QueryPkbPattern(bool is_assign_synonym, const std::string& value);
    IntermediateTable *QueryPKBTwoSynonyms(PKBRelRefs rel, DesignEntity first_synonym, DesignEntity second_synonym);
    IntermediateTable *QueryPKBByValue(PKBRelRefs rel, std::string value);
    IntermediateTable *QueryRelRefExistence(PKBRelRefs rel);
    IntermediateTable *QueryDesignEntity(DesignEntity design_entity);
    IntermediateTable *QueryPatternByValue(DesignEntity design_entity, std::string value);
};

class PKBQueryCommand {
  public:
    virtual ~PKBQueryCommand() = default;
    virtual void SetReceiver(PKBQueryReceiver *receiver) = 0;
    virtual IntermediateTable * ExecuteQuery(Clause *clause) = 0;

    PKBRelRefs GetPKBRelRef(RelRef relation);
};

class QuerySuchThatTwoSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QuerySuchThatTwoSynonymCommand(Clause *clause);
    QuerySuchThatTwoSynonymCommand(PKBQueryReceiver *receiver, Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QuerySuchThatOneSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QuerySuchThatOneSynonymCommand(Clause *clause);
    QuerySuchThatOneSynonymCommand(PKBQueryReceiver *receiver, Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QuerySuchThatNoSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QuerySuchThatNoSynonymCommand(Clause *clause);
    QuerySuchThatNoSynonymCommand(PKBQueryReceiver *receiver, Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QueryPatternTwoSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QueryPatternTwoSynonymCommand(Clause *clause);
    QueryPatternTwoSynonymCommand(PKBQueryReceiver *receiver, Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QueryPatternOneSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QueryPatternOneSynonymCommand(Clause *clause);
    QueryPatternOneSynonymCommand(PKBQueryReceiver *receiver, Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

#endif //AUTOTESTER_PKBQUERYCOMMAND_H
