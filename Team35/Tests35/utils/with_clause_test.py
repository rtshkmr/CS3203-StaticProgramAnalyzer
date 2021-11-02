import random

attrRef = ["pr1.procName", "c1.procName", "v1.varName", "r1.varName", "p1.varName", "const.value", "s1.stmt#", "r1.stmt#", "p1.stmt#", "c1.stmt#", "w1.stmt#", "if1.stmt#", "a1.stmt#"]
declaration = "stmt s1, s2; assign a1, a2; call c1, c2; print p1, p2; read r1, r2; procedure pr1, pr2; while w1, w2; if if1, if2; prog_line pl1, pl2; variable v1, v2; constant const1, const2; "
variable_list = ["x", "First", "noSuchVariable", ""]
integer_list = ["-1", "0", "1", "100"]

count = 1

def get_synonym_cross():
    total_query = ""
    for attr in attrRef:
        for attr2 in attrRef:
            global count
            query = str(count) + " - synonym cross\n"
            query += declaration + "\n"
            query += "Select s1 with " + attr + " = " + attr2 + "\n"
            query += "none\n"
            query += "5000\n"
            total_query += query
            count += 1
    return total_query

def get_attrRef_variable_cross():
    total_query = ""
    for attr in attrRef:
        for var in variable_list:
            global count
            query = str(count) + " - attrRef-variable pair\n"
            query += declaration + "\n"
            if (random.randint(0, 1) == 0):
                query += "Select s1 with " + attr + " = \"" + var + "\"\n"
            else:
                query += "Select s1 with \"" + var + "\" = " + attr + "\n"
            query += "none\n"
            query += "5000\n"
            total_query += query
            count += 1
    return total_query

def get_attrRef_integer_cross():
    total_query = ""
    for attr in attrRef:
        for int_value in integer_list:
            global count
            query = str(count) + " - attrRef-interger pair\n"
            query += declaration + "\n"
            if (random.randint(0, 1) == 0):
                query += "Select s1 with " + attr + " = " + int_value + "\n"
            else:
                query += "Select s1 with " + int_value + " = " + attr + "\n"
            query += "none\n"
            query += "5000\n"
            total_query += query
            count += 1
    return total_query

def get_int_cross():
    total_query = ""
    for int1 in integer_list:
        for int2 in integer_list:
            global count
            query = str(count) + " - int cross\n"
            query += declaration + "\n"
            query += "Select BOOLEAN with " + int1 + " = " + int2 + "\n"
            query += "none\n"
            query += "5000\n"
            total_query += query
            count += 1
    return total_query

def get_var_cross():
    total_query = ""
    for var1 in variable_list:
        for var2 in variable_list:
            global count
            query = str(count) + " - variable cross\n"
            query += declaration + "\n"
            query += "Select BOOLEAN with \"" + var1 + "\" = \"" + var2 + "\"\n"
            query += "none\n"
            query += "5000\n"
            total_query += query
            count += 1
    return total_query

def get_var_int_cross():
    global count
    query = str(count) + " - variable-int pair\n"
    query += declaration + "\n"
    query += "Select BOOLEAN with " + "1" + " = " + "\"x\"" + "\n"
    query += "none\n"
    query += "5000\n"
    return query
    count += 1

file = open("with_clause_test_queries.txt", 'w')
file.write(get_synonym_cross())
file.write(get_attrRef_integer_cross())
file.write(get_attrRef_variable_cross())
file.write(get_int_cross())
file.write(get_var_cross())
file.write(get_var_int_cross())

file.close()