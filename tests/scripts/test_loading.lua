local a, b, c = ...
assert(type(a) == "table", "a is not a table")
a["field"] = 3

assert(type(b) == "number" and b == 0.5, "b is not a number")
assert(c == nil, "c is not nil")

return a, 5
