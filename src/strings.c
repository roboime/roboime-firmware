

int remove_espacos_multiplos(char* buffer, int size)
{
    int i, j = 0;
    for (i = 0; i < size; i++)
    {
        if (buffer[j - ((j == 0) ? 0 : 1)] == ' ' && buffer[i] == ' ')
        {
        }
        else
        {
            buffer[j++] = buffer[i];
        }
    }
    if (j && buffer[j - 1] == ' ')
    {
        j--;
    }
    buffer[j] = 0;
    return j;
}
