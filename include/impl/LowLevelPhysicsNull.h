
#ifndef HPL_LOWLEVELPHYSICS_NULL_H
#define HPL_LOWLEVELPHYSICS_NULL_H

#include "physics/LowLevelPhysics.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsMaterial.h"
#include "physics/CollideShape.h"
#include "physics/PhysicsBody.h"
#include "physics/CharacterBody.h"
#include "physics/PhysicsJointBall.h"
#include "physics/PhysicsJointHinge.h"
#include "physics/PhysicsJointSlider.h"
#include "physics/PhysicsJointScrew.h"

#include "scene/World3D.h"
#include "scene/PortalContainer.h"

namespace hpl
{

	class cPhysicsMaterialNull : public iPhysicsMaterial
	{
	public:
		cPhysicsMaterialNull(const tString &asName, iPhysicsWorld *apWorld, int alMatId=-1)
		: iPhysicsMaterial(asName,apWorld)
		{
			mlMaterialId = alMatId;
			//Setup default properties
			mFrictionMode = ePhysicsMaterialCombMode_Average;
			mElasticityMode  = ePhysicsMaterialCombMode_Average;
			mfElasticity = 0.5f;
			mfStaticFriction = 0.3f;
			mfKineticFriction = 0.3f;
		}
		~cPhysicsMaterialNull(){}

		void SetElasticity(float afElasticity){mfElasticity=afElasticity;}
		float GetElasticity() const{return mfElasticity;};
		void SetStaticFriction(float afStaticFriction){mfStaticFriction=afStaticFriction;}
		float GetStaticFriction() const{return mfStaticFriction;}
		void SetKineticFriction(float afKineticFriction){mfKineticFriction=afKineticFriction;}
		float GetKineticFriction() const{return mfKineticFriction;}

		void SetFrictionCombMode(ePhysicsMaterialCombMode aMode){mFrictionMode=aMode;}
		ePhysicsMaterialCombMode GetFrictionCombMode() const{return mFrictionMode;}
		void SetElasticityCombMode(ePhysicsMaterialCombMode aMode){mElasticityMode=aMode;}
		ePhysicsMaterialCombMode GetElasticityCombMode() const{return mElasticityMode;}

		void UpdateMaterials(){}

		int GetId(){ return mlMaterialId;}
	private:
		int mlMaterialId;
		ePhysicsMaterialCombMode mFrictionMode;
		ePhysicsMaterialCombMode mElasticityMode;
		float mfElasticity;
		float mfStaticFriction;
		float mfKineticFriction;
	};
	
	class cCollideShapeNull : public iCollideShape
	{
	public:
		cCollideShapeNull(eCollideShapeType aType, const cVector3f &avSize,
							cMatrixf* apOffsetMtx,iPhysicsWorld *apWorld)
			: iCollideShape(apWorld){}
		~cCollideShapeNull(){}

		iCollideShape* GetSubShape(int alIdx){return this;}
		int GetSubShapeNum(){return 1;}

		cVector3f GetInertia(float afMass){return cVector3f();}

		void CreateFromShapeVec(tCollideShapeVec &avShapes){}
		void CreateFromVertices(const unsigned int* apIndexArray, int alIndexNum,
								const float *apVertexArray, int alVtxStride, int alVtxNum){}
	};
	
	class cPhysicsBodyNull : public iPhysicsBody
	{
	public:
		cPhysicsBodyNull(const tString &asName,iPhysicsWorld *apWorld,iCollideShape *apShape)
			: iPhysicsBody(asName,apWorld, apShape)
		{
		//Set default property settings
		mbGravity = true;

		mfMaxLinearSpeed =0;
		mfMaxAngularSpeed =0;
		mfMass =0;

		mfAutoDisableLinearThreshold = 0.01f;
		mfAutoDisableAngularThreshold = 0.01f;
		mlAutoDisableNumSteps = 10;
		}
		~cPhysicsBodyNull(){}

		void SetMaterial(iPhysicsMaterial* apMaterial){mpMaterial = apMaterial;}

		void SetLinearVelocity(const cVector3f &avVel){}
		cVector3f GetLinearVelocity() const{return cVector3f();}
		void SetAngularVelocity(const cVector3f &avVel){}
		cVector3f GetAngularVelocity() const{return cVector3f();}
		void SetLinearDamping(float afDamping){}
		float GetLinearDamping() const{return 0;}
		void SetAngularDamping(float afDamping){}
		float GetAngularDamping() const{return 0;}
		void SetMaxLinearSpeed(float afSpeed){}
		float GetMaxLinearSpeed() const{return 0;}
		void SetMaxAngularSpeed(float afDamping){}
		float GetMaxAngularSpeed() const{return 0;}
		cMatrixf GetInertiaMatrix(){return cMatrixf();}

		void  SetMass(float afMass){mfMass=afMass;}
		float GetMass() const{return mfMass;}
		void  SetMassCentre(const cVector3f& avCentre){}
		cVector3f GetMassCentre() const{return cVector3f();}

		void AddForce(const cVector3f &avForce){}
		void AddForceAtPosition(const cVector3f &avForce, const cVector3f &avPos){}
		void AddTorque(const cVector3f &avTorque){}
		void AddImpulse(const cVector3f &avImpulse){}
		void AddImpulseAtPosition(const cVector3f &avImpulse, const cVector3f &avPos){}

		void SetEnabled(bool abEnabled){}
		bool GetEnabled() const{return false;}
		void SetAutoDisable(bool abEnabled){}
		bool GetAutoDisable() const{return false;}
		void SetAutoDisableLinearThreshold(float afThresold){}
		float GetAutoDisableLinearThreshold() const{return 0;}
		void SetAutoDisableAngularThreshold(float afThresold){}
		float GetAutoDisableAngularThreshold() const{return 0;}
		void SetAutoDisableNumSteps(int alNum){}
		int GetAutoDisableNumSteps() const{return 0;}
		void SetContinuousCollision(bool abOn){}
		bool GetContinuousCollision(){return false;}

		void SetGravity(bool abEnabled){mbGravity=abEnabled;}
		bool GetGravity() const{return mbGravity;}

		void RenderDebugGeometry(iLowLevelGraphics *apLowLevel,const cColor &aColor){}

		void ClearForces(){}

		void DeleteLowLevel(){}
	private:
		//Properties
		bool mbGravity;
		float mfMaxLinearSpeed;
		float mfMaxAngularSpeed;
		float mfMass;
		float mfAutoDisableLinearThreshold;
		float mfAutoDisableAngularThreshold;
		int mlAutoDisableNumSteps;
		// Forces that will be set and clear on update callback
		cVector3f mvTotalForce;
		cVector3f mvTotalTorque;
	};

	class cCharacterBodyNull : public iCharacterBody
	{
	public:
		cCharacterBodyNull(const tString &asName,iPhysicsWorld *apWorld, const cVector3f avSize)
		: iCharacterBody(asName,apWorld, avSize){}
		~cCharacterBodyNull(){}
	};

	class cPhysicsJointBallNull : public iPhysicsJointBall
	{
	public:
		cPhysicsJointBallNull(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
			iPhysicsWorld *apWorld,const cVector3f &avPivotPoint)
			: iPhysicsJointBall(asName, apParentBody, apChildBody, apWorld, avPivotPoint){}
		~cPhysicsJointBallNull(){}
		
		void SetCollideBodies(bool abX){}
		bool GetCollideBodies(){return false;}
		void SetStiffness(float afX){}
		float GetStiffness(){return 0;}
		cVector3f GetVelocity(){return cVector3f();}
		cVector3f GetAngularVelocity(){return cVector3f();}
		cVector3f GetForce(){return cVector3f();}
		float GetDistance(){return 0;}
		float GetAngle(){return 0;}
		
		void SetConeLimits(const cVector3f& avPin, float afMaxConeAngle, float afMaxTwistAngle){}
		cVector3f GetAngles(){return cVector3f();}
	};

	class cPhysicsJointHingeNull : public iPhysicsJointHinge
	{
	public:
		cPhysicsJointHingeNull(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
			iPhysicsWorld *apWorld,const cVector3f &avPivotPoint, const cVector3f avPinDir)
			: iPhysicsJointHinge(asName, apParentBody, apChildBody, apWorld, avPivotPoint){}
		~cPhysicsJointHingeNull(){}
		
		void SetCollideBodies(bool abX){}
		bool GetCollideBodies(){return false;}
		void SetStiffness(float afX){}
		float GetStiffness(){return 0;}
		cVector3f GetVelocity(){return cVector3f();}
		cVector3f GetAngularVelocity(){return cVector3f();}
		cVector3f GetForce(){return cVector3f();}
		float GetDistance(){return 0;}
		float GetAngle(){return 0;}
		
		void SetMaxAngle(float afAngle){}
		void SetMinAngle(float afAngle){}
		float GetMaxAngle(){return 0;}
		float GetMinAngle(){return 0;}
	};

	class cPhysicsJointSliderNull : public iPhysicsJointSlider
	{
	public:
		cPhysicsJointSliderNull(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
			iPhysicsWorld *apWorld,const cVector3f &avPivotPoint, const cVector3f avPinDir)
			: iPhysicsJointSlider(asName, apParentBody, apChildBody, apWorld, avPivotPoint){}
		~cPhysicsJointSliderNull(){}
		
		void SetCollideBodies(bool abX){}
		bool GetCollideBodies(){return false;}
		void SetStiffness(float afX){}
		float GetStiffness(){return 0;}
		cVector3f GetVelocity(){return cVector3f();}
		cVector3f GetAngularVelocity(){return cVector3f();}
		cVector3f GetForce(){return cVector3f();}
		float GetDistance(){return 0;}
		float GetAngle(){return 0;}
		
		void SetMaxDistance(float afX){};
		void SetMinDistance(float afX){};
		float GetMaxDistance(){return 0;}
		float GetMinDistance(){return 0;}
	};

	class cPhysicsJointScrewNull : public iPhysicsJointScrew
	{
	public:
		cPhysicsJointScrewNull(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
			iPhysicsWorld *apWorld,const cVector3f &avPivotPoint, const cVector3f avPinDir)
			: iPhysicsJointScrew(asName, apParentBody, apChildBody, apWorld, avPivotPoint){}
		~cPhysicsJointScrewNull(){}
		
		void SetCollideBodies(bool abX){}
		bool GetCollideBodies(){return false;}
		void SetStiffness(float afX){}
		float GetStiffness(){return 0;}
		cVector3f GetVelocity(){return cVector3f();}
		cVector3f GetAngularVelocity(){return cVector3f();}
		cVector3f GetForce(){return cVector3f();}
		float GetDistance(){return 0;}
		float GetAngle(){return 0;}
		
		void SetMaxDistance(float afX){};
		void SetMinDistance(float afX){};
		float GetMaxDistance(){return 0;}
		float GetMinDistance(){return 0;}
	};

	class cPhysicsControllerNull : public iPhysicsController
	{
	public:
		cPhysicsControllerNull(const tString &asName, iPhysicsWorld *apWorld)
			: iPhysicsController(asName, apWorld){}
		~cPhysicsControllerNull(){}
	};

	class cPhysicsWorldNull : public iPhysicsWorld
	{
	public:
		cPhysicsWorldNull(){}
		~cPhysicsWorldNull(){}

		void Simulate(float afTimeStep){}

		void  SetMaxTimeStep(float afTimeStep){mfMaxTimeStep=afTimeStep;}
		float GetMaxTimeStep(){return mfMaxTimeStep;}

		void SetWorldSize(const cVector3f &avMin,const cVector3f &avMax)
		{mvWorldSizeMin=avMin;mvWorldSizeMax=avMax;}
		cVector3f GetWorldSizeMin(){return mvWorldSizeMin;}
		cVector3f GetWorldSizeMax(){return mvWorldSizeMax;}

		void SetGravity(const cVector3f& avGravity){mvGravity=avGravity;}
		cVector3f GetGravity(){return mvGravity;}

		void SetAccuracyLevel(ePhysicsAccuracy aAccuracy){mAccuracy=aAccuracy;}
		ePhysicsAccuracy GetAccuracyLevel(){return mAccuracy;}

		iCollideShape* CreateNullShape()
		{
			iCollideShape *pShape = hplNew( cCollideShapeNull, (eCollideShapeType_Null, 0, NULL,this) );
			mlstShapes.push_back(pShape);
			return pShape;
		}
		iCollideShape* CreateBoxShape(const cVector3f &avSize, cMatrixf* apOffsetMtx)
		{
			iCollideShape *pShape = hplNew( cCollideShapeNull, (eCollideShapeType_Box, avSize, apOffsetMtx,this) );
			mlstShapes.push_back(pShape);
			return pShape;
		}
		iCollideShape* CreateSphereShape(const cVector3f &avRadii, cMatrixf* apOffsetMtx)
		{
			iCollideShape *pShape = hplNew( cCollideShapeNull, (eCollideShapeType_Sphere, avRadii, apOffsetMtx,this) );
			mlstShapes.push_back(pShape);
			return pShape;
		}
		iCollideShape* CreateCylinderShape(float afRadius, float afHeight, cMatrixf* apOffsetMtx)
		{
			iCollideShape *pShape = hplNew( cCollideShapeNull, (eCollideShapeType_Cylinder,
													cVector3f(afRadius,afHeight,afRadius),
													apOffsetMtx,this) );
			mlstShapes.push_back(pShape);
			return pShape;
		}
		iCollideShape* CreateCapsuleShape(float afRadius, float afHeight, cMatrixf* apOffsetMtx)
		{
			iCollideShape *pShape = hplNew( cCollideShapeNull, (eCollideShapeType_Capsule,
													cVector3f(afRadius,afHeight,afRadius),
													apOffsetMtx, this) );
			mlstShapes.push_back(pShape);
			return pShape;
		}
		iCollideShape* CreateMeshShape(iVertexBuffer *apVtxBuffer)
		{
			cCollideShapeNull *pShape = hplNew( cCollideShapeNull, (eCollideShapeType_Mesh,
															0, NULL,this) );
			mlstShapes.push_back(pShape);
			return pShape;
		}
		iCollideShape* CreateCompundShape(tCollideShapeVec &avShapes)
		{
			cCollideShapeNull *pShape = hplNew( cCollideShapeNull, (eCollideShapeType_Compound,
															0, NULL, this) );
			mlstShapes.push_back(pShape);
			return pShape;
		}

		iPhysicsJointBall* CreateJointBall(const tString &asName,const cVector3f& avPivotPoint,
												iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)
		{
			iPhysicsJointBall *pJoint = hplNew( cPhysicsJointBallNull, (asName,apParentBody,apChildBody,this,avPivotPoint) );
			mlstJoints.push_back(pJoint);
			return pJoint;
		}
		iPhysicsJointHinge* CreateJointHinge(const tString &asName,const cVector3f& avPivotPoint,
												const cVector3f& avPinDir,
												iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)
		{
			iPhysicsJointHinge *pJoint = hplNew( cPhysicsJointHingeNull, (asName,apParentBody,apChildBody,this,avPivotPoint,avPinDir) );
			mlstJoints.push_back(pJoint);
			return pJoint;
		}
		iPhysicsJointSlider* CreateJointSlider(const tString &asName,const cVector3f& avPivotPoint,
												const cVector3f& avPinDir,
												iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)
		{
			iPhysicsJointSlider *pJoint = hplNew( cPhysicsJointSliderNull, (asName,apParentBody,apChildBody,this,avPivotPoint,avPinDir) );
			mlstJoints.push_back(pJoint);
			return pJoint;
		}
		iPhysicsJointScrew* CreateJointScrew(const tString &asName,const cVector3f& avPivotPoint,
												const cVector3f& avPinDir,
												iPhysicsBody* apParentBody, iPhysicsBody *apChildBody)
		{
			iPhysicsJointScrew *pJoint = hplNew( cPhysicsJointScrewNull, (asName,apParentBody,apChildBody,this,avPivotPoint,avPinDir) );
			mlstJoints.push_back(pJoint);
			return pJoint;
		}

		iPhysicsBody* CreateBody(const tString &asName,iCollideShape *apShape)
		{
			cPhysicsBodyNull *pBody = hplNew( cPhysicsBodyNull, (asName,this, apShape) );
			mlstBodies.push_back(pBody);
			if(mpWorld3D) mpWorld3D->GetPortalContainer()->AddEntity(pBody);
			return pBody;
		}

		iCharacterBody *CreateCharacterBody(const tString &asName, const cVector3f &avSize)
		{
			cCharacterBodyNull *pChar = hplNew( cCharacterBodyNull, (asName,this,avSize) );
			mlstCharBodies.push_back(pChar);
			return pChar;
		}

		iPhysicsMaterial* CreateMaterial(const tString &asName)
		{
			cPhysicsMaterialNull *pMaterial = hplNew( cPhysicsMaterialNull, (asName,this) );
			tPhysicsMaterialMap::value_type Val(asName,pMaterial);
			m_mapMaterials.insert(Val);
			pMaterial->UpdateMaterials();
			return pMaterial;
		}

		iPhysicsController *CreateController(const tString &asName)
		{
			iPhysicsController* pController = hplNew( cPhysicsControllerNull, (asName, this) );
			mlstControllers.push_back(pController);
			return pController;
		}

		void CastRay(iPhysicsRayCallback *apCallback,
							const cVector3f &avOrigin, const cVector3f& avEnd,
							bool abCalcDist, bool abCalcNormal, bool abCalcPoint,
							bool abUsePrefilter = false){}

		bool CheckShapeCollision(	iCollideShape* apShapeA, const cMatrixf& a_mtxA,
						iCollideShape* apShapeB, const cMatrixf& a_mtxB,
						cCollideData & aCollideData, int alMaxPoints=4){return false;}

		void RenderDebugGeometry(iLowLevelGraphics *apLowLevel, const cColor& aColor){}

	private:
		cVector3f mvWorldSizeMin;
		cVector3f mvWorldSizeMax;
		cVector3f mvGravity;
		float mfMaxTimeStep;
		ePhysicsAccuracy mAccuracy;
	};

	class cLowLevelPhysicsNull : public iLowLevelPhysics
	{
	public:
		cLowLevelPhysicsNull(){}
		~cLowLevelPhysicsNull(){}

		iPhysicsWorld* CreateWorld()
		{return hplNew( cPhysicsWorldNull, () );}
	};
};
#endif // HPL_LOWLEVELPHYSICS_NULL_H

