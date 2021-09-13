This file exists to coordinate the systems testing discussion. General workflow will be that we will have a set of
SIMPLE programs and one or more sets of PQL queries for that.

Once front and backend teams give a green light, we shall add that set of text files for systems testing w autotester.


-------

### 1_SourceCode.txt

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

Testing Targets:

* Entity Creation without any container logic
* Simple assignments
* Operator precedence in parsing

------

-------

### 2_SourceCode.txt

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

### <sourcefile_number>SourceCode.txt

```javascript

<input source file here>


```

Testing Targets:

* List a bunch of goals that the test serves

------

## Template for PQL query:

-------

### <pql_query_file_number>Queries.txt

follow the naming as
per [this guide](https://github.com/nus-cs3203/project-wiki/wiki/Autotester-Integration-and-Testing#12-format-of-pql-queries--its-results)
on how the autotester integration works

```javascript

<insert queries here that folow th>

```

Testing Targets:

* List a bunch of goals that the test serves

------


