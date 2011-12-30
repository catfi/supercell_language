function print<T, ...>(arg:T, rest:...)
{
    system.print(arg);
    print(rest);
}

function print<T>(arg:T)
{
    system.print(arg);
}

function print_raw<...>(rest:...)
{
    system.print(rest);
}
