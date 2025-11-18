local function a()
    for i = 1, 5 do
        local a = (nil)[3]
        coroutine.yield(i)
    end
    return "abc", "def"
end

local function b()
    coroutine.yield "a"
    Var1 = true
    coroutine.yield "b"
    Var2 = true
    return "c"
end

Co1 = coroutine.create(a)
Co2 = coroutine.create(b)
