## PKB Abstract API

### Important Definitions
* **Deliverable**: A wrapper object that contains the EntityTables, RelationshipTables that will be stored in the `PKB`
* **DesignEntity**: An enum class that enumerates the different types of entities (kStmt, kRead, kPrint, kCall, kWhile, kIf, kAssign, kVariable, kConstant, kProcedure, kInvalid)
* **EntityTuple** (format): A tuple of DesignEntity and a string, representing an entity. This string represents either the procedure name, statement number or variable name, depending on what type the EntityType is representing.
* **AssignEntity**: An object that represents Assign statements, also including an AssignmentExpression object
* **AssignmentExpression**: A class that takes in the RHS of an expression (in infix format) and transforms it to postfix, also wrapping some string comparison functions

### List of Components
1. PKB

### SourceProcessor
**Overview**: `PKB` stores all the information known about the input source SIMPLE code, including all entities and their associated metadata, as well as all the relationships between those entities. This information is stored in such a way that optimizes for the efficiency with which queries can be evaluated by the `QueryEvaluator`.
#### API
1. VOID PopulateDataStructures(DELIVERABLE d)
    * Description: Receives a DELIVERABLE object, containing all entities and relationships between those entities, and stores that information within curated data structures within the PKB.

2. LIST<ENTITYTUPLE> GetFollows(STRING stmt)
    * Description: Receives a string referring to a statement number and returns a list of a single object, referring to the statement directly after the specified statement. If there are none, returns an empty list.

3. LIST<ENTITYTUPLE> GetPrevious(STRING stmt)
    * Description: Receives a string referring to a statement number and returns a list of a single object, referring to the statement directly before the specified statement. If there are none, returns an empty list.

4. LIST<ENTITYTUPLE> GetParent(STRING stmt)
    * Description: Receives a string referring to a statement number and returns a list of objects, each referring to child statements of the specified statement. If there are none, returns an empty list.

5. LIST<ENTITYTUPLE> GetChild(STRING stmt)
    * Description: Receives a string referring to a statement number and returns a list of a single object, referring to the parent statement of the specified statement. If there are none, returns an empty list.

6. LIST<ENTITYTUPLE> GetUses(STRING stmt)
    * Description: Receives a string referring to a statement number and returns a list of objects, each referring to variables used by the specified statement. If there are none, returns an empty list.

7. LIST<ENTITYTUPLE> GetUsedBy(STRING var)
    * Description: Receives a string referring to a variable and returns a list of objects, each referring to statements that use the specified variable. If there are none, returns an empty list.

8. LIST<ENTITYTUPLE> GetModifies(STRING stmt)
    * Description: Receives a string referring to a statement number and returns a list of objects, each referring to variables modified by the specified statement. If there are none, returns an empty list.

9. LIST<ENTITYTUPLE> GetModifiedBy(STRING var)
    * Description: Receives a string referring to a variable and returns a list of objects, each referring to statements that modify the specified variable. If there are none, returns an empty list.

10. LIST<ENTITYTUPLE> GetFollowsT(STRING stmt)
* Description: Receives a string referring to a statement number and returns a list of a single object, referring to the statements that fulfill the Follows* relationship with the specified statement. If there are none, returns an empty list.

11. LIST<ENTITYTUPLE> GetPreviousT(STRING stmt)
* Description: Receives a string referring to a statement number and returns a list of a single object, referring to the statements that fulfill the FollowedBy* relationship with the specified statement. If there are none, returns an empty list.

12. LIST<ENTITYTUPLE> GetParentT(STRING stmt)
* Description: Receives a string referring to a statement number and returns a list of objects, each referring to transitive child statements of the specified statement. If there are none, returns an empty list.

13. LIST<ENTITYTUPLE> GetChildT(STRING stmt)
* Description: Receives a string referring to a statement number and returns a list of a single object, referring to transitive parent statement of the specified statement. If there are none, returns an empty list.

14. VECTOR<ASSIGNENTITY> GetPatternByAssign(STRING stmt)
* Description: Receives a string referring to a statement number and returns a vector of a single AssignEntity referring to that statement if one exists, otherwise returns an empty vector.

15. VECTOR<ASSIGNENTITY> GetPatternByVariable(STRING var)
* Description: Receives a string referring to a variable name and returns a vector of multiple AssignEntity objects referring to assign statements that include the variable in the LHS, if any, otherwise returns an empty vector.

2. BOOLEAN HasFollows(STRING stmt)
    * Description: Returns true if there is any statement after the specified statement number, otherwise returns false.

3. BOOLEAN HasPrevious(STRING stmt)
    * Description: Returns true if there is any statement before the specified statement number, otherwise returns false.

4. BOOLEAN HasParent(STRING stmt)
    * Description: Returns true if the specified statement has any children, otherwise returns false.

5. BOOLEAN HasChild(STRING stmt)
    * Description: Returns true if the specified statement has any parents, otherwise returns false.

6. BOOLEAN HasUses(STRING stmt)
    * Description: Returns true if the specified statement uses any variables, otherwise returns false.

7. BOOLEAN HasUsedBy(STRING var)
    * Description: Returns true if the specified variable is used by any statements, otherwise returns false.

8. BOOLEAN HasModifies(STRING stmt)
    * Description: Returns true if the specified statement modifies any variables, otherwise returns false.

9. BOOLEAN HasModifiedBy(STRING var)
    * Description: Returns true if the specified variable is modified by any statement, otherwise returns false.

10. BOOLEAN HasFollowsT(STRING stmt)
* Description: Returns true if the specified statement fulfills the Follows* relationship with any other variable, otherwise returns false.

11. BOOLEAN HasPreviousT(STRING stmt)
* Description: Returns true if the specified statement fulfills the FollowedBy* relationship with any other variable, otherwise returns false.

12. BOOLEAN HasParentT(STRING stmt)
* Description: Returns true if the specified statement fulfills the Parent* relationship with any other variable, otherwise returns false.

13. BOOLEAN HasChildT(STRING stmt)
* Description: Returns true if the specified statement fulfills the Child* relationship with any other variable, otherwise returns false.