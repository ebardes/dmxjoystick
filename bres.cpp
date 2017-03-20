#include "mac.h"

/*
 * A variation of the Bresenham Line Algotithm.
 * {@see https://en.wikipedia.org/wiki/Bresenham's_line_algorithm}
 *
 * Conventions:
 *   x1 = start time = always 0
 *   x2 = end time
 *
 *   y1 = current value
 *   y2 = target value
 */
void fade::init(int y1,int x2,int y2)
{
	const int x1 = 0;
	int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;

	target = y2;

	dx=x2-x1;
	dy=y2-y1;
	dx1=abs(dx);
	dy1=abs(dy);
	px=2*dy1-dx1;
	py=2*dx1-dy1;

	if (dx1 > npoints)
	{
		if (points)
			delete[] points;
		points = (dx1 > 0) ? new int[dx1 + 1] : NULL;
		npoints = dx1;
	}

	if(dy1<=dx1)
	{
		if(dx>=0)
		{
			x=x1;
			y=y1;
			xe=x2;
		}
		else
		{
			x=x2;
			y=y2;
			xe=x1;
		}
		points[x] = y;
		for(i=0;x<xe;i++)
		{
			x=x+1;
			if(px<0)
			{
				px=px+2*dy1;
			}
			else
			{
				if((dx<0 && dy<0) || (dx>0 && dy>0))
				{
					y=y+1;
				}
				else
				{
					y=y-1;
				}
				px=px+2*(dy1-dx1);
			}
			points[x] = y;
		}
	}
	else
	{
		if(dy>=0)
		{
			x=x1;
			y=y1;
			ye=y2;
		}
		else
		{
			x=x2;
			y=y2;
			ye=y1;
		}
		points[x] = y;
		for(i=0;y<ye;i++)
		{
			y=y+1;
			if(py<=0)
			{
				py=py+2*dx1;
			}
			else
			{
				if((dx<0 && dy<0) || (dx>0 && dy>0))
				{
					x=x+1;
				}
				else
				{
					x=x-1;
				}
				py=py+2*(dx1-dy1);
			}
			points[x] = y;
		}
	}
	fading = true;
}

fade::~fade()
{
	if (points)
		delete[] points;
}

int fade::tick(void)
{
	if (p < npoints)
	{
		return points[p++];
	}
	else
	{
		fading = false;
		return target;
	}
}
