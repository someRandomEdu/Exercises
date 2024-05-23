auto findDigits = [](int n) -> int
{
    int result = 0;
    int number = n;
    
    do
    {
        int digit = number % 10;
        number /= 10;
        
        if (digit > 0 && n % digit == 0)
        {
            ++result;
        }
    }
    while (number > 0);
    
    return result;
};
