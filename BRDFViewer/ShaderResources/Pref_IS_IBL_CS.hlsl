
groupshared float smem[256];

RWTexture2D<float> right_PrefEnv;
[numthreads(4, 4, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}