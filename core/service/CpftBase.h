#ifndef CORE_SERVICE_CPTFBASE_H
#define CORE_SERVICE_CPTFBASE_H
#include "TypeDefine.h"
#include "AutoObject.h"
#include "StdLibInclude.h"
#include "service/IService.h"

namespace cptf{
namespace core{

	struct CptfServiceEntities 
	{
		AutoObjectEntry** autoObjMapFirst_;
		AutoObjectEntry** autoObjMapLast_;
	};
	typedef shared_ptr<CptfServiceEntities>  CptfServiceEntitiesPtr;

	struct CPTF_INTMAP_ENTRY
	{
		cptf::IID iid;       // the interface id (IID)
		cptf::ulong	address;
		//_ATL_CREATORARGFUNC* pFunc; //NULL:end, 1:offset, n:ptr
	};

#define  CPTF_PACKING 8
#define cptf_offsetofclass(base, derived) \
	((cptf::ulong)(static_cast<base*>((derived*)CPTF_PACKING))-CPTF_PACKING)


	inline bool cptfModuleGetClassObject(const CptfServiceEntities* cpfgModel
										, const cptf::IID& csid
										, const cptf::IID& iid
										, void** rtnObj)
	{
		bool rtn(false);
		assert(cpfgModel);
		for (AutoObjectEntry** entity = cpfgModel->autoObjMapFirst_
			; entity != cpfgModel->autoObjMapLast_; ++entity)
		{
			AutoObjectEntry* obj = *entity;
			if (obj == NULL) continue;
			if (obj->crateFunc != NULL && csid == obj->iid){
				rtn  = obj->crateFunc(iid, rtnObj);
				break;
			}
		}

		return rtn;
	}

	inline bool cptfInternalQueryInterface(void* pThis
										, const CPTF_INTMAP_ENTRY* pEntries
										, const cptf::IID& iid
										, void** rtnObj)
	{
		if(pThis == NULL || pEntries == NULL)
			return false ;

		*rtnObj = NULL;

		while (pEntries->address != -1)
		{
			if(iid == pEntries->iid){
				IService* service = (IService*)((ulong)pThis+pEntries->address);
				service->addRef();
				*rtnObj = service;
				return true;
			}
			pEntries++;
		}
		return false;
	}

#define CPTF_BEGIN_SERVICE_MAP(x) public: \
	typedef x _ComMapClass; \
	bool internalQueryInterface(const cptf::IID& iid, void** ppvObject) \
 	{ return cptfInternalQueryInterface(this, getEntries(), iid, ppvObject); } \
	const static CPTF_INTMAP_ENTRY* getEntries(){ \
	static const CPTF_INTMAP_ENTRY entries[] = {

#define CPTF_END_SERVICE_MAP() \
	{L"", 0}};\
	return &entries[0];}\
	virtual	cptf::ulong addRef() = 0;\
	virtual cptf::ulong release() = 0;\
	virtual bool queryInterface(const cptf::IID& iid, void**rntObj) = 0;


#define CPTF_SERVICE_INTERFACE_ENTRY(iid,x)\
	{iid, cptf_offsetofclass(x, _ComMapClass)},
}
}

#endif