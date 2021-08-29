## PKB Abstract API

Template for Abstract API

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

    
    
