#include <stdio.h>
#include <stdlib.h>
#include <zc.h>

int ZC_divide(RateDistElem* list,int start,int end)
{
	int i,j,k,pivot;
	RateDistElem temp = NULL; //temp is a pointer

	i = start;
	j = end;
	pivot = end;

	while(i<j)
	{
		while(list[i]->rate <= list[pivot]->rate && i < end)
			i++;

		while(list[j]->rate > list[pivot]->rate)
			j--;

		if(i<j)
		{
			temp = list[i];
			list[i] = list[j];
			list[j] = temp;
		}
	}

	temp = list[j];
	list[j] = list[pivot];
	list[pivot] = temp;

	return j;
}

void ZC_quick_sort(RateDistElem* list,int start,int end)
{
	int mid;

	if(start < end)
	{
		mid = ZC_divide(list,start,end);
		ZC_quick_sort(list,start,mid-1);
		ZC_quick_sort(list,mid+1,end);
	}
}

int ZC_divide2(StringElem* list,int start,int end)
{
	int i,j,k,pivot;
	StringElem temp = NULL; //temp is a pointer

	i = start;
	j = end;
	pivot = end;

	while(i<j)
	{
		while(list[i]->value >= list[pivot]->value && i < end)
			i++;

		while(list[j]->value < list[pivot]->value)
			j--;

		if(i<j)
		{
			temp = list[i];
			list[i] = list[j];
			list[j] = temp;
		}
	}

	temp = list[j];
	list[j] = list[pivot];
	list[pivot] = temp;

	return j;
}

void ZC_quick_sort2(StringElem* list,int start,int end)
{
	int mid;

	if(start < end)
	{
		mid = ZC_divide2(list,start,end);
		ZC_quick_sort2(list,start,mid-1);
		ZC_quick_sort2(list,mid+1,end);
	}
}
