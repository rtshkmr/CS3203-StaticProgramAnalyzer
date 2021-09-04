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
2. STMT[] getStmtTable(STMT_TYPE stmtType)
    * Description: Returns a list of STMT with the type corresponding to STMT_TYPE.
    If there are no such types of STMT, return an empty list.
3. VAR[] getVarTable(VAR_TYPE varType)
    * Description: Returns a list of VAR with the type corresponding to VAR_TYPE.
4. STMT[] getFollows(STMT stmt)
    * Description: Returns a list containing 1 STMT which follows stmt.
    If there is not STMT that follows, return an empty list.
5. STMT[] getPrevious(STMT stmt)
    * Description: Returns a list containing 1 STMT, x, where stmt follows x.
   If there is no previous STMT, return an empty list.
6. STMT[] getFollowsStar(STMT stmt)
    * Description: Returns a list of STMT which follows* stmt.
      If there is no STMT that follows, return an empty list.
7. STMT[] getPreviousStar(STMT stmt)
   * Description: Returns a list of STMT, x, where stmt follows* x.
     If there is no previous STMT, return an empty list.
8. STMT[] getParent(STMT stmt)
   * Description: Returns a list containing 1 STMT, s, where Parent(s, stmt)
9. STMT[] getChild(STMT stmt)
   * Description: Returns a list containing 1 STMT, s, where Parent(stmt, s)
10. STMT[] getParentStar(STMT stmt)
    * Description: Returns a list containing 1 STMT, s, where Parent*(s, stmt)
11. STMT[] getChildStar(STMT stmt)
    * Description: Returns a list containing 1 STMT, s, where Parent*(stmt, s)
12. VAR[] getVarUsed(STMT stmt)
    * Description: Returns a list of VAR, v, where Uses (stmt, v). Empty list otherwise.
13. STMT[] getStmtUsing(Var v)
    * Description: Returns a list of STMT, s, where Uses (s, v). Empty list otherwise.
14. VAR[] getVarModified()
    * Description:  Returns a list of VAR, v, where Modifies (stmt, v). Empty list otherwise.
15. STMT[] getStmtModifying()
    * Description: Returns a list of STMT, s, where Modifies (s, v). Empty list otherwise.
