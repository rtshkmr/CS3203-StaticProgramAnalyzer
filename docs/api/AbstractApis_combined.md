# APIs

[![hackmd-github-sync-badge](https://hackmd.io/SzpXRChpTVSbmjd8zcvYSw/badge)](https://hackmd.io/SzpXRChpTVSbmjd8zcvYSw)


## PKB Abstract API

### Component: PKB
**Overview**: Stores the abstracted information extracted from the source code.

**Requirements**:
* STMT requires stmtRef and STMT_TYPE
* VAR requires stmtRef, entRef and VAR_TYPE
#### API:

1. PROC[] getProcTable()
   * Required only in iter 2
   * Description: Returns a list of PROC. If the procTable is empty, return an empty list.
2. list<string> getDesignEntity(DesignEntity de)
    * Description: Returns a list of strings with the corresponding data to that Design Entity.
    If there are no such types of DesignEntity, return an empty list.
3. list<tuple<DesignEntity, string>> GetFollows(string stmt)
    * Description: Returns a list containing 1 string which follows stmt.
    If there is no stmt that follows, return an empty list.
4. list<tuple<DesignEntity, string>> GetPrevious(string stmt)
    * Description: Returns a list containing 1 string, x, where stmt follows x.
   If there is no previous stmt, return an empty list.
5. list<tuple<DesignEntity, string>> GetFollowsStar(string stmt)
    * Description: Returns a list of stmt which follows* stmt.
      If there is no string that follows, return an empty list.
6. list<tuple<DesignEntity, string>> GetPreviousStar(string stmt)
   * Description: Returns a list of stmt, x, where stmt follows* x.
     If there is no previous string, return an empty list.
7. list<tuple<DesignEntity, string>> GetParent(string stmt)
   * Description: Returns a list containing 1 stmt, s, where Parent(s, stmt)
8. list<tuple<DesignEntity, string>> GetChild(string stmt)
   * Description: Returns a list containing 1 stmt, s, where Parent(stmt, s)
9. list<tuple<DesignEntity, string>> GetParentStar(string stmt)
   * Description: Returns a list containing 1 stmt, s, where Parent*(s, stmt)
10. list<tuple<DesignEntity, string>> GetChildStar(string stmt)
    * Description: Returns a list containing 1 stmt, s, where Parent*(stmt, s)
11. list<tuple<DesignEntity, string>> GetUsedBy(string stmt)
    * Description: Returns a list of VAR, v, where Uses (stmt, v). Empty list otherwise.
12. list<tuple<DesignEntity, string>> GetUses(string var)
    * Description: Returns a list of stmt, s, where Uses (s, v). Empty list otherwise.
13. list<tuple<DesignEntity, string>> GetModifiedBy(string stmt)
    * Description:  Returns a list of VAR, v, where Modifies (stmt, v). Empty list otherwise.
14. list<tuple<DesignEntity, string>> GetModifies(string var)
    * Description: Returns a list of stmt, s, where Modifies (s, v). Empty list otherwise.
15. vector<AssignEntity> getPatternByAssign(string stmtRef)
    * Description: Returns a list of a single OBJECT containing varName and expression.
16. vector<AssignEntity> getPatternByVariable(string varName)
    * Description: Returns a list of OBJECTs(?) each containing varName(LHS) and expression(RHS).


## Query Processor Abstract API

### List of Components (Only lists important ones)
1. QueryExtractor
2. QueryOptimizer
3. QueryEvaluator
4. QueryProjector

### QueryExtractor
**Overview**: `QueryExtractor` is a facade that calls subcomponents (`QueryTokenizer`, `QueryParser`, `QueryValidator`) in order to tokenize, parse and validate the user query and populate the relevant information inside the query objects.

#### API
1. VOID ExtractQuery(STRING query)
   * Description: Processes the query string, modifying the query objects in QueryExtractor in-place with the extracted information.


### QueryOptimizer
**Overview**: `QueryOptimizer` executes extra logic on the initial query object, in order to group the clauses in strategic ways to increase performance during evaluation. 

#### API
1. static void GroupClauses(std::vector<Clause*>* clauses, std::list<Group*>* groups, Synonym* target);
   * Description: Groups clauses (queries) that should be evaluated together, based on existence of common synonyms. The newly formed Groups of clauses are added in-place to the Groups data structure of the query object. Note: For iteration 1, the grouping algorithm has a rudimentary implementation as a maximum of 1 such that and 1 pattern clause is expected.


### QueryEvaluator
**Overview**: `QueryEvaluator` receives and further processes the query objects received from `QueryExtractor`, and evaluates the queries.

#### API
1. VOID QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group*> groups, PKB pkb);
   * Description: Initialiser for the QueryEvaluator instance, which takes in the query objects representing a valid parsed query, and an instance of the PKB object.


### QueryProjector
**Overview**: `QueryProjector` receives raw results of query evaluation, and formats them before passing the results to the autotester.

#### API
1. std::list<std::string> FormatQuery(std::vector<std::string> results);
   * Description: receives raw results of query evaluation as a vector of strings, and formats them before passing the list results to the autotester.



## Template for Abstract API

### \<Insert Module/Component Name\>
**Overview**: <insert purpose/responsibility of module>
#### API
1. RETURN_TYPE functionName(ARG_TYPE argName)
    * Requires: \<insert precondition\>
    * Description: \<insert description for abstract API call\>
2. RETURN_TYPE functionName(ARG_TYPE argName)
    * Requires: \<insert precondition\>
    * Description: \<insert description for abstract API call\>