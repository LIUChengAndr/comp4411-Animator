#ifndef INCLUDED_C2INTERPOLATING_CURVE_EVALUATOR_H
#define INCLUDED_C2INTERPOLATING_EVALUATOR_H

#pragma warning(disable : 4786)  

#include "CurveEvaluator.h"

//using namespace std;

class C2InterpolatingCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
		std::vector<Point>& ptvEvaluatedCurvePts, 
		const float& fAniLength, 
		const bool& bWrap,
		const bool& bAdaptive) const override;

private:
	/*
	 * Helper function, otherwise to long to write together
	 */
	void _evaluate(const int p1, const int p2, const float& animationLength,
		const std::vector<Point>& controlPointsCopy, std::vector<Point>& evaluatedPoints,
		const std::vector<float>& derivativePoints) const;
	void _evaluateDerivative(std::vector<float>& derivative,
		std::vector<Point> controlPointsCopy, int controlN) const;
};

#endif