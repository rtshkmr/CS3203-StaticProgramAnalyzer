## Source Processor Abstract API

### Important Definitions
* **Source Statement**:  SIMPLE code surrounded by two `;` symbols or by `{` and `;` (first statement)
* **Token** (format): should at least contain the original symbol and some tokenTag for categorisation
* **Deliverables**: A wrapper object that contains the EntityTables, RelationshipTables that will be stored in the `PKB`
* **Entity**: Design entities such as stmt as listed in the SPA requirements

### List of Components
1. SourceProcessor
2. Parser
3. PSubsystem
4. Tokenizer
5. SyntaxValidator
6. EntityFactory
7. DesignExtractor

### SourceProcessor
**Overview**: `SourceProcessor` manages the data flow between the `Parser`, `DesignExtractor` and the `PKB`. It holds the necessary data structures(Deliverables) for preprocessing of source code and populates the `PKB` when the preprocessing is done.
#### API
1. PKB* ProcessSourceFile(STRING sourceFilePath)
   * Description: Processes the source file at the path given, and returns a PKB pointer to the PKB that is populated with the necessary data.
2. VOID Terminate(STRING message)
   * Description: Terminates Parser execution and logger, and exits program.

### Parser
**Overview**: `Parser` is a facade that takes in the source program from the `SourceProcessor`,
checks the syntax, creates the necessary data structures and sends these Deliverables back.
#### API
1. VOID Parse(STRING sourceFilePath)
   * Description: Parses the source file at the path given, by calling the `PSubsystem`.

2. DELIVERABLE* GetDeliverables()
   * Description: Wraps the necessary data structures in a Deliverable object and sends the pointer of the Deliverable to the SourceProcessor.

### PSubsystem
**Overview**: `PSubsystem` contains data structures that accumulate(EntityTables, RelationshipTables, Helper Stacks that keep track of tokens and determine how the Deliverables are being modified).
It receives a single Source Statement from the `Parser` and initiates a pipeline for processing it: the statement is
tokenized then its syntax is validated, entity objects are created and relationships are added.
#### API
1. VOID InitDataStructures()
   * Description: Initializes the data structures for the Deliverable and helper stacks.
2. VOID ProcessStatement(STRING stmt)
   * Description: Tokenizes the statement, validates its concrete syntax and creates Entities and relationships that will then be added to the Deliverable.
3. DELIVERABLE* GetDeliverables()
   * Description: Wraps the necessary data structures in a Deliverable object and sends the Deliverable to the Parser.

### Tokenizer
**Overview**: `Tokenizer` is responsible for converting the string format of a Source Statement into a list of lexical tokens, as defined in the Concrete Grammar Syntax.
#### API
1. VECTOR\<TOKENS\> CreateTokens(STRING stmt)
   * Description: Creates tokens out of the statement according to the Syntax.

### SyntaxValidator
**Overview**: `SyntaxValidator` checks whether the tokenized statement follows the Concrete Grammar Syntax.
#### API
1. BOOL ValidateSyntax(VECTOR\<TOKENS\> tokens)
   * Description: Validates the syntax of the vector of tokens.

### EntityFactory
**Overview**: `EntityFactory` is responsible for creating the Entity objects from the tokenized statement provided. Entity is an Abstract class.
#### API
1. ENTITY* CreateEntities(VECTOR\<TOKENS\> tokens)
   * Description: Creates Entities based on the statement represented by the vector of tokens passed in.

### DesignExtractor
**Overview**: Extracts program design abstractions that cannot be extracted in the one pass of parsing of the source code, namely the transitive relationships.
#### API
1. VOID ExtractDesignAbstractions()
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
