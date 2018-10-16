int my_main()
{
    int a = 1;
    {
        int a = 2;
        {
            int a = 3;
        }
        return a;
    }
}
