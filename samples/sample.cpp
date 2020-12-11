int main()
{
    return 0;
}

int anotherFunction(int x, int y)
{
    return x + y;
}

int *escapingFunction()
{
    int x = 12;
    return &x;
}

int *nonEscapingFunction(int *p)
{
    int y = 12;
    y = *p;
    return p;
}

int *definitelyNonEscapingFunction(int *p)
{
    return p;
}
