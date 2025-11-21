local mod = require "my_mod"
assert(mod)
assert(mod.member1 == false)
assert(mod.member2 == "a string!")
assert(mod.nonexistent == nil)
assert(mod.func(34.5) == 69)
assert(mod.sub.nested == 35)
