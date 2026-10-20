__kernel void mainKernel(
    __global int *numbers,
    const int size
)
{
    int id = get_global_id(0);
    int sum = numbers[id] + numbers[id + (size / 2)];
    numbers[id] = sum;
    return;
}