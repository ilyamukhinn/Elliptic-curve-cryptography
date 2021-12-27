#ifndef POINT_CLASS_HPP
#define POINT_CLASS_HPP

#include <vector>
#include <limits.h>

#define INF LLONG_MAX

class Point
{
public:
	long long X;
	long long Y;
	long long p;
	int a;
	int b;

	Point Sum(long long X1, long long Y1, long long X2, long long Y2);
	long long Get_Invert(long long x);
	long long Bin_pow(long long base, long long p, long long m);
	Point k_mul_Point(long long k);

	Point(long long _p, int _a, int _b, long long _X, long long _Y)
	{
		p = _p;
		a = _a;
		b = _b;
		X = _X;
		Y = _Y;
	}
	Point operator=(const Point& P)
	{
		X = P.X;
		Y = P.Y;
		p = P.p;
		a = P.a;
		b = P.b;

		return *this;
	}
	Point operator+(Point P)
	{
		return Sum(this->X, this->Y, P.X, P.Y);
	}
	Point operator+=(Point P)
	{
		return Sum(this->X, this->Y, P.X, P.Y);
	}
	Point operator*(int k)
	{
		return k_mul_Point(k);
	}
	bool operator!=(const Point& P)
	{
		return this->X != P.X || this->Y != P.Y;
	}
	bool operator==(const Point& P)
	{
		return this->X == P.X && this->Y == P.Y;
	}
	friend bool operator<(const Point& P1, const Point& P2)
	{
		return P1.X < P2.X ? true : P1.X > P2.X ? false : P1.Y < P2.Y ? true : false;
	}
	~Point() {}
};

#endif // !POINT_CLASS_HPP


