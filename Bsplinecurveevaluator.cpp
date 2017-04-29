#include "BsplineCurveEvaluator.h"
#include "BezierCurveEvaluator.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"
#include "modelerapp.h"

#define SEGMENT 30

void  BsplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();
	std::vector<Point> controlPointsCopy;
	
	if (bWrap)
	{
		Point start_p1 = Point((ptvCtrlPts.end() - 2)->x - fAniLength,
			(ptvCtrlPts.end() - 2)->y);
		Point start_p2 = Point((ptvCtrlPts.end() - 1)->x - fAniLength,
			(ptvCtrlPts.end() - 1)->y);
		Point end_p1 = Point((ptvCtrlPts.begin())->x + fAniLength, 
			(ptvCtrlPts.begin())->y);
		Point end_p2 = Point((ptvCtrlPts.begin() + 1)->x + fAniLength, 
			(ptvCtrlPts.begin() + 1)->y);
		controlPointsCopy.push_back(start_p1);
		controlPointsCopy.push_back(start_p2);
		controlPointsCopy.insert(controlPointsCopy.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		controlPointsCopy.push_back(end_p1);
		controlPointsCopy.push_back(end_p2);
	}
	else
	{
		controlPointsCopy.push_back(ptvCtrlPts.front());
		controlPointsCopy.push_back(ptvCtrlPts.front());
		controlPointsCopy.insert(controlPointsCopy.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		controlPointsCopy.push_back(ptvCtrlPts.back());
		controlPointsCopy.push_back(ptvCtrlPts.back());
	}


	const Mat4d convert = Mat4d(
		1, 4, 1, 0, 
		0, 4, 2, 0,
		0, 2, 4, 0,
		0, 1, 4, 1)/6.0;

	BezierCurveEvaluator helper = BezierCurveEvaluator();
	for (int i = 0; i < controlPointsCopy.size() - 3; ++i)
	{
		Vec4d param_x(controlPointsCopy[i].x, 
			controlPointsCopy[i+1].x, 
			controlPointsCopy[i+2].x, 
			controlPointsCopy[i+3].x);
		Vec4d param_y(controlPointsCopy[i].y, 
			controlPointsCopy[i+1].y, 
			controlPointsCopy[i+2].y, 
			controlPointsCopy[i+3].y);

		Vec4d convert_x = convert * param_x;
		Vec4d convert_y = convert * param_y;
		std::vector<Point> convertBezierPts;
		for (int j = 0; j < 4; ++j)
		{
			convertBezierPts.push_back(Point(convert_x[j], convert_y[j]));
		}
		std::vector<Point> input_pts;
		helper.evaluateCurve(convertBezierPts, input_pts, fAniLength, false);
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.end(), input_pts.begin(), input_pts.end());
	}
	if (!bWrap) 
	{
		ptvEvaluatedCurvePts.push_back(Point(0, ptvCtrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));
	}
}