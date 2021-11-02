import random

RANDOM_EXPRESSION_VALUE = 1
RANDOM_EXPRESSION_MIN_VALUE = 1
RANDOM_EXPRESSION_MAX_VALUE = 30
RANDOM_CONDITIONAL_VALUE = 1
MAX_NESTING_LEVEL = 4

number_of_statements = 100
variable_list = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l"]

#print random statement
def printRandStmt(level_of_nesting):
    if (random.randint(0, 2) == 0 and level_of_nesting < MAX_NESTING_LEVEL):
        return stmt_list[random.randint(0, 4)](level_of_nesting + 1)
    else:
        return stmt_list[random.randint(0, 2)](level_of_nesting + 1)
# types of statements
# print
# read
#assign
#while
#if
#call(?)
def printStmt(nesting_level):
    var = random.choice(variable_list)
    return lineIndentation(nesting_level) + "print " + var + ";\n"

def readStmt(nesting_level):
    var = random.choice(variable_list)
    return lineIndentation(nesting_level) + "read " + var + ";\n"

def assignStmt(nesting_level):
    lhs_variable = random.choice(variable_list)
    rhs_expression = random_expression()
    return lineIndentation(nesting_level) + lhs_variable + " = " + rhs_expression + ";\n"

def random_expression():
    return str(randomExpression(RANDOM_EXPRESSION_VALUE))

def whileStmt(nesting_level):
    whileStmtString = lineIndentation(nesting_level)
    whileStmtString += ("while (")
    whileStmtString += (randomConditional(RANDOM_CONDITIONAL_VALUE))
    whileStmtString += (") {\n")
    for i in range(random.randint(1, 10)):
        whileStmtString += lineIndentation(nesting_level + 1) + printRandStmt(nesting_level + 1)
        whileStmtString += "\n"
    whileStmtString += ("}\n")
    return whileStmtString

def ifStmt(nesting_level):
    ifStmtString = lineIndentation(nesting_level)
    ifStmtString += ("if (")
    ifStmtString += (randomConditional(RANDOM_CONDITIONAL_VALUE))
    ifStmtString += (") then {\n")
    for i in range(random.randint(1, 10)):
        ifStmtString += lineIndentation(nesting_level + 1)
        ifStmtString += printRandStmt(nesting_level + 1) + "\n"
    ifStmtString += lineIndentation(nesting_level) + ("} else {") + "\n"
    for i in range(random.randint(1, 10)):
        ifStmtString += lineIndentation(nesting_level + 1)
        ifStmtString += printRandStmt(nesting_level + 1) + "\n"
    ifStmtString += lineIndentation(nesting_level) + "}"
    return ifStmtString

class Expression:
    pass

class Number(Expression):
    def __init__(self, num):
        self.num = num

    def __str__(self):
        return str(self.num)

class Variable(Expression):
    def __init__(self, var):
        self.var = var

    def __str__(self):
        return str(self.var)

class BinaryExpression(Expression):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

    def __str__(self):
        return str(self.left) + " " + self.op + " "  + str(self.right)

class ParenthesizedExpression(Expression):
    def __init__(self, exp):
        self.exp = exp

    def __str__(self):
        return "(" + str(self.exp) + ")"

def randomExpression(prob):
    p = random.random()
    if p > prob:
        if random.randint(0, 1) == 0:
            return Number(random.randint(RANDOM_EXPRESSION_MIN_VALUE, RANDOM_EXPRESSION_MAX_VALUE))
        else:
            randVar = random.choice(variable_list)
            return Variable(randVar)
    elif random.randint(0, 1) == 0:
        return ParenthesizedExpression(randomExpression(prob / 1.3))
    else:
        left = randomExpression(prob / 1.2)
        op = random.choice(["+", "-", "*", "/", "%"])
        right = randomExpression(prob / 1.2)
        return BinaryExpression(left, op, right)

def randomConditional(prob):
    if random.random() > prob:
        return randomRelExpr()
    else:
        case = random.randint(0, 2)
        if (case == 0):
            return "! (" + randomConditional(prob/1.2) + ")"
        elif (case == 1):
            return "(" + randomConditional(prob/1.6) + ") && (" + randomConditional(prob/1.6) + ")"
        else:
            return "(" + randomConditional(prob/1.6) + ") || (" + randomConditional(prob/1.6) + ")"

def randomRelExpr():
    lhs = random.choice(variable_list) if random.randint(0, 1) == 0 else str(random.randint(RANDOM_EXPRESSION_MIN_VALUE, RANDOM_EXPRESSION_MAX_VALUE))
    rhs = random.choice(variable_list) if random.randint(0, 1) == 0 else str(random.randint(RANDOM_EXPRESSION_MIN_VALUE, RANDOM_EXPRESSION_MAX_VALUE))
    op = random.choice(["==", "!=", ">", ">=", "<", "<="])
    return lhs + op + rhs

def lineIndentation(nesting_level):
    return_string = ""
    for i in range(nesting_level):
        return_string += "  "
    return return_string

stmt_list = [printStmt, readStmt, assignStmt, whileStmt, ifStmt]

file = open("random_simple_prog.txt", 'w')
for i in range(100):
    file.write(printRandStmt(0))

file.close()
