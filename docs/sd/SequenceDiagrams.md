# Sequence Diagrams

[![hackmd-github-sync-badge](https://hackmd.io/eAaJDS6-RDWTPFKD78FH-A/badge)](https://hackmd.io/eAaJDS6-RDWTPFKD78FH-A)


### ControllerToDesignExtSequence.mmd

    sequenceDiagram
        autonumber
        Controller ->>+ DesignExtractor: extractRelationships(List<HashTable> relationships, List<Node<Statement>> statementList, List<Node<Variable>> variableList)
        activate DesignExtractor

        Note right of Controller: List<HashTable> relationships includes the following hashtables: <br/> Follows::HashTable<Node<Statement>, Node<Statement>> [Complete]<br/> FollowedBy::HashTable<Node<Statement>, Node<Statement>> [Complete]<br/> Uses::HashTable<Node<Statement>, List<Node<Variable>>> [Partial] <br/> UsedBy::HashTable<Node<Variable>, List<Node<Statement>>> [Partial] <br/> Modifies::HashTable<Node<Statement>, List<Node<Variable>>> [Partial]<br/> ModifiedBy::HashTable<Node<Variable>, List<Node<Statement>>>[Partial] <br/> ParentOf::HashTable<Node<Statement>, List<Node<Statement>>>[Complete] <br/> ChildOf::HashTable<Node<Statement>, List<Node<Statement>>>[Complete]

        DesignExtractor ->> DesignExtractor: getTransitiveUses()

        DesignExtractor ->> DesignExtractor: getTransitiveUsedBy()

        DesignExtractor ->> DesignExtractor: getTransitiveModifies()

        DesignExtractor ->> DesignExtractor: getTransitiveModifiedBy()

        DesignExtractor ->> DesignExtractor: getParentStarOf()

        DesignExtractor ->> DesignExtractor: getChildStarOf()

        DesignExtractor -->>+ Controller: List<HashTable> relationships
        deactivate DesignExtractor
        Note right of Controller: List<HashTable> relationships includes the following hashtables:<br/> Uses::HashTable<Node<Statement>, List<Node<Variable>>> [Complete] <br/> UsedBy::HashTable<Node<Variable>, List<Node<Statement>>> [Complete] <br/> Modifies::HashTable<Node<Statement>, List<Node<Variable>>> [Complete]<br/> ModifiedBy::HashTable<Node<Variable>, List<Node<Statement>>>[Complete] <br/> ParentStarOf::HashTable<Node<Statement>, List<Node<Statement>>>[Complete] <br/> ChildStarOf::HashTable<Node<Statement>, List<Node<Statement>>>[Complete]

        Controller ->> PKB: populatePKB(List<HashTable> relationships, List<Node<Statement>> statementList, List<Node<Variable>> variableList)
        
### ParserToControllerSequence_old.mmd

    sequenceDiagram
        autonumber
        participant AT as AutoTester
        participant P as Parser
        %% subsystem creates nodes and storing AST while the lines are being read 
        %% Psystem contains statement stacks.. 
        participant PSub as PSubsystem
        participant Tokenizer
        participant CGV as ConcreteGrammarValidator
        participant C as Controller

        %% parser ===> parses the entire program ==> controller 
        %% parser ===> parses the entire program ==> controller 

        %% Assumption: This sd only focus on parsing %
        AT ->>+ P: parse(sourcefile)

            %% Assumption: P will check correctness while building tree %

            P ->>+ PSub: createNewTree()
            PSub -->>- P: initStatus::IDENTIFIER

            %% statement here refers to the source code in between two semicolons
            %% If validState == false in reportParseStatus(Failure), the loop terminates
            loop hasNextStmt() && validState == True

            P ->>+ PSub: sendStatement(stmt)
            PSub ->>+ Tokenizer: createTokens(stmt)

            Tokenizer ->>+ CGV: checkForValidSyntax(List<TOKEN>) 

            opt any close brackets
            CGV ->>PSub: configStackFlags()
            end

            CGV -->>- Tokenizer: reportValidationStatus(Boolean)

            %% Entity node refers to WHILE/IF/ASSIGN
            %% nb: might need to add in a separate node creator here because there will be multiple types of ENTITY_NODE and having an interface here would be better
            %% a token looks like: (<symbol>, <tokenTag>) brace_open brace_close
            Tokenizer -->>- PSub: ENTITY_NODE      

                alt ENTITY_NODE INVALID
                    PSub -->> P: reportParseStatus(INVALID)
                else
                    %% entity is an Abstract/Parent class here, 
                    %% addRelationships modifies the existing DataStructures we have 
                    %% TODO: discuss the logic behind this 
                    PSub ->> PSub: addRelationships(ENTITY_NODE) 
                    PSub -->>- P: reportParseStatus(VALID)
                end
            end 

            P -->> AT: programStatus::IDENTIFIER
            P -)+ PSub: terminate()
            PSub ->>-P: sendDeliverables(AST tree, hashtable, entitytable)
            P->>-C: setDataStructures(AST tree, hashtable, entitytable)
            %% PSub ->>- C: sendDeliverables(AST tree, hashtable, entitytable)

            %% Transitive => Parents*, Children* Follows*, FollowedBy* Use, UsedBy, Modifies, ModifiesBy

            %% By right, AST should not be used for resolving queries (anything, we try to add aux Data Structures)

            %% This program ends when preprocessing is finish %% 
            
### SourceProcessorToPkbSequence.mmd

    sequenceDiagram
        autonumber
        participant AutoTester
        participant Parser
        participant Controller
        participant DesignExtr
        participant PKB

        %% Assumption: This sd only focus on parsing %
        AutoTester ->> Parser: parse(sourcefile)

            % Assumption: Parser will check correctness while building tree %

            Parser ->> Parser: createNewTree()

            %% todo: extract into separate sequence diagram, representing interactions within the Parser %%
            loop hasNextStatement()

            Parser -->> Parser: check_create_node() 
            %%Refer to parser_nodes.mmd for the above statement
            Parser -->> Parser: get_relationships_in_line() 

            loop whileHasRelationship()
                Parser -->> Controller: addRelationship(Node n1, Node n2, RSType rs) 
            end
            Parser -->> Controller: addEntity(Node n)

            end

            Parser ->> Controller: sendAST(AST tree)

            %% Transitive => Parents*, Children* Follows*, FollowedBy* Use, UsedBy, Modifies, ModifiesBy
            Controller ->> DesignExtr: extractTransitiveRelationships(List<HashTable> relationships, List<Node> entities)
            DesignExtr -->> Controller: return

            Controller ->> PKB: storeAST_HashTable_Entity(AST tree, List<HashTable> ht, List<Node> entities)
            %% By right, AST should not be used for resolving queries (anything, we try to add aux Data Structures)
        Parser -->> AutoTester: parseStatus::int
        %% This program ends when preprocessing is finish %% 

### SourceProcessorToPkbSequence_detailed.mmd

    sequenceDiagram
        autonumber
        participant AT as AutoTester
        participant SP as SourceProcessor
        participant P as Parser
        %% subsystem creates nodes and storing AST while the lines are being read
        %% Psystem contains statement stacks..
        participant PSub as PSubsystem
        participant Tkn as Tokenizer
        participant CGV as ConcreteGrammarValidator
        %% EntityFactory modifies necessary DSes (creates nodes in AST, add entries into relationship, entity tables ...)
        %% To prevent the passing of multiple different objects, there's going to be a bi-directional relationship b/w EF and PSub where PSub will read the values of DSes and Helper Stacks kept inside PSub, and EF will be creating and modifying said objects within PSub. This might need an intermediary Relationship Class or smth to avoid circular dependency
        participant EF as EntityFactory
        participant DE as DesignExtractor
        participant PKB as ProgramKnowledgeBase

        AT->>+SP: processSourceFile(sourceFile)
        SP ->>+ P: parse(sourcefile)

        %% initialises the necessary data structures inside PSub, these DSes (AST, RelationshipTables, EntityTables) will be accumulated
        %% Assumption: P will check correctness while building DSes %
        P ->>+ PSub: initDataStructures()
        PSub -->>- P: initStatus::IDENTIFIER

        %% statement here refers to the source code in between two semicolons
        %% If validState == false in reportParseStatus(Failure), the loop terminates
        loop hasNextStmt && validState
            P ->>+ PSub: processStatement(stmt)
            PSub ->>+ Tkn: createTokens(stmt)
            %% a token looks like: (<symbol>, <tokenTag>) brace_open brace_close
            Tkn -->>- PSub: tokens::List<TOKEN>

            %% We ask the validator to validate the syntax for a single statement, if there's an error at any token, it should fail early and return us the validationStatus
            %% SideEffect:Since the validator looks at tokens one by one, we let the validator modify "helper stacks" that are in the PSub
            %% "Helper Stacks" are cumulative stacks or any other DSes kept in the PSubsystem. e.g.
            %% "Helper Stacks" are there to help keep track of tokens and determine how the DSes are being modified. e.g. if CGV current sees an open brace, it knows that it's a new container, hence the PSub's helper stack should be notified such that the AST will branch out, the relationship tables will create new entries...
            PSub ->>+ CGV: validateConcreteSyntax(List<TOKEN>)
            CGV -->>-PSub: validationStatus::IDENTIFIER

            %% create Entities:  Entity here refers to the Abstraction Entity as described by the wiki/lecture 1 slides
            PSub ->>+EF:createEntities(List<Tokens>)
            %% This method has responsibilities such as adding necessary Entity Nodes to the AST and updating the necessary tables to reflect relationships (this is where we add info about relationships b/w lines, vars...)
            %% Entity node refers to WHILE/IF/ASSIGN... ref to
            EF-->>-PSub:completionStatus::IDENTIFIER

            PSub -->>-P: statementProcessStatus::IDENTIFIER
            %% todo: handle the error handling opt code here
        end
        %% at this point, all the statements have been processed, the parser is ready for deliverables to be sent to SP
        P ->>+PSub: sendDeliverables()
        PSub -->>-P: deliverables::DELIVERABLES

        P -->>-SP: parseStatus::IDENTIFIER

        SP ->>+P: sendDeliverables()
        P -->>-SP: deliverables::DELIVERABLES

        %% todo: add in DE activities
        SP ->>+DE:extractDesigns()
        %% DE and SP will comms with each other bidirectionally
        %% todo: these sequences will be fleshed out in a separate sequence diagram
        DE -->>-SP:extractionStatus::IDENTIFIER


        alt valid sourceProcessStatus
            SP ->>+PKB: populateDataStructures()
            PKB -->>-SP:pkbSuccessStatus::IDENTIFIER
        else
            SP ->>SP : handleSourceCodeError()
        end

        SP -->>- AT: sourceProcessStatus::IDENTIFIER

        %% Transitive => Parents*, Children* Follows*, FollowedBy* Use, UsedBy, Modifies, ModifiesBy
        %% By right, AST should not be used for resolving queries (anything, we try to add aux Data Structures)
        %% This sequence of steps ends when the source processing is done and deliverables sent to PKB %%

### queries_overview.mmd
    sequenceDiagram
        autonumber
        participant AutoTester
        participant QuerySystemController
        participant QueryExtractor
        participant QueryEvaluator
        participant QueryProjector

        %% This sd is for per query (PQL statement) basis.
        AutoTester ->> QuerySystemController: Evaluate(query, pkb)
        QuerySystemController ->> QueryExtractor: ExtractQuery()
        Note right of QueryExtractor: Ref <br/> SD_Extract_Query
        QueryExtractor-->>QuerySystemController: Populate query objects
        QuerySystemController->>QueryEvaluator: EvaluateQuery()
        Note right of QueryEvaluator: Ref <br/> SD_Evaluate_Query
        QueryEvaluator-->>QuerySystemController: list<string> result_list
        QuerySystemController->>QueryProjector: FormatQuery(result_list)
        QueryProjector-->>QuerySystemController: list<string> populated_result_list
        QuerySystemController-->>AutoTester: list<string> populated_result_list
        loop every item in populated_result_list
            AutoTester->>AutoTester: add item to results
        end
        
### query_extractor.mmd
    sequenceDiagram
        autonumber
        participant QueryExtractor
        participant QueryParser
        participant QueryTokenizer
        participant QueryValidator
        participant QueryOptimizer

        QueryExtractor->>QueryTokenizer: init()
        QueryTokenizer-->>QueryExtractor: QueryTokenizer tokenizer
        QueryExtractor->>QueryParser: init(list<Clause> clauses, list<Group> groups, list<Synonym> synonyms, Synonym target, tokenizer)
        QueryParser-->>QueryExtractor: QueryParser parser
        QueryExtractor->>QueryParser: parser.Parse()
        Note right of QueryParser: Calls multiple handlers according to <br> grammar rules (recursive descent)
        loop is expecting tokens based on grammar rules
            QueryParser->>QueryTokenizer: GetNextToken()
            QueryTokenizer-->>QueryParser: Token t
            Note right of QueryParser: First layer of validations
            opt is relRef
                Note right of QueryParser: Additional layer of validations
                QueryParser->>QueryValidator: Is_Semantically_Valid_RelRef()
                QueryValidator-->>QueryParser: Boolean b
            end
        end
        QueryParser-->>QueryExtractor: 
        QueryExtractor->>QueryOptimizer: GroupClauses(clauses, groups, target)
        QueryOptimizer-->>QueryExtractor: 

### queries.mmd (deprecated)

    sequenceDiagram
        autonumber
        participant AutoTester
        participant PqlPreprocessor
        participant QueryEvaluator
        participant PKB
        participant QueryResultProjector
        participant UI

        %% This sd is for per query (PQL statement) basis. 
        AutoTester ->> PqlPreprocessor: evaluate(q)
        PqlPreprocessor->>PqlPreprocessor: tokenizePql(q)
        PqlPreprocessor->>PqlPreprocessor: parsePql(q)
        PqlPreprocessor->>PqlPreprocessor: verifyPql(q)
        PqlPreprocessor->>PqlPreprocessor: orderClauses()
        PqlPreprocessor->>QueryEvaluator: sendQueryStruct(q)

        %% Refer to query_evaluation_pkb.mmd
        note right of PKB: info refers to relevant PSub-structures <br/> (maybe a linked list...) that <br/> the QueryEvaluatorcan use <br/> to resolve the query.
        QueryEvaluator->>PKB: retrieveClauseInfo(clause) 

        %% MergeSets() represents union, intersect, join between the different clauses results. 
        QueryEvaluator ->>QueryEvaluator:mergeSets() 
        QueryEvaluator ->>QueryResultProjector: generateResultFormatter(RESULT r)
        QueryResultProjector ->> UI: finalResult(FORMATTED_RESULT fr)

### query_evaluation_pkb.mmd

    sequenceDiagram
        participant QueryPreProcessor
        participant QueryEvaluator
        participant PKB
        participant QueryResultProjector

        % synList - all syn declared
        % e.g assign a; variable v;
        % Select v such that Parent (a, 1) pattern a ("x", "1 + 2")
        QueryPreProcessor->>+QueryEvaluator: QueryStruct(SYNONYM[] synList, SYNONYM target, SUCH_THAT[] stList, PATTERN[] patternList)

        QueryEvaluator->>+PKB: getProcTable()
        PKB-->>-QueryEvaluator: PROC[]
        % querying smts or var is not put into an alt else to illustrate extension for procedure and call queries later on
        loop Every valid synonym declaration
            opt querying statements
                % Note that Stmt can be ‘stmt’ | ‘read’ | ‘print’ | ‘while’ | ‘if’ | ‘assign’
                QueryEvaluator->>+PKB: getStmtTable(STMT_TYPE stmtType)
                PKB-->>-QueryEvaluator: STMT[] listOfStmt, ERR_CODE err
            end
            opt querying variables
                % Note that Var can be ‘variable’ | ‘constant’
                QueryEvaluator->>+PKB: getVarTable(VAR_TYPE varType)
                PKB-->>-QueryEvaluator: STMT[] listOfVar, ERR_CODE err
            end
        end

        % For each such that or pattern
        loop Every such that clause
            opt Select s such that Follows (stmt1, s)
                QueryEvaluator->>+PKB: getFollows (STMT stmt1)
                PKB-->>-QueryEvaluator: STMT clauseResult
            end
            opt Select s such that Follows* (stmt1, s)
                QueryEvaluator->>+PKB: getFollowsStar (STMT stmt1)
                PKB-->>-QueryEvaluator: STMT[] clauseResult
            end
            opt Select s such that Follows (s, stmt1)
                QueryEvaluator->>+PKB: getPrevious (STMT stmt1)
                 PKB-->>-QueryEvaluator: STMT clauseResult
            end
            opt Select s such that Follows* (s, stmt1)
                QueryEvaluator->>+PKB: getPreviousStar (STMT stmt1)
                PKB-->>-QueryEvaluator: STMT[] clauseResult
            end
            opt Select s such that Parent (s, stmt1)
                QueryEvaluator->>+PKB: getParent (STMT stmt1)
                PKB-->>-QueryEvaluator: STMT clauseResult
            end
            opt Select s such that Parent* (s, stmt1)
                QueryEvaluator->>+PKB: getParentStar (STMT stmt1)
                PKB-->>-QueryEvaluator: STMT[] clauseResult
            end
             opt Select s such that Parent (stmt1, s)
                QueryEvaluator->>+PKB: getChild (STMT stmt1)
                PKB-->>-QueryEvaluator: STMT clauseResult
            end
            opt Select s such that Parent* (stmt1, s)
                QueryEvaluator->>+PKB: getChildStar (STMT stmt1)
                PKB-->>-QueryEvaluator: STMT[] clauseResult
            end
            opt Select v such that Uses (stmt1, v)
                QueryEvaluator->>+PKB: getVarUsed (STMT stmt1)
                PKB-->>-QueryEvaluator: VAR clauseResult
            end
            opt Select s such that Uses (s, var1)
                QueryEvaluator->>+PKB: getStmtUsing (VAR var1)
                PKB-->>-QueryEvaluator: STMT[] clauseResult
            end
            opt Select v such that Modifies (stmt1, v)
                QueryEvaluator->>+PKB: getVarModified (STMT stmt1)
                PKB-->>-QueryEvaluator: VAR clauseResult
            end
            opt Select s such that Modifies (s, var1)
                QueryEvaluator->>+PKB: getStmtModifying (VAR var1)
                PKB-->>-QueryEvaluator: STMT[] clauseResult
            end
        end
        opt has >= 1 pattern-cl && notCached
            QueryEvaluator ->> PKB: getAllAssignStmts()
            PKB -->> QueryEvaluator: AST_ASSIGN[] assignStmts
            QueryEvaluator ->> QueryEvaluator: cacheAssignStmts
        end
        loop Each pattern clause
            QueryEvaluator->>QueryEvaluator: STMT[] comparePattern(PATTERN p)
            note right of QueryEvaluator: Case lhs: <br/>if "var_name" (exact match): filter assignStmts<br/>if synonym: variable filter assignStmts<br/>if _ (any match): do nothing
        note right of QueryEvaluator: Case rhs: <br/> if "..." (exact match): filter assignStmts<br/>if _"..."_ (partial match): check AST_ASSIGNS for subtree match first<br/>if _ (any match): do nothing
        end

        % Consider extracting the work of combining clauses to another
        % component.

        % Compute combination of clause results
        opt querying statements
            QueryEvaluator->>+QueryEvaluator: STMT[] ComputeCombinedResult(STMT[][])
        end
        opt querying variables
            QueryEvaluator->>+QueryEvaluator: VAR[] ComputeCombinedResult(VAR[][])
        end

        QueryEvaluator->>QueryResultProjector: STMT[]/VAR[] RESULT
