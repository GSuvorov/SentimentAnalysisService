//UnitsExpert.h
#pragma once
#include "./BaseExpertConstruction.h"

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	///Эксперт для конструкции из базы
	class CUnitsExpert : public CBaseExpertConstruction
	{
	private:
		///Провести анализ предложения
		inline void ExecuteAnalysis(PBLOCK pBlock);

	public:
		///Конструктор
		CUnitsExpert( SS::SyntaxConstructions::Types::CConstructionEx & oConstruction,
					  SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager,
					  SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams,
					  int iIdGroup, ESyntaxBase eBase )
					  :
					  CBaseExpertConstruction(oConstruction, pLoadManager, pAnalyseParams, iIdGroup, eBase) {};
		///Деструктор
		~CUnitsExpert() {};
		///Проход по доске
		bool PassBoard(CBoard & oBoard, PBLOCK pBlock);
	};

}
}
}