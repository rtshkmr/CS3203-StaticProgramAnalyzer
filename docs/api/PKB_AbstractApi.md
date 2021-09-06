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
3. list<string> getFollows(string stmt)
    * Description: Returns a list containing 1 string which follows stmt.
    If there is no stmt that follows, return an empty list.
4. list<string> getPrevious(string stmt)
    * Description: Returns a list containing 1 string, x, where stmt follows x.
   If there is no previous stmt, return an empty list.
5. list<string> getFollowsStar(string stmt)
    * Description: Returns a list of stmt which follows* stmt.
      If there is no string that follows, return an empty list.
6. list<string> getPreviousStar(string stmt)
   * Description: Returns a list of stmt, x, where stmt follows* x.
     If there is no previous string, return an empty list.
7. list<string> getParent(string stmt)
   * Description: Returns a list containing 1 stmt, s, where Parent(s, stmt)
8. list<string> getChild(string stmt)
   * Description: Returns a list containing 1 stmt, s, where Parent(stmt, s)
9. list<string> getParentStar(string stmt)
   * Description: Returns a list containing 1 stmt, s, where Parent*(s, stmt)
10. list<string> getChildStar(string stmt)
    * Description: Returns a list containing 1 stmt, s, where Parent*(stmt, s)
11. list<string> getVarUsed(string stmt)
    * Description: Returns a list of VAR, v, where Uses (stmt, v). Empty list otherwise.
12. list<string> getStmtUsing(string var)
    * Description: Returns a list of stmt, s, where Uses (s, v). Empty list otherwise.
13. list<string> getVarModified(string stmt)
    * Description:  Returns a list of VAR, v, where Modifies (stmt, v). Empty list otherwise.
14. list<string> getStmtModifying(string var)
    * Description: Returns a list of stmt, s, where Modifies (s, v). Empty list otherwise.
