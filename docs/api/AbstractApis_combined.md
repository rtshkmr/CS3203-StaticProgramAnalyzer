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
    * Description: Returns a list of PROC. If the procTable is empty, return an empty list.
2. STMT[] getStmtTable(STMT_TYPE stmtType)
    * Description: Returns a list of STMT with the type corresponding to STMT_TYPE.
      If there are no such types of STMT, return an empty list.
3. VAR[] getVarTable(STMT_TYPE stmtType)
    * Description: Returns a list of STMT with the type corresponding to STMT_TYPE.
      If there are no such types of STMT, return an empty list.
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



## Source Processor Abstract API

### Important Definitions
* **Source Statement**:  SIMPLE code surrounded by two `;` symbols or by `{` and `;` (first statement)
* **Token** (format): should at least contain the original symbol and some tokenTag for categorisation
* **Deliverables**: A wrapper object that contains the AST, EntityTables, RelationshipTables that will be stored in the `PKB`
* **Entity**: Design entities such as stmt as listed in the SPA requirements

### List of Components
1. SourceProcessor
2. Parser
3. PSubsystem
4. Tokenizer
5. ConcreteGrammarValidator
6. EntityFactory
7. DesignExtractor

### SourceProcessor
**Overview**: `SourceProcessor` manages the data flow between the `Parser`, `DesignExtractor` and the `PKB`. It holds the necessary data structures(Deliverables) for preprocessing of source code and populates the `PKB` when the preprocessing is done.
#### API
1. IDENTIFIER processSourceFile(FILE sourceFile)

### Parser
**Overview**: `Parser` is a facade that takes in the source program from the `SourceProcessor`,
checks the syntax, creates the necessary data structures and sends these Deliverables back.
#### API
1. IDENTIFIER parse(FILE sourceFile)

2. IDENTIFIER sendDeliverables()
    * Description: Wraps the necessary data structures in a Deliverables object and sends the Deliverables to the SourceProcessor.

### PSubsystem
**Overview**: `PSubsystem` contains data structures that accumulate(AST, EntityTables, RelationshipTables, Helper Stacks that keep track of tokens and determine how the Deliverables are being modified).
It receives a single Source Statement from the `Parser` and initiates a pipeline for processing it: the statement is
tokenized then its syntax is validated, entity nodes are created and added to the AST, relationships are added.
#### API
1. IDENTIFIER initDataStructures()
    * Description: Initializes the data structures for the Deliverables and helper stacks.
2. IDENTIFIER processStatement(STRING stmt)
    * Description: Tokenizes the statement, validates its concrete syntax and creates Entities and relationships that will then be added to the Deliverables.
3. IDENTIFIER sendDeliverables()
    * Description: Wraps the necessary data structures in a Deliverables object and sends the Deliverables to the Parser.

### Tokenizer
**Overview**: `Tokenizer` is responsible for converting the string format of a Source Statement into a list of lexical tokens, as defined in the Concrete Grammar Syntax.
#### API
1. LIST\<TOKENS\> createTokens(STRING stmt)
    * Description: Creates tokens out of the statement according to the Syntax.

### ConcreteGrammarValidator
**Overview**: `ConcreteGrammarValidator` checks whether the tokenized statement follows the Concrete Grammar Syntax.
#### API
1. IDENTIFIER validateConcreteSyntax(LIST\<TOKENS\> tokens)
    * Description: Validates the syntax of the list of tokens.

### EntityFactory
**Overview**: `EntityFactory` is responsible for creating the Entity nodes for the AST from the tokenized statement provided. Entity can be an Abstract/Parent class.
#### API
1. IDENTIFIER createEntities(LIST\<TOKENS\> tokens)
    * Description: Builds the sub-tree of the AST from the list of tokens that represent a statement.

### DesignExtractor
**Overview**: Extracts program design abstractions that cannot be extracted in the one pass of parsing of the source code, namely the transitive relationships.
#### API
1. IDENTIFIER extractDesignAbstractions()
    * Description: Extracts program design abstractions from the data available after parsing and populates the relevant tables.




Template for Abstract API

### \<Insert Module/Component Name\>
**Overview**: <insert purpose/responsibility of module>
#### API
1. RETURN_TYPE functionName(ARG_TYPE argName)
    * Requires: \<insert precondition\>
    * Description: \<insert description for abstract API call\>
2. RETURN_TYPE functionName(ARG_TYPE argName)
    * Requires: \<insert precondition\>
    * Description: \<insert description for abstract API call\>