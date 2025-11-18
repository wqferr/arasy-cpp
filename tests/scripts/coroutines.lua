local function a()
    for i = 1, 5 do
        coroutine.yield(i)
    end
    return "abc", "def"
end

local function b(arg)
    Var1 = arg
    Var2 = coroutine.yield("a", 1)
    Var3 = coroutine.yield "b"
    return "c"
end

Co1 = coroutine.create(a)
Co2 = coroutine.create(b)
