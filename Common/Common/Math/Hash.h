#pragma once

#define BKDR_SEED 131

// BKDR Hash Function
inline __int64  BKDRHash(char *str)
{
	__int64 seed = BKDR_SEED;			// 31 131 1313 13131 131313 etc..
	__int64 hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFFFFFFFFFF);
}

// AP Hash Function
inline unsigned int  APHash(char *str)
{
	unsigned int hash = 0;
	int i;

	for (i = 0; *str; i++)
	{
		if ((i & 1) == 0)
		{
			hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
		}
		else
		{
			hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
		}
	}

	return (hash & 0x7FFFFFFF);
}

