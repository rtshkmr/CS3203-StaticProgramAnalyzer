string = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34"
new_string = ""
for value in string.split(" "):
    new_string += value + ", "
final_string = new_string[ : -2]
print(final_string)