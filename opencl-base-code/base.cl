__kernel void mainKernel(
    __global const uchar *image,
    const int width,
    const int isMax,
    __global uchar *imageOut
)
{
    int smallX = get_global_id(0);
    int smallY = get_global_id(1);
    int x = smallX * 2;
    int y = smallY * 2;
    int value;
    if (isMax) 
    {
        value = max(
            max(image[y * width + x], image[y * width + x + 1]),
            max(image[(y + 1) * width + x], image[(y + 1) * width + x + 1])
        );
    }
    else
    {
        value = min(
            min(image[y * width + x], image[y * width + x + 1]),
            min(image[(y + 1) * width + x], image[(y + 1) * width + x + 1])
        );
    }
    imageOut[smallY * width/2 + smallX] = value;
    return;
}