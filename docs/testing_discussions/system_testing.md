# Systems Testing Workspace


[![hackmd-github-sync-badge](https://hackmd.io/GCrfsq1BQ7mzzDCoLLLwMg/badge)](https://hackmd.io/GCrfsq1BQ7mzzDCoLLLwMg)   

This file exists to coordinate the systems testing discussion. General workflow will be that we will have a set of SIMPLE programs and one or more sets of PQL queries for that.

Once front and backend teams give a green light, we shall add that set of text files for systems testing w autotester.


-------

### Source-Query pair 1
#### STATUS: Pending Greenlight


### test_source_1.txt

```javascript

procedure
p
{
    x = z * 3 + 2 * y;
    print
    me;
    i = i - 1;
    x = x + 1;
    z = x + z;
    z = 1;
    read
    me;
    z = z + x + i;
    z = z - z + 2 * z % z;
}
```


### test_queries_1.txt



```javascript
1 - Simple statement
stmt s;
Select s
1, 2, 3, 4, 5, 6, 7, 8, 9
5000
2 - Simple assign
assign a;
Select a
1, 3, 4, 5, 6, 8, 9
5000
3 - Simple print
print p;
Select p
2
5000
4 - Simple read
read r;
Select r
7
5000
5 - Simple variable
variable v;
Select v
x, y, z, i
5000
6 - Multiple declarations (same line)
assign a1, a2;
Select a1
1, 3, 4, 5, 6, 7, 8, 9
5000
7 - Single follows clause
assign a; print p;
Select a such that Follows(a, p)
1
5000
8 - Single follows clause
assign a1, a2;
Select a1 such that Follows(a1, a2)
1
5000
9 - Follows with one wildcard (first)
assign a1, a2;
Select a2 such that Follows(_, a2)
4, 5, 6, 9
5000
10 - Follows with two wildcard (both)
assign a1, a2;
Select a1 such that Follows(_, _)
1, 3, 4, 5, 6, 8, 9
5000
11 - Follows with one wildcard (second) with target in group
assign a1, a2;
Select a1 such that Follows(a1, _)
1, 3, 4, 5, 6, 8
5000
12 - Follows with one wildcard (second) with target not in group
assign a1, a2;
Select a1 such that Follows(a2, _)
1, 3, 4, 5, 6, 8, 9
5000
13 - Parent with 2 wildcard but Simple program has no such relationship
assign a1, a2;
Select a1 such that Parent(_, _)
None
5000
14 - Pattern query with both wildcard
assign a1, a2;
Select a1 pattern a1 (_, _)
1, 3, 4, 5, 6, 8, 9
5000

```


### Testing Targets:


#### SOURCE
* Entity Creation without any container logic
* Simple assignments
* Operator precedence in parsing

#### QUERY
* simple select statements 
* Follows query - wildcard included 
* Pattern query - wildcard included 

-------

## Source-Query Pair 2

### test_source_2.txt

```javascript

procedure
Bumblebee
{
    read
    x;
    y = 2 * z;
    z = p - y;
    if (q != 1) then
    {
        z = x + 24;
    }
else
    {
        while (z > 0) {
            y = x * q - 5;
            z = z - 1;
        }
        y = x + y * z + p * q;
    }
    i = x + j + z;
}


```

Testing Targets:

* Container nesting with 1 inner layer of container nesting
* Conditional expressions are basic for current level of validation

------

```javascript

procedure Bumblebee {
    read
    x;
    y = 2 * z;
    z = p - y;

    while (z > 0) {
        while ((nice + 1) > 12) {
            y = x * q - 5;
            while (x < 69) {
                z = z - 1;
                while (sleepTime > 0) {
                    while ((bananas * cherries) == 0) {

                        y = x + y * z + p * q;
                    }
                }
            }
            i = x + j + z;
        }
    }
```

Testing Targets:

* Deep level nesting of while loop (no branches)

------

## Template for Simple Program:

-------

### test_source_<sourcefile_number>.txt
Make sure that sourcefile_numbers are consecutive

```javascript

<input source file here>


```

Testing Targets:

* List a bunch of goals that the test serves

------

## Template for PQL query:

-------

### test_queries_<sourcefile_number>.txt

follow the naming as
per [this guide](https://github.com/nus-cs3203/project-wiki/wiki/Autotester-Integration-and-Testing#12-format-of-pql-queries--its-results)
on how the autotester integration works

```javascript

<insert queries here that folow th>

```

Testing Targets:

* List a bunch of goals that the test serves

------


