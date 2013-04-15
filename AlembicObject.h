#ifndef _ALEMBIC_OBJECT_H_
#define _ALEMBIC_OBJECT_H_

#include "CommonSceneGraph.h"

class AlembicWriteJob;
class AlembicObject;

typedef boost::shared_ptr < AlembicObject > AlembicObjectPtr;

class AlembicObject
{
private:
   MObjectArray mRefs;
   AlembicWriteJob * mJob;
   AlembicObjectPtr mParent;
   Abc::OObject mMyParent;
   MString nodeName;

protected:
   SceneNodePtr mExoSceneNode;
   int mNumSamples;

public:
   AlembicObject(SceneNodePtr eNode, AlembicWriteJob * in_Job, Abc::OObject oParent);
   ~AlembicObject();

   AlembicWriteJob * GetJob() { return mJob; }
   const MObject & GetRef(ULONG index = 0) { return mRefs[index]; }
   ULONG GetRefCount() { return mRefs.length(); }
   void AddRef(const MObject & in_Ref) { mRefs.append(in_Ref); }

   Abc::OObject GetMyParent() { return mMyParent; }
   virtual Abc::OObject GetObject() = 0;
   Abc::OObject GetParentObject();

   bool IsParentedToRoot() const { return !mParent; }
   AlembicObjectPtr GetAlembicParentObject(void) { return mParent; }

   virtual Abc::OCompoundProperty GetCompound() = 0;
   int GetNumSamples() { return mNumSamples; }
   MString GetUniqueName(const MString & in_Name);

   virtual MStatus Save(double time) = 0;
};

class AlembicObjectNode : public MPxNode
{
public:
   AlembicObjectNode();
   virtual ~AlembicObjectNode();
   virtual void PreDestruction() = 0;
protected:
   unsigned int mRefId;
};

class AlembicObjectDeformNode : public MPxDeformerNode
{
public:
   AlembicObjectDeformNode();
   virtual ~AlembicObjectDeformNode();
   virtual void PreDestruction() = 0;
protected:
   unsigned int mRefId;
};

class AlembicObjectEmitterNode : public MPxEmitterNode
{
public:
   AlembicObjectEmitterNode();
   virtual ~AlembicObjectEmitterNode();
   virtual void PreDestruction() = 0;
protected:
   unsigned int mRefId;
};

class AlembicObjectLocatorNode : public MPxLocatorNode
{
public:
   AlembicObjectLocatorNode();
   virtual ~AlembicObjectLocatorNode();
   virtual void PreDestruction() = 0;
protected:
   unsigned int mRefId;
};

void preDestructAllNodes();

#include "AlembicWriteJob.h"

#endif