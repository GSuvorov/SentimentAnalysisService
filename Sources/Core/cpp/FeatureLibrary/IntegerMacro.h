#pragma once
#define INTEGER_FEATURE(BaseClass, GroupDescription, FeatureName, Description)\
	class FeatureName##Type;\
	typedef BaseClass<FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
			const char* GetTypeName(){return #BaseClass;};\
			const char* GetTypeDescription(){return GroupDescription;};\
			const char* GetValueName(unsigned int n){return #FeatureName;};\
			const char* GetValueDescription(unsigned int n){return Description;};\
	};
