#pragma once

///функциональность обязательности
class CObligation
{
public:
	CObligation():m_bIsObligatory(true), m_fWeight(0.0){};
	virtual ~CObligation(void){};

	virtual void SetObligatory(BOOL bObligatroy){m_bIsObligatory = bObligatroy; };
	///снимает флаг обязательности
	virtual void SetUnObligatory(void){m_bIsObligatory=false;};
	///выставляет флаг обязательности
	virtual void SetObligatory(void){m_bIsObligatory=true;};
	///возвращает флаг обязательности
	virtual BOOL IsObligatory(void){return m_bIsObligatory;};
	///устанавливает вес индекса
	virtual void SetWeight(float fValue){m_fWeight=fValue;};
	///возвращает вес индекса
	virtual float GetWeight(void){return m_fWeight;};

protected:
	///флаг обязательности
	BOOL m_bIsObligatory;
	///вес
	float m_fWeight;
};
	
