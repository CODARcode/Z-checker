#include <stdio.h>
#include <stdlib.h>
#include "ZC_quicksort.h"
#include "zc.h"

size_t ZC_divide(RateDistElem* list,size_t start,size_t end)
{
	size_t i,j,pivot;
	RateDistElem temp = NULL; //temp is a pointer

	i = start;
	j = end;
	pivot = end;

	while(i<j)
	{
		while(list[i]->psnr <= list[pivot]->psnr && i < end)
			i++;

		while(list[j]->psnr > list[pivot]->psnr)
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

size_t ZC_divide3(RateDistElem* list,size_t start,size_t end)
{
	size_t i,j,pivot;
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

size_t ZC_divide_vis_CR(ZC_CompareData** list,size_t start,size_t end)
{
	size_t i,j,pivot;
	ZC_CompareData* temp = NULL; //temp is a pointer

	i = start;
	j = end;
	pivot = end;

	while(i<j)
	{
		while(list[i]->compressRatio <= list[pivot]->compressRatio && i < end)
			i++;

		while(list[j]->compressRatio > list[pivot]->compressRatio)
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

void ZC_quick_sort(RateDistElem* list,size_t start,size_t end)
{
	size_t mid;

	if(start < end)
	{
		mid = ZC_divide(list,start,end);
		ZC_quick_sort(list,start,mid-1);
		ZC_quick_sort(list,mid+1,end);
	}
}

size_t ZC_divide2(StringElem* list,size_t start,size_t end)
{
	size_t i,j,pivot;
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

void ZC_quick_sort2(StringElem* list,size_t start,size_t end)
{
	size_t mid;

	if(start < end)
	{
		mid = ZC_divide2(list,start,end);
		ZC_quick_sort2(list,start,mid-1);
		ZC_quick_sort2(list,mid+1,end);
	}
}

void ZC_quick_sort_vis_CR(ZC_CompareData** list,size_t start,size_t end)
{
	size_t mid;
	if(start < end)
	{
		mid = ZC_divide_vis_CR(list,start,end);
		ZC_quick_sort_vis_CR(list,start,mid-1);
		ZC_quick_sort_vis_CR(list,mid+1,end);
	}	
}
