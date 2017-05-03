#include "CatmullRomCurveEvaluator.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"
#include "modelerapp.h"
#include "modelerglobals.h"

#define SEGMENT 30

void CatmullRomCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap,
										 const bool& bAdaptive) const
{
	ptvEvaluatedCurvePts.clear();

	std::vector<Point> controlPointsCopy;

	if (bWrap)
	{
		Point start_p1 = Point((ptvCtrlPts.end() - 2)->x - fAniLength, 
			(ptvCtrlPts.end() - 2)->y);
		Point start_p2 = Point((ptvCtrlPts.end() - 1)->x - fAniLength,
			(ptvCtrlPts.end() -1)->y);
		Point end_p1 = Point((ptvCtrlPts.begin())->x + fAniLength, 
			(ptvCtrlPts.begin())->y);
		Point end_p2 = Point((ptvCtrlPts.begin()+1)->x + fAniLength,
			(ptvCtrlPts.begin()+1)->y);
		controlPointsCopy.push_back(start_p1);
		controlPointsCopy.push_back(start_p2);
		controlPointsCopy.insert(controlPointsCopy.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		controlPointsCopy.push_back(end_p1);
		controlPointsCopy.push_back(end_p2);
	}
	else 
	{
		controlPointsCopy.push_back(ptvCtrlPts.front());
		controlPointsCopy.insert(controlPointsCopy.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		controlPointsCopy.push_back(ptvCtrlPts.back());
	}

	const Mat4d basis = Mat4d(
		-1, 3, -3, 1,
		2, -5, 4, -1,
		-1, 0, 1, 0,
		0, 2, 0, 0)*0.5;

	for (int i = 0; i < controlPointsCopy.size() - 3; ++i)
	{
		Vec4d param_x = Vec4d(
			controlPointsCopy[i].x, 
			controlPointsCopy[i+1].x,
			controlPointsCopy[i+2].x,
			controlPointsCopy[i+3].x);
		Vec4d param_y = Vec4d(
			controlPointsCopy[i].y,
			controlPointsCopy[i+1].y,
			controlPointsCopy[i+2].y,
			controlPointsCopy[i+3].y);
		for (int t = 0; t < SEGMENT; ++t)
		{
			double time = (double)t/(double)SEGMENT;
			Vec4d param_time = Vec4d(time*time*time, time*time, time, 1);
			Point eval_point = Point(param_time * basis * param_x, param_time * basis * param_y);
			if (ptvEvaluatedCurvePts.empty() || eval_point.x > ptvEvaluatedCurvePts.back().x)
			{
				ptvEvaluatedCurvePts.push_back(eval_point);
			}
		}
	}
	if (!bWrap)
	{
		if (ptvCtrlPts.front().x < ptvEvaluatedCurvePts.front().x)
		{
			ptvEvaluatedCurvePts.push_back(ptvCtrlPts.front());
		}
		if (ptvCtrlPts.back().x > ptvEvaluatedCurvePts.back().x)
		{
			ptvEvaluatedCurvePts.push_back(ptvCtrlPts.back());
		}
		ptvEvaluatedCurvePts.push_back(Point(0, ptvEvaluatedCurvePts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvEvaluatedCurvePts.back().y));
	}
}
