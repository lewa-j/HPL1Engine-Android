/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "impl/PhysicsJointHingeNewton.h"

#include "system/LowLevelSystem.h"

#include "impl/PhysicsBodyNewton.h"
#include "impl/PhysicsWorldNewton.h"

#include "math/Math.h"

namespace hpl {

	static cMatrixf GetMatrixFromPinAndPivot(const cVector3f& avPinDir, const cVector3f& avPivot);

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cPhysicsJointHingeNewton::cPhysicsJointHingeNewton(const tString &asName,
		iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
		iPhysicsWorld *apWorld,const cVector3f &avPivotPoint, const cVector3f &avPinDir)
		: iPhysicsJointNewton<iPhysicsJointHinge>(asName,apParentBody,apChildBody,apWorld,avPivotPoint)
	{
		mvPinDir = avPinDir * -1;
		mvPivotPoint = avPivotPoint;

		mfMaxAngle = 0;
		mfMinAngle = 0;

		mfPreviousAngle =0;

		mlMaxDOF = 6;
		mpNewtonJoint = NewtonConstraintCreateUserJoint(mpNewtonWorld, mlMaxDOF, StaticSubmitConstraints, mpNewtonChildBody, mpNewtonParentBody);
		NewtonJointSetUserData(mpNewtonJoint, this);

		cMatrixf mtxPinAndPivot = GetMatrixFromPinAndPivot(mvPinDir, mvPivotPoint);
		CalculateLocalMatrix(mtxPinAndPivot, m_mtxLocalPinPivot0, m_mtxLocalPinPivot1);
	}

	//-----------------------------------------------------------------------

	cPhysicsJointHingeNewton::~cPhysicsJointHingeNewton()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cPhysicsJointHingeNewton::SetMaxAngle(float afAngle)
	{
		mfMaxAngle = afAngle;
	}
	void cPhysicsJointHingeNewton::SetMinAngle(float afAngle)
	{
		mfMinAngle = afAngle;
	}
	float cPhysicsJointHingeNewton::GetMaxAngle()
	{
		return mfMaxAngle;
	}
	float cPhysicsJointHingeNewton::GetMinAngle()
	{
		return mfMinAngle;
	}

	//-----------------------------------------------------------------------

	cVector3f cPhysicsJointHingeNewton::GetVelocity()
	{
		return cVector3f(0,0,0);
	}
	cVector3f cPhysicsJointHingeNewton::GetAngularVelocity()
	{
		if (mpParentBody)
		{
			cVector3f vAngularVel0 = mpChildBody->GetAngularVelocity();
			cVector3f vAngularVel1 = mpParentBody->GetAngularVelocity();

			return GetPinDir() * cMath::Vector3Dot(vAngularVel0 - vAngularVel1, GetPinDir());
		}
		else
		{
			return GetPinDir() * cMath::Vector3Dot(mpChildBody->GetAngularVelocity(), GetPinDir());
		}
	}

	//-----------------------------------------------------------------------

	cVector3f cPhysicsJointHingeNewton::GetForce()
	{
		cVector3f vForce(0);
		//Only get for the linear rows!
		for (int i = 0; i < 3; ++i)
			vForce.v[i] = NewtonUserJointGetRowForce(mpNewtonJoint, i);
		return vForce;
	}

	//-----------------------------------------------------------------------

	float cPhysicsJointHingeNewton::GetDistance()
	{
		return 0;
	}
	float cPhysicsJointHingeNewton::GetAngle()
	{
		cMatrixf mtxPinPivot0;
		cMatrixf mtxPinPivot1;

		CalculateGlobalMatrix(m_mtxLocalPinPivot0, m_mtxLocalPinPivot1, mtxPinPivot0, mtxPinPivot1);

		cMatrixf mtxPinPivot0Inv = mtxPinPivot0.GetTranspose();
		cMatrixf mtxPinPivot1Inv = mtxPinPivot1.GetTranspose();

		float fSinAngle = cMath::Vector3Dot(cMath::Vector3Cross(mtxPinPivot0Inv.GetRight(), mtxPinPivot1Inv.GetRight()),
			mtxPinPivot0Inv.GetUp());
		float fCosAngle = cMath::Vector3Dot(mtxPinPivot0Inv.GetRight(), mtxPinPivot1Inv.GetRight());
		float fAngle = atan2(fSinAngle, fCosAngle);

		return fAngle;
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// STATIC CALLBACKS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMatrixf GetMatrixFromPinAndPivot(const cVector3f& avPinDir, const cVector3f& avPivot)
	{
		cMatrixf mtxPinAndPivot = cMatrixf::Identity;
		cVector3f vUp = cMath::Vector3Normalize(avPinDir);
		cVector3f vTemp = cMath::Vector3Normalize(cVector3f(1) - vUp);
		if (vTemp == vUp) vTemp = cMath::Vector3Normalize(vTemp * cVector3f(1.5f, 0.5f, 2.0f)); //Make sure all does not become wierd in the special case.

		cVector3f vRight = cMath::Vector3Normalize(cMath::Vector3Cross(vUp, vTemp));
		cVector3f vForward = cMath::Vector3Normalize(cMath::Vector3Cross(vUp, vRight));

		mtxPinAndPivot.SetUp(vUp);
		mtxPinAndPivot.SetRight(vRight);
		mtxPinAndPivot.SetForward(vForward);
		mtxPinAndPivot = mtxPinAndPivot.GetTranspose();

		mtxPinAndPivot.SetTranslation(avPivot);

		return mtxPinAndPivot;
	}

	//-----------------------------------------------------------------------

	void cPhysicsJointHingeNewton::CalculateGlobalMatrix(const cMatrixf& a_mtxLocalMatrix0, const cMatrixf& a_mtxLocalMatrix1, cMatrixf& a_mtxMatrix0, cMatrixf& a_mtxMatrix1)
	{
		cMatrixf mtxMatrix0 = this->mpChildBody->GetLocalMatrix();
		cMatrixf mtxMatrix1 = this->mpParentBody ? this->mpParentBody->GetLocalMatrix() : cMatrixf::Identity;

		a_mtxMatrix0 = cMath::MatrixMul(mtxMatrix0, a_mtxLocalMatrix0);
		a_mtxMatrix1 = cMath::MatrixMul(mtxMatrix1, a_mtxLocalMatrix1);
	}

	//-----------------------------------------------------------------------

	void cPhysicsJointHingeNewton::CalculateLocalMatrix(const cMatrixf& a_mtxPinsAndPivotFrame, cMatrixf& a_mtxLocalMatrix0, cMatrixf& a_mtxLocalMatrix1)
	{
		cMatrixf mtxMatrix0 = this->mpChildBody->GetLocalMatrix();
		cMatrixf mtxMatrix1 = this->mpParentBody ? this->mpParentBody->GetLocalMatrix() : cMatrixf::Identity;

		// calculate the relative matrix of the pin and pivot on each body
		a_mtxLocalMatrix0 = cMath::MatrixMul(cMath::MatrixInverse(mtxMatrix0), a_mtxPinsAndPivotFrame);
		a_mtxLocalMatrix1 = cMath::MatrixMul(cMath::MatrixInverse(mtxMatrix1), a_mtxPinsAndPivotFrame);
	}

	//-----------------------------------------------------------------------

	void cPhysicsJointHingeNewton::SubmitConstraints(dFloat afTimestep, int alThreadIndex)
	{
		//OnPhysicsUpdate();

		cMatrixf mtxPinPivot0;
		cMatrixf mtxPinPivot1;

		// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
		CalculateGlobalMatrix(m_mtxLocalPinPivot0, m_mtxLocalPinPivot1, mtxPinPivot0, mtxPinPivot1);

		cVector3f vPinPivot0Pos = mtxPinPivot0.GetTranslation();
		cVector3f vPinPivot1Pos = mtxPinPivot1.GetTranslation();

		cMatrixf mtxPinPivot0Inv = mtxPinPivot0.GetTranspose();
		cMatrixf mtxPinPivot1Inv = mtxPinPivot1.GetTranspose();

		// Restrict the movement on the pivot point along all tree orthonormal direction
		NewtonUserJointAddLinearRow(mpNewtonJoint, vPinPivot0Pos.v, vPinPivot1Pos.v, mtxPinPivot0Inv.GetRight().v);
		NewtonUserJointAddLinearRow(mpNewtonJoint, vPinPivot0Pos.v, vPinPivot1Pos.v, mtxPinPivot0Inv.GetUp().v);
		NewtonUserJointAddLinearRow(mpNewtonJoint, vPinPivot0Pos.v, vPinPivot1Pos.v, mtxPinPivot0Inv.GetForward().v);

		// get a point along the pin axis at some reasonable large distance from the pivot
		cVector3f vQ0(vPinPivot0Pos + mtxPinPivot0Inv.GetUp() * 50.0f);
		cVector3f vQ1(vPinPivot1Pos + mtxPinPivot1Inv.GetUp() * 50.0f);

		// two constraints row perpendicular to the pin vector
		NewtonUserJointAddLinearRow(mpNewtonJoint, vQ0.v, vQ1.v, mtxPinPivot0Inv.GetRight().v);
		NewtonUserJointAddLinearRow(mpNewtonJoint, vQ0.v, vQ1.v, mtxPinPivot0Inv.GetForward().v);

		//////////////////////////
		//Check limits
		if (mfMinAngle != 0 || mfMaxAngle != 0)
		{
			/////////////////////////7
			// Get Angle
			//  the joint angle can be determine by getting the angle between any two non parallel vectors
			float fSinAngle = cMath::Vector3Dot(cMath::Vector3Cross(mtxPinPivot0Inv.GetRight(), mtxPinPivot1Inv.GetRight()),
				mtxPinPivot0Inv.GetUp());
			float fCosAngle = cMath::Vector3Dot(mtxPinPivot0Inv.GetRight(), mtxPinPivot1Inv.GetRight());
			float fAngle = atan2(fSinAngle, fCosAngle);

			///////////////////////////
			//Avoid oscillation
			CheckLimitAutoSleep(this, mfMinAngle, mfMaxAngle, fAngle);

			bool bSkipLimitCheck = false;
			if (std::abs(mfPreviousAngle - fAngle) > cMath::ToRad(300)) bSkipLimitCheck = true;

			///////////////
			//Min
			if (fAngle < mfMinAngle && bSkipLimitCheck == false)
			{
				OnMinLimit();

				float fRelAngle = fAngle - mfMinAngle;

				// tell joint error will minimize the exceeded angle error
				NewtonUserJointAddAngularRow(mpNewtonJoint, fRelAngle, mtxPinPivot0Inv.GetUp().v);

				// need high stiffness here
				NewtonUserJointSetRowStiffness(mpNewtonJoint, 1.0f);

				// allow the joint to move back freely 
				NewtonUserJointSetRowMaximumFriction(mpNewtonJoint, 0.0f);
			}
			///////////////
			//Max
			else if (fAngle > mfMaxAngle && bSkipLimitCheck == false)
			{
				OnMaxLimit();

				float fRelAngle = fAngle - mfMaxAngle;

				// tell joint error will minimize the exceeded angle error
				NewtonUserJointAddAngularRow(mpNewtonJoint, fRelAngle, mtxPinPivot0Inv.GetUp().v);

				// need high stiffness here
				NewtonUserJointSetRowStiffness(mpNewtonJoint, 1.0f);

				// allow the joint to move back freely
				NewtonUserJointSetRowMinimumFriction(mpNewtonJoint, 0.0f);
			}
			else
			{
				if (mpParentBody == NULL || mpParentBody->GetMass() == 0)
				{
					/*if (bSkipLimitCheck == false &&
						((mfStickyMaxDistance != 0 && mfPreviousAngle > mfMaxAngle) ||
							(mfStickyMinDistance != 0 && mfPreviousAngle < mfMinAngle)))
					{
						mpChildBody->SetAngularVelocity(0);
						mpChildBody->SetLinearVelocity(0);
					}*/

					if(	(mfStickyMaxDistance != 0 && fabs(fAngle - mfMaxAngle) < mfStickyMaxDistance)
						||
						(mfStickyMinDistance != 0 && fabs(fAngle - mfMinAngle) < mfStickyMinDistance)
						)
					{
						mpChildBody->SetAngularVelocity(0);
						mpChildBody->SetLinearVelocity(0);
					}
				}

				OnNoLimit();
			}

			mfPreviousAngle = fAngle;
		}
	}

	void cPhysicsJointHingeNewton::StaticSubmitConstraints(const NewtonJoint* apJoint, dFloat afTimestep, int alThreadIndex)
	{
		cPhysicsJointHingeNewton* pJointData = (cPhysicsJointHingeNewton*)NewtonJointGetUserData(apJoint);

		pJointData->SubmitConstraints(afTimestep, alThreadIndex);
	}

	//-----------------------------------------------------------------------

}
