__kernel void mainKernel(
    __global int *bufferA,
    __global int *bufferB,
    const int size,
    __global int *bufferOut
)
{
    int index = get_global_id(0);

    if (index < size/2)
    {
        int element = 0;
        while(element < size/2 && bufferB[element] < bufferA[index])
        {
            element++;
        }
        bufferOut[element + index] = bufferA[index];
        return;
    }

    index = index % (size / 2);

    int element = 0;
    while(element < size/2 && bufferA[element] < bufferB[index])
    {
        element++;
    }
    bufferOut[element + index] = bufferB[index];
    return;
}