/*
T DivideAndConquer(P)
{
	if(P可以直接解决)
	{
		T <- P的结果;
		return T;
	}

	将P分解为子问题 {P1, P2,..., Pn};
	for_each(Pi : {P1, P2,..., Pn})
	{
		ti <- DivideAndConquer(Pi); //递归解决子问题Pi
	}
	T <- Merge(t1, t2,...,tn); //合并子问题的解

	return T;
}
*/

void exchange(int *arElem, int m, int n)
{
	int tmp = arElem[m];
	arElem[m] = arElem[n];
	arElem[n] = tmp;
}

int partion(int *arElem, int left, int right)
{
	int x = arElem[right];
	int i = left;
	for (int j = left; j < right; j++)
	{
		if (arElem[j] < x)
		{
			if (i != j)
				exchange(arElem, i, j);
			i++;
		}
	}
	exchange(arElem, i, right);
	return i;
}

int partion2(int *arElem, int left, int right)
{
	int ind = right;
	int value = arElem[ind];
	while (left < right)
	{
		int i = left;
		int j = right;

		while (arElem[i] <= value && i <= j)
			i++;
		if (i > j)
			return j;
		left = i;

		while (arElem[j] >= value && j >= i)
			j--;
		if (j < i)
			return i;
		right = j;

		exchange(arElem, left, right);
	}
}

void quick_sort(int *arElem, int left, int right)
{
	if (left < right)
	{
		int mid = partion(arElem, left, right);
		quick_sort(arElem, left, mid - 1);
		quick_sort(arElem, mid + 1, right);
	}
}

void Quick_Sort(int *arElem, int n)
{
	quick_sort(arElem, 0, n - 1);
}

int main()
{
	return 0;
}
