#ifndef AUTOTESTER_PKBQUERYCOMMAND_H
#define AUTOTESTER_PKBQUERYCOMMAND_H

#include <component/QueryProcessor/types/IntermediateTable.h>
#include "component/PKB/DBManager.h"
#include "model/Entity.h"
#include "component/QueryProcessor/types/Types.h"

/**
 * Contains the business logic of querying information directly from the PKB.
 */
class PKBQueryReceiver {
  private:
    DBManager *db_manager;
  public:
    explicit PKBQueryReceiver(DBManager *db_manager);

    IntermediateTable *QueryPKBTwoSynonyms(PKBRelRefs rel, DesignEntity first_synonym, DesignEntity second_synonym);
    IntermediateTable *QueryPKBByValue(PKBRelRefs rel, std::string value);
    IntermediateTable *QueryEntityWithWildcard(PKBRelRefs rel, DesignEntity entity);
    IntermediateTable *QueryPKBByValueForBoolean(PKBRelRefs rel, std::string value);
    IntermediateTable *QueryPKBByValueForBoolean(PKBRelRefs rel, std::string first_value, std::string second_value);
    IntermediateTable *QueryRelRefExistence(PKBRelRefs rel);
    IntermediateTable *QueryDesignEntity(DesignEntity design_entity);
    IntermediateTable *QueryPatternByValue(DesignEntity design_entity, std::string value);
    IntermediateTable *QueryAttributeMatch(DesignEntity first_design_entity, DesignEntity second_design_entity);
    IntermediateTable *QueryEntityAttributeMatch(DesignEntity design_entity, Attribute attribute, std::string value);
};

// TODO: change the set receiver method to be in the base class.
class PKBQueryCommand {
  public:
    virtual void SetReceiver(PKBQueryReceiver *receiver) = 0;
    virtual IntermediateTable * ExecuteQuery(Clause *clause) = 0;

    PKBRelRefs GetPKBRelRef(RelRef relation, bool order_of_values_unchanged_from_clause);
};

class QuerySuchThatTwoSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    explicit QuerySuchThatTwoSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver) override;
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};


class QuerySuchThatOneSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    explicit QuerySuchThatOneSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver) override;
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

/**
 * This class should handle all sorts of no-synonym queries regardless of number of wildcards and should always return
 * a boolean in the Intermediate table.
 */
class QuerySuchThatNoSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    explicit QuerySuchThatNoSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver) override;
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QueryPatternTwoSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    explicit QueryPatternTwoSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver) override;
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QueryPatternOneSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    explicit QueryPatternOneSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver) override;
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QueryWithTwoSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    explicit QueryWithTwoSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver) override;
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QueryWithOneSynonymCommand : public PKBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    explicit QueryWithOneSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver) override;
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

#endif //AUTOTESTER_PKBQUERYCOMMAND_H
