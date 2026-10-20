__kernel void mainKernel(
    __global int *numbers,
    const int size,
    const int isEven
)
{
    int index = get_global_id(0) * 2;
    if (isEven)
        index += 1;
    //swap

    if (index + 1 >= size)
    {
        return;
    }

    if (numbers[index + 1] < numbers[index])
    {
        int aux = numbers[index+1];
        numbers[index+1] = numbers[index];
        numbers[index] = aux;
    }

    return;
}