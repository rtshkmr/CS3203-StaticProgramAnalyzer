## Source Processor Abstract API

### Important Definitions
* **Source Statement**:  SIMPLE code surrounded by two `;` symbols or by `{` and `;` (first statement)
* **Token** (format): should at least contain the original symbol and some tokenTag for categorisation 

### Parser 
**Overview**: `Parser` is a facade class that takes in the source program from the `AutoTester`, 
creates necessary data structures (AST, EntityTables, RelationshipTables) and sends these deliverables to the `Controller`. 
#### API
1. RETURN_TYPE functionName(ARG_TYPE argName)
   * Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>
2. RETURN_TYPE functionName(ARG_TYPE argName)
   * Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>


### PSubsystem 
**Overview**: `PSubsystem` contains data structures that accumulate(Relationship Stacks, AST, EntityTables, RelationshipTables).
It receives a single Source Statement from the Parser and initiates a pipeline for processing it: the statement is 
tokenized then its syntax is validated, entity nodes are created and added to the AST, relationships are added.

#### API
1. RETURN_TYPE functionName(ARG_TYPE argName)
   * Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>
2. RETURN_TYPE functionName(ARG_TYPE argName)
   * Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>


### Tokenizer
**Overview**: Converts the string format of a Source Statement into a list of lexical tokens, as defined in the Concrete
Grammar Syntax. The tokenizer depends on the `ConcreteGrammarValidator` for validation of these lexical tokens after which
it returns a node representing the statement to the `PSubsystem`.
#### API
1. RETURN_TYPE functionName(ARG_TYPE argName)
   * Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>
2. RETURN_TYPE functionName(ARG_TYPE argName)
   * Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>


### ConcreteGrammarValidator
**Overview**: Checks whether the statement follows the 
Concrete Grammar Syntax. It modifies the stack flags and other 
data structures accumulated in the `PSubsystem` and also reports the 
validity of the statement to the `Tokenizer`

#### API
1. RETURN_TYPE functionName(ARG_TYPE argName)
   * Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>
2. RETURN_TYPE functionName(ARG_TYPE argName)
   * Requires: \<insert precondition\>
   * Description: \<insert description for abstract API call\>


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
