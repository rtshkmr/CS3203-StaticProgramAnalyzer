This file exists to coordinate the systems testing discussion. General workflow will be that we will have a set of
SIMPLE programs and one or more sets of PQL queries for that.

Once front and backend teams give a green light, we shall add that set of text files for systems testing w autotester.


-------

### 1_SourceCode.txt

```javascript

procedure p {
    x = z * 3 + 2 * y;
    i = i - 1;
    x = x + 1;
    z = x + z;
    z = 1;
    z = z + x + i;
    z = z - z + 2 * z % z;
}
```

Testing Targets:

* Entity Creation without any container logic
* Simple assignments
* Operator precedence in parsing

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


