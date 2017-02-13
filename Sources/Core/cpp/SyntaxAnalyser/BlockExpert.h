//BlockExpert.h
#pragma once
#include "./BaseExpertConstruction.h"

namespace SS
{
namespace Syntax
{
namespace ExpertModel
{
	///Експерт для работы с блоками
	class CBlockExpert : public CBaseExpertConstruction
	{
	private:
		///Указатель на пришедшее предложение
		CBoard* m_pBoard;
		///Блок с детект-элементом
		PBLOCK m_pBlockDetect;
		///Блок справа от детект
		PBLOCK m_pBlockRight;
		///Блок слева от детект
		PBLOCK m_pBlockLeft;
		///Список сработавших блоков
		std::list<PBLOCK> m_UnionBlocks; 

	private:
		///Провести анализ предложения
		inline void ExecuteAnalysis();
		///Провести анализ детект-элемента
		inline bool ExecuteDetect(IObjectAnalyser* pObjectAnalyser);
		///Провести анализ остальных элементов
		inline bool ExecuteOther(IObjectAnalyser* pObjectAnalyser);
		///Обьединить блоки
		void UniteBlocks();

	public:
		///Конструктор
		CBlockExpert( SS::SyntaxConstructions::Types::CConstructionEx & oConstruction,
					  SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager,
					  SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams,
					  int IdGroup, ESyntaxBase eBase )
					  :
					  CBaseExpertConstruction(oConstruction, pLoadManager, pAnalyseParams, IdGroup, eBase),
					  m_pBoard(NULL), m_pBlockDetect(NULL), m_pBlockRight(NULL), m_pBlockLeft(NULL) {};
		///Деструктор
		~CBlockExpert(){};
		///Проход по доске 
		bool PassBoard(CBoard & oBoard, PBLOCK pBlock);
	};

}
}
}