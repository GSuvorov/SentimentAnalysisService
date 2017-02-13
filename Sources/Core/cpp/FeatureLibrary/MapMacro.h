#pragma once
#define MAP_FEATURE1(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 1, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName##Type(void)\
			: name_1(0)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 1;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE2(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 2, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 2;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE3(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 3, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 3;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE4(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 4, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 4;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE5(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 5, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 5;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE6(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 6, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 6;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE7(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 7, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 7;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE8(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 8, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 8;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE9(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 9, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 9;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE10(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 10, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 10;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE11(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 11, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 11;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE12(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 12, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 12;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE13(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 13, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 13;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE14(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 14, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 14;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE15(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 15, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 15;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE16(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 16, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 16;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE17(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 17, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 17;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE18(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 18, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 18;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE19(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 19, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 19;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE20(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 20, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 20;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE21(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 21, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 21;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE22(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 22, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 22;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE23(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 23, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 23;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE24(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 24, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 24;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE25(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24,\
	name_25,\
	desc_25)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 25, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName name_25;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
			, name_25(24)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 25;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				case 24: return #name_25;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				case 24: return desc_25;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE26(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24,\
	name_25,\
	desc_25,\
	name_26,\
	desc_26)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 26, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName name_25;\
		FeatureName name_26;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
			, name_25(24)\
			, name_26(25)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 26;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				case 24: return #name_25;\
				case 25: return #name_26;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				case 24: return desc_25;\
				case 25: return desc_26;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE27(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24,\
	name_25,\
	desc_25,\
	name_26,\
	desc_26,\
	name_27,\
	desc_27)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 27, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName name_25;\
		FeatureName name_26;\
		FeatureName name_27;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
			, name_25(24)\
			, name_26(25)\
			, name_27(26)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 27;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				case 24: return #name_25;\
				case 25: return #name_26;\
				case 26: return #name_27;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				case 24: return desc_25;\
				case 25: return desc_26;\
				case 26: return desc_27;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE28(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24,\
	name_25,\
	desc_25,\
	name_26,\
	desc_26,\
	name_27,\
	desc_27,\
	name_28,\
	desc_28)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 28, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName name_25;\
		FeatureName name_26;\
		FeatureName name_27;\
		FeatureName name_28;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
			, name_25(24)\
			, name_26(25)\
			, name_27(26)\
			, name_28(27)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 28;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				case 24: return #name_25;\
				case 25: return #name_26;\
				case 26: return #name_27;\
				case 27: return #name_28;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				case 24: return desc_25;\
				case 25: return desc_26;\
				case 26: return desc_27;\
				case 27: return desc_28;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE29(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24,\
	name_25,\
	desc_25,\
	name_26,\
	desc_26,\
	name_27,\
	desc_27,\
	name_28,\
	desc_28,\
	name_29,\
	desc_29)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 29, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName name_25;\
		FeatureName name_26;\
		FeatureName name_27;\
		FeatureName name_28;\
		FeatureName name_29;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
			, name_25(24)\
			, name_26(25)\
			, name_27(26)\
			, name_28(27)\
			, name_29(28)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 29;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				case 24: return #name_25;\
				case 25: return #name_26;\
				case 26: return #name_27;\
				case 27: return #name_28;\
				case 28: return #name_29;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				case 24: return desc_25;\
				case 25: return desc_26;\
				case 26: return desc_27;\
				case 27: return desc_28;\
				case 28: return desc_29;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE30(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24,\
	name_25,\
	desc_25,\
	name_26,\
	desc_26,\
	name_27,\
	desc_27,\
	name_28,\
	desc_28,\
	name_29,\
	desc_29,\
	name_30,\
	desc_30)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 30, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName name_25;\
		FeatureName name_26;\
		FeatureName name_27;\
		FeatureName name_28;\
		FeatureName name_29;\
		FeatureName name_30;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
			, name_25(24)\
			, name_26(25)\
			, name_27(26)\
			, name_28(27)\
			, name_29(28)\
			, name_30(29)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 30;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				case 24: return #name_25;\
				case 25: return #name_26;\
				case 26: return #name_27;\
				case 27: return #name_28;\
				case 28: return #name_29;\
				case 29: return #name_30;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				case 24: return desc_25;\
				case 25: return desc_26;\
				case 26: return desc_27;\
				case 27: return desc_28;\
				case 28: return desc_29;\
				case 29: return desc_30;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE31(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24,\
	name_25,\
	desc_25,\
	name_26,\
	desc_26,\
	name_27,\
	desc_27,\
	name_28,\
	desc_28,\
	name_29,\
	desc_29,\
	name_30,\
	desc_30,\
	name_31,\
	desc_31)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 31, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName name_25;\
		FeatureName name_26;\
		FeatureName name_27;\
		FeatureName name_28;\
		FeatureName name_29;\
		FeatureName name_30;\
		FeatureName name_31;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
			, name_25(24)\
			, name_26(25)\
			, name_27(26)\
			, name_28(27)\
			, name_29(28)\
			, name_30(29)\
			, name_31(30)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 31;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				case 24: return #name_25;\
				case 25: return #name_26;\
				case 26: return #name_27;\
				case 27: return #name_28;\
				case 28: return #name_29;\
				case 29: return #name_30;\
				case 30: return #name_31;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				case 24: return desc_25;\
				case 25: return desc_26;\
				case 26: return desc_27;\
				case 27: return desc_28;\
				case 28: return desc_29;\
				case 29: return desc_30;\
				case 30: return desc_31;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

#define MAP_FEATURE32(\
	FeatureName,\
	Description,\
	BitSize,\
	name_1,\
	desc_1,\
	name_2,\
	desc_2,\
	name_3,\
	desc_3,\
	name_4,\
	desc_4,\
	name_5,\
	desc_5,\
	name_6,\
	desc_6,\
	name_7,\
	desc_7,\
	name_8,\
	desc_8,\
	name_9,\
	desc_9,\
	name_10,\
	desc_10,\
	name_11,\
	desc_11,\
	name_12,\
	desc_12,\
	name_13,\
	desc_13,\
	name_14,\
	desc_14,\
	name_15,\
	desc_15,\
	name_16,\
	desc_16,\
	name_17,\
	desc_17,\
	name_18,\
	desc_18,\
	name_19,\
	desc_19,\
	name_20,\
	desc_20,\
	name_21,\
	desc_21,\
	name_22,\
	desc_22,\
	name_23,\
	desc_23,\
	name_24,\
	desc_24,\
	name_25,\
	desc_25,\
	name_26,\
	desc_26,\
	name_27,\
	desc_27,\
	name_28,\
	desc_28,\
	name_29,\
	desc_29,\
	name_30,\
	desc_30,\
	name_31,\
	desc_31,\
	name_32,\
	desc_32)\
	class FeatureName##Type;\
	typedef CMapFeature<BitSize, 32, FeatureName##Type> FeatureName;\
	class FeatureName##Type\
		: public CBaseType\
	{\
	public:\
		FeatureName name_1;\
		FeatureName name_2;\
		FeatureName name_3;\
		FeatureName name_4;\
		FeatureName name_5;\
		FeatureName name_6;\
		FeatureName name_7;\
		FeatureName name_8;\
		FeatureName name_9;\
		FeatureName name_10;\
		FeatureName name_11;\
		FeatureName name_12;\
		FeatureName name_13;\
		FeatureName name_14;\
		FeatureName name_15;\
		FeatureName name_16;\
		FeatureName name_17;\
		FeatureName name_18;\
		FeatureName name_19;\
		FeatureName name_20;\
		FeatureName name_21;\
		FeatureName name_22;\
		FeatureName name_23;\
		FeatureName name_24;\
		FeatureName name_25;\
		FeatureName name_26;\
		FeatureName name_27;\
		FeatureName name_28;\
		FeatureName name_29;\
		FeatureName name_30;\
		FeatureName name_31;\
		FeatureName name_32;\
		FeatureName##Type(void)\
			: name_1(0)\
			, name_2(1)\
			, name_3(2)\
			, name_4(3)\
			, name_5(4)\
			, name_6(5)\
			, name_7(6)\
			, name_8(7)\
			, name_9(8)\
			, name_10(9)\
			, name_11(10)\
			, name_12(11)\
			, name_13(12)\
			, name_14(13)\
			, name_15(14)\
			, name_16(15)\
			, name_17(16)\
			, name_18(17)\
			, name_19(18)\
			, name_20(19)\
			, name_21(20)\
			, name_22(21)\
			, name_23(22)\
			, name_24(23)\
			, name_25(24)\
			, name_26(25)\
			, name_27(26)\
			, name_28(27)\
			, name_29(28)\
			, name_30(29)\
			, name_31(30)\
			, name_32(31)\
		{\
		}\
		~FeatureName##Type(void){};\
			unsigned int GetValueCount(){\
				return 32;};\
			const char* GetTypeName(){return #FeatureName;};\
			const char* GetTypeDescription(){return Description;};\
			const char* GetValueName(unsigned int n){\
				switch(n){\
				case 0: return #name_1;\
				case 1: return #name_2;\
				case 2: return #name_3;\
				case 3: return #name_4;\
				case 4: return #name_5;\
				case 5: return #name_6;\
				case 6: return #name_7;\
				case 7: return #name_8;\
				case 8: return #name_9;\
				case 9: return #name_10;\
				case 10: return #name_11;\
				case 11: return #name_12;\
				case 12: return #name_13;\
				case 13: return #name_14;\
				case 14: return #name_15;\
				case 15: return #name_16;\
				case 16: return #name_17;\
				case 17: return #name_18;\
				case 18: return #name_19;\
				case 19: return #name_20;\
				case 20: return #name_21;\
				case 21: return #name_22;\
				case 22: return #name_23;\
				case 23: return #name_24;\
				case 24: return #name_25;\
				case 25: return #name_26;\
				case 26: return #name_27;\
				case 27: return #name_28;\
				case 28: return #name_29;\
				case 29: return #name_30;\
				case 30: return #name_31;\
				case 31: return #name_32;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
			const char* GetValueDescription(unsigned int n){\
				switch(n){\
				case 0: return desc_1;\
				case 1: return desc_2;\
				case 2: return desc_3;\
				case 3: return desc_4;\
				case 4: return desc_5;\
				case 5: return desc_6;\
				case 6: return desc_7;\
				case 7: return desc_8;\
				case 8: return desc_9;\
				case 9: return desc_10;\
				case 10: return desc_11;\
				case 11: return desc_12;\
				case 12: return desc_13;\
				case 13: return desc_14;\
				case 14: return desc_15;\
				case 15: return desc_16;\
				case 16: return desc_17;\
				case 17: return desc_18;\
				case 18: return desc_19;\
				case 19: return desc_20;\
				case 20: return desc_21;\
				case 21: return desc_22;\
				case 22: return desc_23;\
				case 23: return desc_24;\
				case 24: return desc_25;\
				case 25: return desc_26;\
				case 26: return desc_27;\
				case 27: return desc_28;\
				case 28: return desc_29;\
				case 29: return desc_30;\
				case 30: return desc_31;\
				case 31: return desc_32;\
				default: SS_THROW(L"некорректный номер значения");\
				}\
			};\
	};\

