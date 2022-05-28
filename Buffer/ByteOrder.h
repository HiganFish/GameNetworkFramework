#pragma once

#include <climits>

static bool IsLittleEndian()
{
    union Foo
    {
        int i1 = 0x12345678;
        char c1;
    };
	constexpr Foo foo{};
    return foo.c1 == 0x78;
}

template <typename T>
T swap_endian(T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

template <typename T>
T ntoh(T u)
{
	static bool is_little_endian = IsLittleEndian();
    return is_little_endian ? swap_endian(u) : u;
}

template <typename T>
T hton(T u)
{
	static bool is_little_endian = IsLittleEndian();
    return is_little_endian ? swap_endian(u) : u;
}