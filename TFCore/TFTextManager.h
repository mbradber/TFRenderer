#pragma once
class TFTextManager
{
public:

	static TFTextManager* Instance();

	~TFTextManager(void);


private:
	TFTextManager(void);
	TFTextManager(const TFTextManager&);

	static TFTextManager m_pInstance;
};

