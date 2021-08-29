## PKB Abstract API

### Component: PKB
**Overview**: Stores the abstracted information extracted from the source code.

<<<<<<< HEAD
**Requirements**:
* STMT requires stmtRef and STMT_TYPE
* VAR requires stmtRef, entRef and VAR_TYPE
#### API:

1. PROC[] getProcTable()
    * Description: Returns a list of PROC. If the procTable is empty, return an empty list.
2. STMT[] getStmtTable(STMT_TYPE stmtType)
    * Description: Returns a list of STMT with the type corresponding to STMT_TYPE.
    If there are no such types of STMT, return an empty list.
3. Sim. for getVarTable
4. STMT[] getFollows(STMT stmt)
    * Description: Returns a list containing 1 STMT which follows stmt.
    If there is not STMT that follows, return an empty list.
5. STMT[] getPrevious(STMT stmt)
    * Description: Returns a list containing 1 STMT, x, where stmt follows x.
   If there is no previous STMT, return an empty list.
6. BOOLEAN isFollows(STMT stmt1, STMT stmt2)
    * Description: Returns true if stmt1 follows stmt2. False otherwise.
7. STMT[] getFollowsStar(STMT stmt)
    * Description: Returns a list of STMT which follows* stmt.
      If there is no STMT that follows, return an empty list.
8. STMT[] getPreviousStar(STMT stmt)
   * Description: Returns a list of STMT, x, where stmt follows* x.
     If there is no previous STMT, return an empty list.
9. BOOLEAN isFollowsStar(STMT stmt1, STMT stmt2)
   * Description: Returns true if stmt1 follows* stmt2. False otherwise.
10. sim. for getParent(STMT stmt), getChild(STMT stmt), isParent(STMT stmt1, STMT2)
   getParentStar(STMT stmt), getChildStar(STMT stmt), isParentStar(STMT stmt1, STMT, stmt2)
11. VAR[] getVarUsed(STMT stmt)
    * Description: Returns a list of VAR, v, where Uses (stmt, v). Empty list otherwise.
12. STMT[] getStmtUsing(Var v)
    * Description: Returns a list of STMT, s, where Uses (s, v). Empty list otherwise.
13. BOOLEAN isUses(STMT s, VAR v)
    * Description: Returns true if Uses(s, v). False otherwise.
14. sim. for getVarModified()
15. sim. for getStmtModifying()
16. sim. for isModifies(STMT s, VAR v)
17. RETURN_TYPE functionName(ARG_TYPE argName)
* Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>
=======
### PKB
**Overview**: The PKB serves as a database, consisting of a collection of data structures organised to optimise the time taken for information retrieval and queries.
#### API:

1. List<Node\<Statement\>> getStmt(Node<Statement> statement)

2. Node<Statement> getFollows(Node<Statement> statement)

3. List<Node\<Statement>> getFollowsStar(Node<Statement> statement)

4. List<Node\<Statement>> getPrevious(Node<Statement> statement)

5. List<Node\<Statement>> getPreviousStar(Node<Statement> statement)

6. Node<Statement> getParent(Node<Statement> statement)

7. List<Node\<Statement>> getParentStar(Node<Statement> statement)

8. Node<Statement> getChild(Node<Statement> statement)

9. List<Node\<Statement>> getChildStar(Node<Statement> statement)

10. List<Node\<Variable>> getVarUsed(Node<Statement> statement)

11. List<Node\<Statement>> getStmtUsing(Node<Variable> variable)

12. List<Node\<Variable>> getVarModified(Node<Statement> statement)

13. List<Node\<Statement>> getStmtModifying(Node<Variable> variable)

14. List<Node\<Statement>> getAllAssignStmt(Node<Variable> variable)

15. VOID populatePKB(List<HashTable> relationships, List<Node\<Statement>> statementList, List<Node\<Variable>> variableList)

>>>>>>> 6dd42009735dd3ef3b1225db44fc438fed654d37
    
    
