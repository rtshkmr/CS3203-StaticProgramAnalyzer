#ifndef AUTOTESTER_DBQUERYCOMMAND_H
#define AUTOTESTER_DBQUERYCOMMAND_H

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
    PKBQueryReceiver(DBManager *db_manager);

    IntermediateTable *QueryPKBTwoSynonyms(PKBRelRefs rel, DesignEntity first_synonym, DesignEntity second_synonym);
    IntermediateTable *QueryPKBByValue(PKBRelRefs rel, std::string value);
    IntermediateTable *QueryEntityWithWildcard(PKBRelRefs rel, DesignEntity entity);
    IntermediateTable *QueryPKBByValueForBoolean(PKBRelRefs rel, std::string value);
    IntermediateTable *QueryPKBByValueForBoolean(PKBRelRefs rel, std::string first_value, std::string second_value);
    IntermediateTable *QueryRelRefExistence(PKBRelRefs rel);
    IntermediateTable *QueryDesignEntity(DesignEntity design_entity);
    IntermediateTable *QueryPatternByValue(DesignEntity design_entity, std::string value);
};

class DBQueryCommand {
  public:
    virtual void SetReceiver(PKBQueryReceiver *receiver) = 0;
    virtual IntermediateTable * ExecuteQuery(Clause *clause) = 0;

    PKBRelRefs GetPKBRelRef(RelRef relation, bool order_of_values_unchanged_from_clause);
};

class QuerySuchThatTwoSynonymCommand : public DBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QuerySuchThatTwoSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};


class QuerySuchThatOneSynonymCommand : public DBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QuerySuchThatOneSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

/**
 * This class should handle all sorts of no-synonym queries regardless of number of wildcards and should always return
 * a boolean in the Intermediate table.
 */
class QuerySuchThatNoSynonymCommand : public DBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QuerySuchThatNoSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QueryPatternTwoSynonymCommand : public DBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QueryPatternTwoSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

class QueryPatternOneSynonymCommand : public DBQueryCommand {
  private:
    Clause *clause;
    PKBQueryReceiver *receiver;
  public:
    QueryPatternOneSynonymCommand(Clause *clause);
    void SetReceiver(PKBQueryReceiver *receiver);
    IntermediateTable * ExecuteQuery(Clause *clause) override;
};

#endif //AUTOTESTER_DBQUERYCOMMAND_H
