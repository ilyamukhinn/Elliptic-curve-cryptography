#include "Point Class.hpp"

Point Point::Sum(long long X1, long long Y1, long long X2, long long Y2)
{
	long long alpha = 0;
	long long X3 = 0;
	long long Y3 = 0;

	if (X2 == INF && Y2 == INF)
	{
		X3 = X1;
		Y3 = Y1;
	}
	else if (X1 == X2 && Y1 == Y2 && Y1 != 0) {
		alpha = (3 * X1 * X1 + a) * Get_Invert(2 * Y1) % p;
		alpha = alpha >= 0 ? alpha : alpha + p;

		X3 = (alpha * alpha - 2 * X1) % p;
		X3 = X3 >= 0 ? X3 : X3 + p;

		Y3 = (alpha * (X1 - X3) - Y1) % p;
		Y3 = Y3 >= 0 ? Y3 : Y3 + p;
	}
	else if (X1 != X2) {
		alpha = (Y2 - Y1) * Get_Invert(X2 - X1) % p;
		alpha = alpha >= 0 ? alpha : alpha + p;

		X3 = (alpha * alpha - X1 - X2) % p;
		X3 = X3 >= 0 ? X3 : X3 + p;

		Y3 = (alpha * (X1 - X3) - Y1) % p;
		Y3 = Y3 >= 0 ? Y3 : Y3 + p;
	}
	else
	{
		X3 = INF;
		Y3 = INF;
	}

	return Point(p, a, b, X3, Y3);
}

long long Point::Get_Invert(long long x)
{
	return Bin_pow(x, p - 2, p);
}

long long Point::Bin_pow(long long base, long long p, long long m)
{
	if (p == 1)
	{
		return base;
	}

	if (p % 2 == 0)
	{
		long long t = Bin_pow(base, p / 2, m);
		return (t * t) % m;
	}
	else
	{
		return (Bin_pow(base, p - 1, m) * base) % m;
	}
}

Point Point::k_mul_Point(long long k)
{
	Point P({ p, a, b, INF, INF });
	Point T({ p, a, b, this->X, this->Y });

	while (k) {
		if (k & 1) {
			if (P.X != INF && P.Y != INF) {
				P = P + T;
			}
			else {
				P = T;
			}
		}

		T = T + T;
		k = k >> 1;
	}

	return P;
}
