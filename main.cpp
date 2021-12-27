#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <time.h>
#include <algorithm>
#include "Point Class.hpp"

struct Curve
{
	long long p;
	int a;
	int b;
};

std::vector<long long> nPrimes(const int& n); //return first n prime numbers
Curve Generate_Curve(const std::vector<long long>& Primes); //return vector<int> Curve = {p, a, b}
std::vector<Point> Get_Points(const Curve& curve); //return vector<Point> - Generated Curve's points

std::vector<Point> Get_kG(const Curve& curve, const std::vector<Point>& Points,
	std::vector<long long>& Primes); //return sequence of k*G, where
											  //all 1 <= k < q
											  // //q -> q * G = O
											  //G - generation point

void Encryption_function(const char* Open_data_file_name, const Curve& curve, 
	const std::vector<Point>& k_G, Point& Db, const char* Encrypted_data_file_name);
void Decryption_function(const char* Encrypted_data_file_name, 
	const Curve& curve, const long long& Cb, const char* Decrypted_data_file_name);

int main(int argc, char* argv[])
{
	srand(time(0));
	
	std::vector<long long> Primes = nPrimes(300);
	Curve curve = Generate_Curve(Primes);
	std::vector<Point> Points = Get_Points(curve);

	std::wcout << "Elliptic curve ---> y^2 mod " << curve.p 
		<< " = (x^3  + " << curve.a << "*x + " << curve.b << ") mod " << curve.p << std::endl;

	std::vector<Point> k_G = Get_kG(curve, Points, Primes);
	long long q = k_G.size() + 1;
	
	//Getting keys
	long long Ca = rand() % (q - 1) + 1, Cb = rand() % (q - 1) + 1;
	Point Da(k_G[Ca - 1]);
	Point Db(k_G[Cb - 1]);

	Encryption_function(argv[1], curve, k_G, Db, argv[2]);
	Decryption_function(argv[2], curve, Cb, argv[3]);
    
	return 0;
}

std::vector<long long> nPrimes(const int& n)
{
	const int AIM = n;
	const int startSize = AIM; //start size of natural numbers array
	int addSize = AIM; //size of natural numbers additional array

	std::vector<bool> nums(startSize);
	std::vector<long long> primeNums(AIM);

	int foundPrimes = 0;
	for (int i = 2; i < startSize; i++)
	{
		nums[i] = true;
	}

	bool addition = false;

	while (true)
	{
		if (addition)
		{
			nums.resize(nums.size() + addSize, true);

			//excluding composite numbers by primes from the previous iterations
			for (int i = 0; i < foundPrimes; i++)
			{
				int cur_num = primeNums[i];
				if ((addSize + ((nums.size() - addSize) % cur_num)) < cur_num)
				{
					continue;
				}

				for (int j = ((nums.size() - addSize) / cur_num) * cur_num; j < nums.size(); j += cur_num)
				{
					nums[j] = false;
				}
			}
		}
		else
		{
			addition = true;
		}

		long long iter;
		if (foundPrimes == 0)
		{
			iter = 2;
		}
		else
		{
			iter = primeNums[foundPrimes - 1] + 2;
		}

		for (; iter < nums.size(); iter++)
		{
			//choosing the next prime number
			if (nums[iter])
			{
				primeNums[foundPrimes] = iter;
				foundPrimes++;

				if (foundPrimes == AIM)
				{
					break;
				}

				//"sifting"
				for (int j = iter + iter; j < nums.size(); j += iter)
				{
					nums[j] = false;
				}
			}
			else
			{
				continue;
			}
		}

		if (foundPrimes == AIM)
		{
			break;
		}
	}

	return primeNums;
}

Curve Generate_Curve(const std::vector<long long>& Primes)
{
	Curve curve;

	long long p = Primes[rand() % Primes.size()];

	//p has to be greater than 255 for encryt and decrypt bytes of data
	//0 < byte < 255
	while (p < 255)
	{
		p = Primes[rand() % Primes.size()];
	}

	curve.p = p;

	int a = rand() % 51, b = rand() % 51;
	while ((4 * a * a * a + 27 * b * b) % p == 0)
	{
		a = rand() % 51;
		b = rand() % 51;
	}

	curve.a = a;
	curve.b = b;

	return curve;
}

std::vector<Point> Get_Points(const Curve& curve)
{
	std::vector<Point> Points;
	for (long long i = 0; i < curve.p; i++)
	{
		long long y2 = (i * i * i + curve.a * i + curve.b) % curve.p;
		for (long long j = 0; j < curve.p; j++)
		{
			if (j * j % curve.p == y2)
			{
				Points.push_back({ curve.p, curve.a, curve.b, i, j });
			}
		}
	}
	return Points;
}

std::vector<Point> Get_kG(const Curve& curve, const std::vector<Point>& Points, std::vector<long long>& Primes)
{
	Point O({ curve.p, curve.a, curve.b, INF , INF });
	Point G(Points[rand() % Points.size()]);
	long long q = 1;
	Point qG(G);
	std::vector<Point> k_G;

	while (true)
	{
		//find q -> q * G = O (neutral element)
		do
		{
			k_G.push_back(qG);
			q++;
		} while ((qG = G * q) != O);

		//q has to be a prime number, and it's less than p,
		//so we can try to find it in the list of all prime numbers, which we have
		std::vector<long long>::iterator it = find(Primes.begin(), Primes.end(), q); 
		if (it != Primes.end())
		{
			break;
		}

		G = Points[rand() % Points.size()];
		q = 1;
		qG = G;
		k_G.clear();
	}

	//k_G vector contains all possible k * G (1 <= k < q),
	//so we don't need to calculate this k * G during the encryption
	return k_G;
}

void Encryption_function(const char* Open_data_file_name, const Curve& curve, 
	const std::vector<Point>& k_G, Point& Db, const char* Encrypted_data_file_name)
{
	std::ifstream Open_data_file(Open_data_file_name, std::ios_base::binary);
	std::ofstream Encrypted_data_file(Encrypted_data_file_name, std::ios_base::binary);

	std::wcout << std::endl << "Encryption..." << std::endl;

	Point O({ curve.p, curve.a, curve.b, INF , INF });
	long long q = k_G.size() + 1;

	//Vector of all k * Db (1 <= k < q)
	//it will be filled during encryption, 
	//which will allow (after a certain number of iterations) 
	//not to calculate this k * Db, but to select them from the vector
	std::vector<Point> k_Db(q - 1, { curve.p, curve.a, curve.b, INF, INF });
	unsigned ch;

	while ((ch = Open_data_file.get()) != EOF) {
		long long k = 0;
		Point P({ curve.p, curve.a, curve.b, INF, INF });

		do
		{
			k = rand() % (q - 1) + 1;

			if (k_Db[k - 1] != O) {
				P = k_Db[k - 1];
			}
			else {
				P = Db * k;
				k_Db[k - 1] = P;
			}
		} while (P.X == 0);

		Point R(k_G[k - 1]);

		long long e = ch * P.X % curve.p;

		//REWRITE

		//Writing cypher into the output file
		Encrypted_data_file.put(R.X >> 8);
		Encrypted_data_file.put(R.X & 0xff);

		Encrypted_data_file.put(R.Y >> 8);
		Encrypted_data_file.put(R.Y & 0xff);

		Encrypted_data_file.put(e >> 8);
		Encrypted_data_file.put(e & 0xff);
	}

	Open_data_file.close();
	Encrypted_data_file.close();

	std::wcout << std::endl << "Data is encrypted" << std::endl;
}

void Decryption_function(const char* Encrypted_data_file_name, 
	const Curve& curve, const long long& Cb,
	const char* Decrypted_data_file_name)
{
	std::ifstream Encrypted_data_file(Encrypted_data_file_name, std::ios_base::binary);
	std::ofstream Decrypted_data_file(Decrypted_data_file_name, std::ios_base::binary);

	std::wcout << std::endl << "Decryption..." << std::endl;

	//Container of all R and Q matches
	//it will be filled during decryption, 
	//which will allow (after a certain number of iterations) 
	//not to calculate this Q = R * Cb, but to select them from the vector
	std::map<Point, Point> R_Q;
	std::map<Point, Point>::iterator R_Q_it;
	
	while (!Encrypted_data_file.eof()) {
		char* R_e = new char[6];

		Encrypted_data_file.read(R_e, 6);

		//REWRITE

		Point R(curve.p, curve.a, curve.b, (int)(((unsigned char)R_e[0] << 8) | (unsigned char)R_e[1]),
			(int)(((unsigned char)R_e[2] << 8) | (unsigned char)R_e[3]));

		long long e = ((unsigned char)R_e[4] << 8) | (unsigned char)R_e[5];

		Point Q({ curve.p, curve.a, curve.b, INF, INF });

		int m = 0;

		if ((R_Q_it = R_Q.find(R)) != R_Q.end()) {
			m = (e * (R_Q_it->second.Get_Invert(R_Q_it->second.X))) % curve.p;
		}
		else {
			Q = R * Cb;
			R_Q.insert({ R, Q });
			m = (e * Q.Get_Invert(Q.X)) % curve.p;
		}

		Decrypted_data_file.put(m);
	}

	std::wcout << std::endl << "Data is decrypted" << std::endl;
}